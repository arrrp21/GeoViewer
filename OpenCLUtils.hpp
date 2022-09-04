#pragma once

#include "OpenCLErrors.hpp"
#include <type_traits>
#include <typeinfo>
#include <QDebug>
#include "Log.hpp"

namespace open_cl_utils
{
namespace details
{
constexpr cl_int setKernelArgs(cl_kernel, cl_uint) { return CL_SUCCESS; }

template <typename T, typename ... Args>
cl_int setKernelArgs(cl_kernel kernel, cl_uint arg_index, T arg, Args... args)
{
    cl_int error = clSetKernelArg(kernel, arg_index, sizeof(T), (void*)&arg);
    if (error != CL_SUCCESS)
    {
        LOG_INFO("clSetKernelArg erro at arg_index: {}, {}", arg_index, OpenCLErrors::toString(error));
        return error;
    }
    arg_index++;
    return setKernelArgs(kernel, arg_index, args...);
}
} // namespace details

template <typename ... Args>
cl_int setKernelArgs(cl_kernel kernel, Args... args)
{
    return details::setKernelArgs(kernel, 0, args...);
}

} // namespace open_cl_utils
