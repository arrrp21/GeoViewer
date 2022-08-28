#pragma once
#include "ImageTransformer.hpp"
#include <QString>

#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl2.hpp>
#pragma GCC diagnostic pop

class QImageWrapper;

namespace image_transforming
{

class GpuImageTransformer : public ImageTransformer
{
public:
    GpuImageTransformer(QImageWrapper& imageWrapper);
    ~GpuImageTransformer() override;

    void rotate90() override;
    void gain(int from, int to, double gainLower, double gainUpper) override;
    void equalizeHistogram(int from, int to) override;
    void applyFilter(const Mask& mask) override;
    void backgroundRemoval() override;
    void trimTop() override;

    void commitChanges(Operation) override;

private:
    QImageWrapper& imageWrapper;

    cl_context context;
    cl_device_id device;
    cl_kernel kernelRotate90{nullptr};
    cl_kernel kernelLinearGain{nullptr};
    cl_kernel kernelApplyFilterInt3x3{nullptr};

    cl_uint maxWorkItemDimensions;
    std::vector<size_t> maxWorkItemSizes;
    cl_uint maxComputeUnits;
    size_t maxWorkGroupSize;
    size_t preferredWorkGroupSizeMultiple;

    cl_command_queue queueGain;

    cl_mem inputGain;
    cl_mem outputGain;
    bool isGainCommited{true};

    struct Kernels
    {
        const QString programName;
        const std::vector<std::pair<std::string, cl_kernel*>> functions;
    };

    const std::vector<Kernels> kernels = {
        {
            "GpuImageTransformer.cl",
            {{"rotate90",   &kernelRotate90  },
             {"linearGain", &kernelLinearGain},
             {"applyFilterInt3x3", &kernelApplyFilterInt3x3}}
        }};

    void setupKernels();
    void setupQueues();
    cl_program buildProgram(cl_context context, const QString& fileName);
    cl_kernel createKernel(cl_program program, const char* functionName);

    const QString kernelsLocation = "../GeoViewer/kernels/";

    bool checkError(cl_int error, std::optional<QString> functionName = std::nullopt);
    size_t upToMultipleOf(int multiplier, size_t value);

    void applyFilter(const details::Mask<int>& mask);
    void applyFilter(const details::Mask<double>& mask);

    void releaseDevice();
    void releaseContext();
    void releaseKernels();
    void releaseQueues();
};
} // namespace image_transforming
