#include "GpuImageTransformer.hpp"
#include "QImageWrapper.hpp"
#include "Log.hpp"
#include "Mask.hpp"

#include <QMessageBox>

#define ASSERT_NO_ERROR(err, functionName) \
    if (checkError(err, functionName) == true) return;

namespace image_transforming
{

bool GpuImageTransformer::checkError(cl_int err, std::optional<QString> functionName)
{
    if (err != CL_SUCCESS)
    {
        QMessageBox errorMessageBox;

        QString content =
            "OpenCL Error:\n" +
            QString::fromStdString(OpenCLErrors::toString(err)) +
            "\n\nDescription:\n" +
            QString::fromStdString(OpenCLErrors::getDescription(err));

        if (functionName.has_value())
            content += ("\n\nFunction name:\n" + *functionName);

        errorMessageBox.setText(content);
        errorMessageBox.exec();

        return true;
    }
    return false;
}

GpuImageTransformer::GpuImageTransformer(QImageWrapper &imageWrapper)
    : imageWrapper{imageWrapper}
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<cl::Device> devices;
    platforms.front().getDevices(CL_DEVICE_TYPE_GPU, &devices);

    if (devices.empty())
    {
        QMessageBox errorMessageBox;
        errorMessageBox.setText("GPU device not found on this platform.");
        errorMessageBox.exec();
        isGpuAvailable = false;
        return;
    }
    isGpuAvailable = true;

    cl::Context contextWrapper(devices.front());
    device = devices.front().get();

    context = contextWrapper.get();

    cl_int err;

    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxWorkItemDimensions), &maxWorkItemDimensions, NULL);
    ASSERT_NO_ERROR(err, "clGetDeviceInfo");
    LOG_INFO("CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: {}", maxWorkItemDimensions);

    maxWorkItemSizes.resize(maxWorkItemDimensions);
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * maxWorkItemDimensions, maxWorkItemSizes.data(), NULL);
    ASSERT_NO_ERROR(err, "clGetDeviceInfo");
    std::string sizes = "";
    for (const auto& s : maxWorkItemSizes) sizes += (std::to_string(s) + " ");
    LOG_INFO("CL_DEVICE_MAX_WORK_ITEM_SIZES: {}", sizes);

    err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
    ASSERT_NO_ERROR(err, "clGetDeviceInfo");
    LOG_INFO("CL_DEVICE_MAX_COMPUTE_UNITS: {}", maxComputeUnits);

    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
    ASSERT_NO_ERROR(err, "clGetDeviceInfo");
    LOG_INFO("CL_DEVICE_MAX_WORK_GROUP_SIZE: {}", maxWorkGroupSize);

    cl_device_fp_config deviceFpConfig;
    err = clGetDeviceInfo(device, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(deviceFpConfig), &deviceFpConfig, NULL);
    ASSERT_NO_ERROR(err, "clGetDeviceInfo");
    LOG_INFO("CL_DEVICE_DOUBLE_FP_CONFIG: {}", deviceFpConfig);

    setupKernels();
    setupQueues();

    err = clGetKernelWorkGroupInfo(kernelApplyFilterInt, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(preferredWorkGroupSizeMultiple), &preferredWorkGroupSizeMultiple, NULL);
    ASSERT_NO_ERROR(err, "clGetKernelWorkGroupInfo");
    LOG_INFO("CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: {}", preferredWorkGroupSizeMultiple);
}

GpuImageTransformer::~GpuImageTransformer()
{
    LOG_INFO("Destroying GpuImageTransformer");
    if (isGpuAvailable)
    {
        releaseDevice();
        releaseContext();
        releaseKernels();
        releaseQueues();
    }
}

