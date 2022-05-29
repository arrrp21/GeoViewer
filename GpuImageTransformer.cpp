#include "GpuImageTransformer.hpp"
#include "QImageWrapper.hpp"

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

    cl_int err;

    cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0 , &err);
    if (err != 0)
    {
        qDebug () << "clCreateCommandQueue err: " << err;
    }

    setupKernels(context);

    qDebug() << "created GpuImageTransformer";
}

void GpuImageTransformer::rotate90()
{
    cl_int err;
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != 0)
    {
        qDebug () << "clCreateCommandQueue err: " << err;
    }

    const GprData::DataType* src = imageWrapper.getImageData().getData();
    size_t sizeInBytes = imageWrapper.getImageData().getSizeInBytes();
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &err);
    if (err != 0)
    {
        qDebug() << "input clCreateBuffer err: " << err;
    }

    const int newHeight = imageWrapper.width();
    const int newWidth = imageWrapper.height();
    ImageData newData(newWidth, newHeight);

    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &err);
    if (err != 0)
    {
        qDebug() << "output clCreateBuffer err: " << err;
    }

    err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, sizeInBytes, (void*)src, 0, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clEnqueueWriteBuffer err: " << err;
    }

    clSetKernelArg(kernelRotate90, 0, sizeof(cl_mem), (void*)&input);
    clSetKernelArg(kernelRotate90, 1, sizeof(cl_mem), (void*)&output);
    clSetKernelArg(kernelRotate90, 2, sizeof(cl_int), (void*)&newWidth);
    clSetKernelArg(kernelRotate90, 3, sizeof(cl_int), (void*)&newHeight);

    size_t localws[2] = {8, 8};
    size_t globalws[2] = {static_cast<size_t>(newWidth/2), static_cast<size_t>(newHeight/2)};

    err = clEnqueueNDRangeKernel(queue, kernelRotate90, 2, 0, globalws, localws, 0, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clEnqueueNDRangeKernel err: " << err;
    }

    GprData::DataType* newRawData = newData.getData();
    err = clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeInBytes, (void*)newRawData, NULL, NULL, NULL);

    imageWrapper.changeOriginalImageData(newData);
    imageWrapper.setNewImage(std::move(newData));

    qDebug() << "rotate90 on GPU done";

    /*const uchar* oldData = imageWrapper.getImage().constBits();
    const GprData::DataType* castedOldData = reinterpret_cast<const GprData::DataType*>(oldData);

    const int newHeight = imageWrapper.width();
    const int newWidth = imageWrapper.height();

    qDebug() << "newWidth: " << newWidth << ", newHeight: " << newHeight;

    ImageData newData(newWidth, newHeight);
    qDebug() << "newWidth: " << newData.getWidth() << ", newHeight: " << newData.getHeight();
    GprData::DataType* castedNewData = newData.getData();

    for (int h = 0; h < newHeight; h++)
        for (int w = 0; w < newWidth; w++)
            castedNewData[h * newWidth + w] = castedOldData[(newWidth - w - 1) * newHeight + h];

    imageWrapper.changeOriginalImageData(newData);
    imageWrapper.setNewImage(std::move(newData));*/
}

void GpuImageTransformer::changeContrast(float contrast)
{

}

void GpuImageTransformer::gain(int from, int to, float value)
{

}

void GpuImageTransformer::gain(int from, int to, double gainLower, double gainUpper)
{

}

void GpuImageTransformer::equalizeHistogram(int from, int to)
{

}

void GpuImageTransformer::applyFilter(const Mask &mask)
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
        qDebug() << "clCreateProgramWithSource err: " << err;
        return nullptr;
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clBuildProgram err: " << err;
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
        qDebug() << "clCreateKernel err: " << err;
        return nullptr;
    }
    return kernel;
}
