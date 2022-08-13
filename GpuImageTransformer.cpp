#include "GpuImageTransformer.hpp"
#include "QImageWrapper.hpp"
#include "OpenCLErrors.hpp"
#include "OpenCLUtils.hpp"

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

    cl::Context contextWrapper(devices.front());
    device = devices.front().get();

    context = contextWrapper.get();

    setupKernels(context);

    qDebug() << "created GpuImageTransformer";
}

void GpuImageTransformer::rotate90()
{
    cl_int err;
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue");

    const GprData::DataType* src = imageWrapper.getImageData().getData();
    size_t sizeInBytes = imageWrapper.getImageData().getSizeInBytes();
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: input");

    const cl_uint newHeight = imageWrapper.width();
    const cl_uint newWidth = imageWrapper.height();
    ImageData newData(newWidth, newHeight);

    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: output");

    err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

    const cl_uint workGroupSize = 8;

    open_cl_utils::setKernelArgs(kernelRotate90, input, output, newWidth, newHeight, workGroupSize);

    size_t localws[2] = {workGroupSize, workGroupSize};
    size_t globalws[2] = {upToMultipleOf(workGroupSize, newWidth/workGroupSize), upToMultipleOf(workGroupSize, newHeight/workGroupSize)};

    err = clEnqueueNDRangeKernel(queue, kernelRotate90, 2, 0, globalws, localws, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueNDRangeKernel");

    GprData::DataType* newRawData = newData.getData();
    err = clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);

    imageWrapper.setNewImage(std::move(newData));
    imageWrapper.updatePreviousImage();

    qDebug() << "rotate90 on GPU done";
}

void GpuImageTransformer::changeContrast(float contrast)
{

}

void GpuImageTransformer::gain(int from, int to, float value)
{

}

void GpuImageTransformer::gain(int from, int to, double gainLower, double gainUpper)
{
    if (from < 0 or to >= imageWrapper.height())
        return;

    cl_int err;
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    ASSERT_NO_ERROR(err, "clCreateCommandQueue");

    const GprData::DataType* src = imageWrapper.getOriginalImageData().getData();
    size_t sizeInBytes = imageWrapper.getImageData().getSizeInBytes();
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: input");

    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
    ASSERT_NO_ERROR(err, "clCreateBuffer: output");

    err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueWriteBuffer");

    cl_float step = (gainUpper - gainLower) / (to - from);
    cl_uint width = imageWrapper.width();
    cl_uint height = imageWrapper.height();
    cl_uint from_ = static_cast<cl_uint>(from);
    cl_float gainLower_ = static_cast<cl_float>(gainLower);

    open_cl_utils::setKernelArgs(kernelLinearGain, input, output, width, height, from_, step, gainLower_);
    ASSERT_NO_ERROR(err, "setKernelArgs");

    size_t localws[1] = {1u};
    size_t globalws[1] = {static_cast<size_t>(to - from + 1)};
    err = clEnqueueNDRangeKernel(queue, kernelLinearGain, 1, 0, globalws, localws, 0, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueNDRangeKernel");

    ImageData newImageData{imageWrapper.getOriginalImageData()};
    GprData::DataType* newRawData = newImageData.getData();
    err = clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);
    ASSERT_NO_ERROR(err, "clEnqueueReadBuffer");

    ImageData finalImageData{imageWrapper.getOriginalImageData()};

    std::memcpy(
        static_cast<void*>(&finalImageData.at(from, 0)),
        static_cast<void*>(&newImageData.at(from, 0)),
        width * (to - from) * sizeof(GprData::DataType));

    imageWrapper.setNewImage(std::move(finalImageData));

    err = clReleaseMemObject(input);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
    err = clReleaseMemObject(output);
    ASSERT_NO_ERROR(err, "clReleaseMemObject");
    err = clReleaseCommandQueue(queue);
    ASSERT_NO_ERROR(err, "clReleaseCommandQueue");


    /*ImageData newImageData{imageWrapper.getOriginalImageData()};

    double step = (gainUpper - gainLower) / (to - from);
    double gain = gainLower;
    for (int i = from; i < to; i++)
    {
        gain += step;
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            std::uint64_t tempNewValue = static_cast<std::uint64_t>(newImageData.at(i, j) * gain);
            GprData::DataType newValue = tempNewValue <= limits::max() ? tempNewValue : limits::max();
            newImageData.at(i, j) = newValue;
        }
    }

    imageWrapper.setNewImage(std::move(newImageData));*/
}

void GpuImageTransformer::equalizeHistogram(int from, int to)
{

}

void GpuImageTransformer::applyFilter(const Mask &mask)
{

}

void GpuImageTransformer::backgroundRemoval()
{

}

void GpuImageTransformer::trimTop()
{

}

void GpuImageTransformer::setupKernels(cl_context context)
{
    for (const auto& kernel : kernels)
    {
        cl_program program = buildProgram(context, kernel.programName);
        if (program == nullptr)
            return;
        for (const auto& function : kernel.functions)
        {
            cl_kernel kernel = createKernel(program, function.first.c_str());
            if (kernel == nullptr)
                return;
            *(function.second) = kernel;
        }
    }
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
        char* buffer = (char*)std::calloc(40000, 1);
        size_t size;
        cl_int err2 = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 40000 - 1, buffer, &size);
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
} // namespace image_transforming