void GpuImageTransformer::rotate90()
{
    cl_int err;
    queueRotate90 = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue");

    const GprData::DataType* src = imageWrapper.getPreviousImageData().getData();
    size_t sizeInBytes = imageWrapper.getPreviousImageData().getSizeInBytes();
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: input");

    const cl_uint newHeight = imageWrapper.width();
    const cl_uint newWidth = imageWrapper.height();
    ImageData newData(newWidth, newHeight);

    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: output");

    err = clEnqueueWriteBuffer(queueRotate90, input, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

    const cl_uint workGroupSize = 8;

    open_cl_utils::setKernelArgs(kernelRotate90, input, output, newWidth, newHeight, workGroupSize);

    size_t localws[2] = {workGroupSize, workGroupSize};
    size_t globalws[2] = {upToMultipleOf(workGroupSize, newWidth/workGroupSize), upToMultipleOf(workGroupSize, newHeight/workGroupSize)};

    err = clEnqueueNDRangeKernel(queueRotate90, kernelRotate90, 2, 0, globalws, localws, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueNDRangeKernel");

    GprData::DataType* newRawData = newData.getData();
    err = clEnqueueReadBuffer(queueRotate90, output, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);

    imageWrapper.setNewImage(std::move(newData));

    err = clReleaseMemObject(input);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
    err = clReleaseMemObject(output);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
}

void GpuImageTransformer::gain(int from, int to, double gainLower, double gainUpper)
{
    if (from < 0 or to >= imageWrapper.height() or from >= to)
        return;

    cl_int err;

    size_t sizeInBytes = imageWrapper.getPreviousImageData().getSizeInBytes();
    if (isGainCommited)
    {
        const GprData::DataType* src = imageWrapper.getPreviousImageData().getData();
        inputGain = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
        ASSERT_NO_ERROR(err, "clCreateBuffer: inputGain");

        outputGain = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
        ASSERT_NO_ERROR(err, "clCreateBuffer: outputGain");

        err = clEnqueueWriteBuffer(queueGain, inputGain, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
        ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

        isGainCommited = false;
    }


    cl_float step = (gainUpper - gainLower) / (to - from + 1);
    cl_uint width = imageWrapper.width();
    cl_uint height = imageWrapper.height();
    cl_uint from_ = static_cast<cl_uint>(from);
    cl_float gainLower_ = static_cast<cl_float>(gainLower);
    constexpr cl_uint rowsPerWorkItem = 1;

    err = open_cl_utils::setKernelArgs(kernelLinearGain, inputGain, outputGain, width, height, from_, step, gainLower_, rowsPerWorkItem);
    ASSERT_NO_ERROR(err, "setKernelArgs");

    size_t localws[1] = {preferredWorkGroupSizeMultiple};
    size_t globalws[1] = {upToMultipleOf(preferredWorkGroupSizeMultiple, (to - from + 1)/rowsPerWorkItem + 1u)};
    err = clEnqueueNDRangeKernel(queueGain, kernelLinearGain, 1, 0, globalws, localws, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueNDRangeKernel");

    ImageData newImageData{imageWrapper.getPreviousImageData()};
    GprData::DataType* newRawData = newImageData.getData();
    err = clEnqueueReadBuffer(queueGain, outputGain, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueReadBuffer");

    ImageData finalImageData{imageWrapper.getPreviousImageData()};

    std::memcpy(
        static_cast<void*>(&finalImageData.at(from, 0)),
        static_cast<void*>(&newImageData.at(from, 0)),
        width * (to - from) * sizeof(GprData::DataType));

    imageWrapper.setNewImage(std::move(finalImageData));
}

void GpuImageTransformer::equalizeHistogram(int from, int to)
{
    ImageTransformer::equalizeHistogram(imageWrapper, from, to);
}

void GpuImageTransformer::applyFilter(const details::Mask<int>& mask)
{
    cl_int err;

    const GprData::DataType* src = imageWrapper.getPreviousImageData().getData();
    const size_t sizeInBytes = imageWrapper.getPreviousImageData().getSizeInBytes();
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: input");

    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: output");

    cl_mem inputMask = clCreateBuffer(context, CL_MEM_READ_ONLY, mask.width * mask.height * sizeof(int), NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: inputMask");

    err = clEnqueueWriteBuffer(queueFilterInt, inputMask, CL_TRUE, 0, mask.length * sizeof(int), (void*)mask.values.data(), 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

    err = clEnqueueWriteBuffer(queueFilterInt, input, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

    cl_uint width = imageWrapper.width();
    cl_uint height = imageWrapper.height();
    cl_uint maskHeight = static_cast<cl_uint>(mask.height);
    cl_uint maskWidth = static_cast<cl_uint>(mask.width);
    constexpr cl_uint rowsPerWorkItem = 1;

    err = open_cl_utils::setKernelArgs(kernelApplyFilterInt, input, output, width, height, inputMask, maskHeight, maskWidth, rowsPerWorkItem);
    ASSERT_NO_ERROR(err, "setKernelArgs");

    size_t localws[1] = {preferredWorkGroupSizeMultiple};
    size_t globalws[1] = {upToMultipleOf(preferredWorkGroupSizeMultiple, height/rowsPerWorkItem)};
    timer.start(fmt::format("GPU applyFilter int {}x{}", mask.height, mask.width));
    err = clEnqueueNDRangeKernel(queueFilterInt, kernelApplyFilterInt, 1, 0, globalws, localws, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueNDRangeKernel");

    LOG_INFO("width: {}, height: {}, maskHeight: {}, maskWidth: {}", width, height, maskHeight, maskWidth);

    ImageData newImageData{imageWrapper.getPreviousImageData()};
    GprData::DataType* newRawData = newImageData.getData();
    err = clEnqueueReadBuffer(queueFilterInt, output, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);
    timer.stop();
    ASSERT_NO_ERROR(err, "clEnqueueReadBuffer");

    int midHeight = mask.height/2;
    int midWidth = mask.width/2;
    fillEdges(newImageData, midHeight, midWidth);
    imageWrapper.setNewImage(std::move(newImageData));

    err = clReleaseMemObject(input);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
    err = clReleaseMemObject(output);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
    err = clReleaseMemObject(inputMask);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
}

void GpuImageTransformer::applyFilter(const details::Mask<double>&)
{

}

void GpuImageTransformer::applyFilter(const Mask &mask)
{
    std::visit([this] (const auto& mask) { applyFilter(mask); }, mask);
}

void GpuImageTransformer::backgroundRemoval()
{
    cl_int err;

    const GprData::DataType* src = imageWrapper.getPreviousImageData().getData();
    const size_t sizeInBytes = imageWrapper.getPreviousImageData().getSizeInBytes();
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: input");

    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: output");

    err = clEnqueueWriteBuffer(queueFilterInt, input, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

    cl_uint width = imageWrapper.width();
    cl_uint height = imageWrapper.height();
    constexpr cl_uint rowsPerWorkItem = 1;

    err = open_cl_utils::setKernelArgs(kernelBackgroundRemoval, input, output, width, height, rowsPerWorkItem);
    ASSERT_NO_ERROR(err, "setKernelArgs");

    size_t localws[1] = {preferredWorkGroupSizeMultiple};
    size_t globalws[1] = {upToMultipleOf(preferredWorkGroupSizeMultiple, height/rowsPerWorkItem)};
    err = clEnqueueNDRangeKernel(queueBackgroundRemoval, kernelBackgroundRemoval, 1, 0, globalws, localws, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueNDRangeKernel");

    ImageData newImageData{imageWrapper.getPreviousImageData()};
    GprData::DataType* newRawData = newImageData.getData();
    err = clEnqueueReadBuffer(queueBackgroundRemoval, output, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueReadBuffer");

    imageWrapper.setNewImage(std::move(newImageData));

    err = clReleaseMemObject(input);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
    err = clReleaseMemObject(output);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
}

void GpuImageTransformer::trimTop()
{
    ImageTransformer::trimTop(imageWrapper);
}

void GpuImageTransformer::commitChanges(Operation operation)
{
    switch (operation)
    {
    case Operation::gain:
    {
        if (not isGainCommited)
        {
            cl_int err;
            err = clReleaseMemObject(inputGain);
            ASSERT_NO_ERROR(err, "clReleaseMemObject");
            err = clReleaseMemObject(outputGain);
            ASSERT_NO_ERROR(err, "clReleaseMemObject");

            isGainCommited = true;
        }
        return;
    }
    default: return;
    }
}

void GpuImageTransformer::setupKernels()
{
    for (const auto& kernel : kernels)
    {
        cl_program program = buildProgram(context, kernel.programName);
        if (program == nullptr)
            return;
        for (const auto& [kernelName, kernelPtr] : kernel.functions)
        {
            cl_kernel kernel = createKernel(program, kernelName.c_str());
            if (kernel == nullptr)
                return;
            *(kernelPtr) = kernel;
        }
        cl_int err = clReleaseProgram(program);
        ASSERT_NO_ERROR(err, "clReleaseProgram");
    }
}

void GpuImageTransformer::setupQueues()
{
    cl_int err;
    queueRotate90 = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue queueRotate90");

    queueGain = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue queueGain");

    queueFilterInt = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue queueFilterInt");

    queueBackgroundRemoval = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue queueBackgroundRemoval");
}

cl_program GpuImageTransformer::buildProgram(cl_context context, const QString &fileName)
{
    QFile file(kernelsLocation + fileName);
    file.open(QFile::ReadOnly);
    std::string fileContentStr = file.readAll().toStdString();
    const char* fileContent = fileContentStr.c_str();
    cl_int err = 0;
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&fileContent, 0, &err);
    if (err != 0)
    {
        checkError(err, "clCreateProgramWithSource err: ");
        return nullptr;
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != 0)
    {
        constexpr std::size_t bufferSize = 40000;
        char* buffer = (char*)std::calloc(bufferSize, 1);
        size_t size;
        cl_int err2 = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, bufferSize - 1, buffer, &size);
        qDebug() << size;
        qDebug() << buffer;

        checkError(err, "clBuildProgram");
        checkError(err2);
        return nullptr;
    }
    return program;
}

cl_kernel GpuImageTransformer::createKernel(cl_program program, const char *functionName)
{
    cl_int err;
    cl_kernel kernel = clCreateKernel(program, functionName, &err);
    if (err != 0)
    {
        checkError(err, "clCreateKernel");
        return nullptr;
    }
    return kernel;
}

size_t GpuImageTransformer::upToMultipleOf(int multiplier, size_t value)
{
    if (multiplier <= 0 or value % multiplier == 0)
        return value;

    return value + (multiplier - value % multiplier);
}

void GpuImageTransformer::releaseDevice()
{
    cl_int err;
    err = clReleaseDevice(device);
    ASSERT_NO_ERROR(err, "clReleaseDevice");
}

void GpuImageTransformer::releaseContext()
{
    cl_int err;
    err = clReleaseContext(context);
}

void GpuImageTransformer::releaseKernels()
{
    cl_int err;
    for (const auto& kernel : kernels)
    {
        for (const auto& [_, kernelPtr] : kernel.functions)
        {
            err = clReleaseKernel(*kernelPtr);
            ASSERT_NO_ERROR(err, "clReleaseKernel");
        }
    }
}

void GpuImageTransformer::releaseQueues()
{
    cl_int err;

    err = clReleaseCommandQueue(queueRotate90);
    ASSERT_NO_ERROR(err, "clReleaseCommandQueue queueRotate90");

    err = clReleaseCommandQueue(queueGain);
    ASSERT_NO_ERROR(err, "clReleaseCommandQueue queueGain");

    err = clReleaseCommandQueue(queueFilterInt);
    ASSERT_NO_ERROR(err, "clReleaseCommandQueue queueFilterInt");

    err = clReleaseCommandQueue(queueBackgroundRemoval);
    ASSERT_NO_ERROR(err, "clReleaseCommandQueue queueBackgroundRemoval");
}
} // namespace image_transforming
