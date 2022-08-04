#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <CL/opencl.hpp>
#pragma GCC diagnostic pop

#include "OpenCLErrors.hpp"
#include <type_traits>
#include <typeinfo>
#include <QDebug>

namespace open_cl_utils
{
namespace
{
class Counter
{
public:
    int operator++(int) { return ++count; }
    int operator()()    { return count; }
    void reset()        { count = 0; }
private:
    int count = 0;
};

Counter counter;
} // namespace

cl_int setKernelArgs(cl_kernel)
{
    counter.reset();
    return CL_SUCCESS;
}

template <typename T, typename ... Args>
cl_int setKernelArgs(cl_kernel kernel, T arg, Args... args)
{
    cl_int error = clSetKernelArg(kernel, counter(), sizeof(T), (void*)&arg);
    counter++;
    if (error != CL_SUCCESS)
    {
        qDebug() << "clSetKernelArg index: " << counter();
        qDebug() << "error: " << QString::fromStdString(OpenCLErrors::toString(error));
        counter.reset();
        return error;
    }
    return setKernelArgs(kernel, args...);
}

} // namespace open_cl_utils
