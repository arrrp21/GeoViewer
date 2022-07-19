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

#include <map>

namespace OpenCLErrors
{

static const std::map<cl_int, std::string> openCLErrors {
    {CL_SUCCESS,                                    "No error"},
    {CL_DEVICE_NOT_FOUND,                           "Returned by clGetDeviceIDs and clCreateContextFromType if no OpenCL devices that match the specified devices were found."},
    {CL_DEVICE_NOT_AVAILABLE,                       "Returned by clCreateContext and clCreateContextFromType if the specified device is not currently available."},
    {CL_COMPILER_NOT_AVAILABLE,                     "Returned by clBuildProgram if the parameter program is created with clCreateProgramWithSource and a compiler is not available. For example CL_DEVICE_COMPILER_AVAILABLE is set to CL_FALSE."},
    {CL_MEM_OBJECT_ALLOCATION_FAILURE,              "Returned by the functions listed below if there is a failure to allocate memory for data store associated with image or buffer objects specified as arguments to kernel. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clCreateBuffer, clEnqueueReadBuffer, clEnqueueWriteBuffer, clEnqueueCopyBuffer, clCreateImage2D, clCreateImage3D, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueNDRangeKernel, clEnqueueTask, and clEnqueueNativeKernel"},
    {CL_OUT_OF_RESOURCES,                           "Returned by clEnqueueNDRangeKernel, clEnqueueTask, and clEnqueueNativeKernel in the event of a failure to queue the execution instance of kernel on the command-queue because of insufficient resources needed to execute the kernel."},
    {CL_OUT_OF_HOST_MEMORY,                         "Returned by the functions listed below in the event of a failure to allocate resources required by the OpenCL implementation on the host. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clCreateContext, clCreateContextFromType, clCreateCommandQueue, clCreateBuffer, clEnqueueCopyBuffer, clCreateImage2D, clCreateImage3D, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clCreateSampler, clCreateProgramWithSource, clCreateProgramWithBinary, clBuildProgram, clCreateKernel, clCreateKernelsInProgram, clEnqueueNDRangeKernel, clEnqueueTask, clEnqueueNativeKernel, clEnqueueMarker, clEnqueueWaitForEvents, clEnqueueBarrier, clFlush, clFinish, clCreateFromGLBuffer, clCreateFromGLTexture2D, clCreateFromGLTexture3D, clCreateFromGLRenderbuffer, clEnqueueAcquireGLObjects, and clEnqueueReleaseGLObjects"},
    {CL_PROFILING_INFO_NOT_AVAILABLE,               "Returned by clGetEventProfilingInfo if the CL_QUEUE_PROFILING_ENABLE flag is not set for the command-queue and the profiling information is currently not available (because the command identified by event has not completed)."},
    {CL_MEM_COPY_OVERLAP,                           "Returned by clEnqueueCopyBuffer and clEnqueueCopyImage if the source and destination images are the same image (or the source and destination buffers are the same buffer), and the source and destination regions overlap."},
    {CL_IMAGE_FORMAT_MISMATCH,                      "Returned by clEnqueueCopyImage if the specified source and destination images are not valid image objects."},
    {CL_IMAGE_FORMAT_NOT_SUPPORTED,                 "Returned by clCreateImage2D and clCreateImage3D if the specified image format is not supported."},
    {CL_BUILD_PROGRAM_FAILURE,                      "Returned by clBuildProgram if there is a failure to build the program executable. This error will be returned if clBuildProgram does not return until the build has completed."},
    {CL_MAP_FAILURE,                                "Returned by clEnqueueMapBuffer and clEnqueueMapImage if there is a failure to map the requested region into the host address space. This error cannot occur for buffer objects created with CL_MEM_USE_HOST_PTR or CL_MEM_ALLOC_HOST_PTR."},
#ifdef CL_VERSION_1_1
    {CL_MISALIGNED_SUB_BUFFER_OFFSET,               "Returned by clCreateSubBuffer if there are no devices in context associated with buffer for which the origin value is aligned to the CL_DEVICE_MEM_BASE_ADDR_ALIGN value."},
    {CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST,  "Returned by clWaitForEvents if the execution status of any of the events in event_list is a negative integer value."},
#endif
#ifdef CL_VERSION_1_2
    {CL_COMPILE_PROGRAM_FAILURE,                    "Returned by clCompileProgram if there is a failure to compile the program source. This error will be returned if clCompileProgram does not return until the compile has completed."},
    {CL_LINKER_NOT_AVAILABLE,                       "Returned by clLinkProgram if a linker is not available i.e. CL_DEVICE_LINKER_AVAILABLE specified in the table of allowed values for param_name for clGetDeviceInfo is set to CL_FALSE."},
    {CL_LINK_PROGRAM_FAILURE,                       "Returned by clLinkProgram if there is a failure to link the compiled binaries and/or libraries."},
    {CL_DEVICE_PARTITION_FAILED,                    "Returned by clCreateSubDevices if the partition name is supported by the implementation but in_device could not be further partitioned."},
    {CL_KERNEL_ARG_INFO_NOT_AVAILABLE,              "Returned by clGetKernelArgInfo if the argument information is not available for kernel."},
#endif
    {CL_INVALID_VALUE,                              "Returned by the functions listed below if a parameter is not an expected value. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clGetPlatformIDs, clGetPlatformInfo, clGetDeviceIDs, clGetDeviceInfo, clCreateContext, clCreateContextFromType, clGetContextInfo, clCreateCommandQueue, clGetCommandQueueInfo, clSetCommandQueueProperty, clCreateBuffer, clEnqueueReadBuffer, clEnqueueWriteBuffer, clEnqueueCopyBuffer, clCreateImage2D, clCreateImage3D, clGetSupportedImageFormats, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clGetMemObjectInfo, clGetImageInfo, clCreateSampler, clGetSamplerInfo, clCreateProgramWithSource, clCreateProgramWithBinary, clBuildProgram, clGetProgramInfo, clGetProgramBuildInfo, clCreateKernel, clCreateKernelsInProgram, clGetKernelInfo, clGetKernelWorkGroupInfo, clEnqueueNativeKernel, clWaitForEvents, clGetEventInfo, clEnqueueMarker, clEnqueueWaitForEvents, clGetEventProfilingInfo, clCreateFromGLBuffer, clCreateFromGLTexture2D, clCreateFromGLTexture3D, clCreateFromGLRenderbuffer, clGetGLTextureInfo, and clEnqueueAcquireGLObjects"},
    {CL_INVALID_DEVICE_TYPE,                        "Returned by clGetDeviceIDs and clCreateContextFromType if device type specified is not valid."},
    {CL_INVALID_PLATFORM,                           "Returned by clGetPlatformInfo and clGetDeviceIDs if the specified platform is not a valid platform.\nReturned by clCreateContext and clCreateContextFromType if properties is NULL and no platform could be selected, or if platform value specified in properties is not a valid platform."},
    {CL_INVALID_DEVICE,                             "Returned by the functions listed below if the device or devices specified are not valid. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clGetDeviceIDs, clGetDeviceInfo, clCreateContext, clCreateContextFromType, clCreateCommandQueue, clCreateProgramWithBinary, clBuildProgram, clGetProgramBuildInfo, and clGetKernelWorkGroupInfo"},
    {CL_INVALID_CONTEXT,                            "Returned by the functions listed below if the specified context is not a valid OpenCL context, or the context associated with certain parameters are not the same. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clRetainContext, clReleaseContext, clGetContextInfo, clCreateCommandQueue, clCreateBuffer, clEnqueueReadBuffer, clEnqueueWriteBuffer, clEnqueueCopyBuffer, clCreateImage2D, clCreateImage3D, clGetSupportedImageFormats, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clCreateSampler, clCreateProgramWithSource, clCreateProgramWithBinary, clEnqueueNDRangeKernel, clEnqueueTask, clEnqueueNativeKernel, clWaitForEvents, clEnqueueWaitForEvents, clCreateFromGLBuffer, clCreateFromGLTexture2D, clCreateFromGLTexture3D, clCreateFromGLRenderbuffer, clEnqueueAcquireGLObjects, and clEnqueueReleaseGLObjects"},
    {CL_INVALID_QUEUE_PROPERTIES,                   "Returned by clCreateCommandQueue and clSetCommandQueueProperty if specified properties are valid but are not supported by the device."},
    {CL_INVALID_COMMAND_QUEUE,                      "Returned by the functions listed below if the specified command-queue is not a valid command-queue. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clRetainCommandQueue, clReleaseCommandQueue, clGetCommandQueueInfo, clSetCommandQueueProperty, clEnqueueReadBuffer, clEnqueueWriteBuffer, clEnqueueCopyBuffer, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clEnqueueNDRangeKernel, clEnqueueTask, clEnqueueNativeKernel, clEnqueueMarker, clEnqueueWaitForEvents, clEnqueueBarrier, clFlush, clFinish, clEnqueueAcquireGLObjects, and clEnqueueReleaseGLObjects"},
    {CL_INVALID_HOST_PTR,                           "Returned by the functions listed below if host_ptr is NULL and CL_MEM_USE_HOST_PTR or CL_MEM_COPY_HOST_PTR are set in flags or if host_ptr is not NULL but CL_MEM_COPY_HOST_PTR or CL_MEM_USE_HOST_PTR are not set in flags. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the functions clCreateBuffer, clCreateImage2D, and clCreateImage3D."},
    {CL_INVALID_MEM_OBJECT,                         "Returned by the functions listed below if a parameter is not a valid memory, image, or buffer object. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clEnqueueReadBuffer, clEnqueueWriteBuffer, clEnqueueCopyBuffer, clRetainMemObject, clReleaseMemObject, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clGetMemObjectInfo, clGetImageInfo, clSetKernelArg, clEnqueueNativeKernel, clGetGLObjectInfo, clGetGLTextureInfo, clEnqueueAcquireGLObjects, and clEnqueueReleaseGLObjects"},
    {CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,            "Returned by clCreateImage2D and clCreateImage3D if the image format specified is not valid or is NULL.\nReturned byclCreateFromGLTexture2D and clCreateFromGLTexture3D\nReturned by clCreateFromGLRenderbuffer if the OpenGL renderbuffer internal format does not map to a supported OpenCL image format."},
    {CL_INVALID_IMAGE_SIZE,                         "Returned by clCreateImage2D if the specified image width or height are 0 or if they exceed values specified in CL_DEVICE_IMAGE2D_MAX_WIDTH or CL_DEVICE_IMAGE2D_MAX_HEIGHT respectively for all devices in context, or if the specified image row pitch does not follow rules described for clCreateImage2D.\nReturned by clCreateImage3D if the specified image width or height are 0 or if the image depth is <= 1, or if they exceed values specified in CL_DEVICE_IMAGE3D_MAX_WIDTH, CL_DEVICE_IMAGE3D_MAX_HEIGHT or CL_DEVICE_IMAGE3D_MAX_DEPTH respectively for all devices in context, or if the image row pitch and image slice pitch do not follow rules described for clCreateImage3D."},
    {CL_INVALID_SAMPLER,                            "Returned by clRetainSampler, clReleaseSampler, and clGetSamplerInfo if the specified sampler is not a valid sampler object.\nReturned by clSetKernelArg for an argument declared to be of type sampler_t when the specified arg_value is not a valid sampler object."},
    {CL_INVALID_BINARY,                             "Returned by clBuildProgram and clCreateProgramWithBinary if the program binary is not a valid binary for the specified device."},
    {CL_INVALID_BUILD_OPTIONS,                      "Returned by clBuildProgram if the specified build options are invalid."},
    {CL_INVALID_PROGRAM,                            "Returned by the functions listed below if the specified program is not a valid program object. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clRetainProgram, clReleaseProgram, clBuildProgram, clGetProgramInfo, clGetProgramBuildInfo, clCreateKernel, and clCreateKernelsInProgram"},
    {CL_INVALID_PROGRAM_EXECUTABLE,                 "Returned by clCreateKernel if there is no successfully built executable for program, and returned by clCreateKernelsInProgram if there is no device in program.\nReturned by clEnqueueNDRangeKernel and clEnqueueTask if there is no successfully built program executable available for device associated with command_queue."},
    {CL_INVALID_KERNEL_NAME,                        "Returned by clCreateKernel if the specified kernel name is not found in program."},
    {CL_INVALID_KERNEL_DEFINITION,                  "Returned by clCreateKernel if the function definition for __kernel function given by kernel_name such as the number of arguments, the argument types are not the same for all devices for which the program executable has been built."},
    {CL_INVALID_KERNEL,                             "Returned by the functions listed below if the specified kernel is not a valid kernel object. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clReleaseKernel, clRetainKernel, clSetKernelArg, clGetKernelInfo, clGetKernelWorkGroupInfo, clEnqueueNDRangeKernel, and clEnqueueTask"},
    {CL_INVALID_ARG_INDEX,                          "Returned by clSetKernelArg if an invalid argument index is specified."},
    {CL_INVALID_ARG_VALUE,                          "Returned by clSetKernelArg if the argument value specified is NULL for an argument that is not declared with the __local qualifier or vice-versa."},
    {CL_INVALID_ARG_SIZE,                           "Returned by clSetKernelArg if argument size specified (arg_size) does not match the size of the data type for an argument that is not a memory object, or if the argument is a memory object and arg_size != sizeof(cl_mem) or if arg_size is zero and the argument is declared with the __local qualifier or if the argument is a sampler and arg_size != sizeof(cl_sampler)."},
    {CL_INVALID_KERNEL_ARGS,                        "Returned by clEnqueueNDRangeKernel and clEnqueueTask if the kernel argument values have not been specified."},
    {CL_INVALID_WORK_DIMENSION,                     "Returned by clEnqueueNDRangeKernel if work_dim is not a valid value."},
    {CL_INVALID_WORK_GROUP_SIZE,                    "Returned by clEnqueueNDRangeKernel and clEnqueueTask if local_work_size is specified and number of workitems specified by global_work_size is not evenly divisible by size of work-group given by local_work_size or does not match the work-group size specified for kernel using the __attribute__((reqd_work_group_size(X, Y, Z))) qualifier in program source."},
    {CL_INVALID_WORK_ITEM_SIZE,                     "Returned by clEnqueueNDRangeKernel if the number of work-items specified in any of local_work_size... [0]... local_work_size[work_dim - 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0],... CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim -1]."},
    {CL_INVALID_GLOBAL_OFFSET,                      "Returned by clEnqueueNDRangeKernel if global_work_offset is not NULL."},
    {CL_INVALID_EVENT_WAIT_LIST,                    "Returned by the functions listed below if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list_list is not NULL and num_events_in_wait_list is 0, or specified event objects are not valid events. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clEnqueueReadBuffer, clEnqueueWriteBuffer, clEnqueueCopyBuffer, clEnqueueReadImage, clEnqueueWriteImage, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clEnqueueNDRangeKernel, clEnqueueTask, clEnqueueCopyImage, clEnqueueCopyImageToBuffer, clEnqueueCopyBufferToImage, clEnqueueMapBuffer, clEnqueueMapImage, clEnqueueUnmapMemObject, clEnqueueNDRangeKernel, clEnqueueAcquireGLObjects, and clEnqueueReleaseGLObjects"},
    {CL_INVALID_EVENT,                              "Returned by the functions listed below if the event objects specified are not valid. The exact condition that generates this error depends on the calling function. Refer to the function for more information.\nReturned by the following functions: clWaitForEvents, clGetEventInfo, clRetainEvent, clReleaseEvent, clEnqueueWaitForEvents, and clGetEventProfilingInfo"},
    {CL_INVALID_OPERATION,                          "Returned by clCreateImage2D, clCreateImage3D, and clCreateSampler if there are no devices in context that support images.\nReturned by clBuildProgram if the build of a program executable for any of the devices specified by a previous call to clBuildProgram for program has not completed, or if there are kernel objects attached to program.\nReturned by clEnqueueNativeKernel if the specified device cannot execute the native kernel.\nReturned by clCreateFromGLTexture2D if the miplevel is less than 0.\nReturned by clCreateFromGLTexture3D if 3D images are not supported by the OpenCL embedded profile."},
    {CL_INVALID_GL_OBJECT,                          "Returned by clCreateFromGLBuffer if bufobj is not a GL buffer object or is a GL buffer object but does not have an existing data store.\nReturned by clCreateFromGLRenderbuffer if renderbuffer is not a GL renderbuffer object or if the width or height of renderbuffer is zero.\nReturned by clCreateFromGLTexture2D and clCreateFromGLTexture3D if texture is not a GL texture object whose type matches texture_target, if the specified miplevel of texture is not defined, or if the width, height or depth of the specified miplevel is zero.\nReturned by clGetGLObjectInfo and clGetGLTextureInfo if there is no GL object or texture associated with memobj.\nReturned by clEnqueueAcquireGLObjects and clEnqueueReleaseGLObjects if memory objects in mem_objects have not been created from a GL object(s). The exact condition that generates this error depends on the calling function."},
    {CL_INVALID_BUFFER_SIZE,                        "Returned by clCreateBuffer if the value of the parameter size is 0 or is greater than CL_DEVICE_MAX_MEM_ALLOC_SIZE for all devices specified in the parameter context."},
    {CL_INVALID_MIP_LEVEL,                          "if miplevel is greater than zero and the OpenGL implementation does not support creating from non-zero mipmap levels"},
    {CL_INVALID_GLOBAL_WORK_SIZE,                   "Returned by clEnqueueNDRangeKernel if global_work_size is NULL, or if any of the values specified in global_work_size[0], ...global_work_size [work_dim - 1] are 0 or exceed the range given by the sizeof(size_t) for the device on which the kernel execution will be enqueued."},
#ifdef CL_VERSION_1_1
    {CL_INVALID_PROPERTY,                           "Vague error, depends on the function"},
#endif
#ifdef CL_VERSION_1_2
    {CL_INVALID_IMAGE_DESCRIPTOR,                   "Returned by clCreateImage if values specified in image_desc are not valid or if image_desc is NULL."},
    {CL_INVALID_COMPILER_OPTIONS,                   "Returned by clCompileProgram if the compiler options specified by options are invalid."},
    {CL_INVALID_LINKER_OPTIONS,                     "Returned by clLinkProgram if the linker options specified by options are invalid"},
    {CL_INVALID_DEVICE_PARTITION_COUNT,             "Returned by clCreateSubDevices if the partition name specified in properties is CL_DEVICE_PARTITION_BY_COUNTS and the number of sub-devices requested exceeds CL_DEVICE_PARTITION_MAX_SUB_DEVICES or the total number of compute units requested exceeds CL_DEVICE_PARTITION_MAX_COMPUTE_UNITS for in_device, or the number of compute units requested for one or more sub-devices is less than zero or the number of sub-devices requested exceeds CL_DEVICE_PARTITION_MAX_COMPUTE_UNITS for in_device."},
#endif
#ifdef CL_VERSION_2_0
    {CL_INVALID_PIPE_SIZE,                          "Returned by clCreatePipe if pipe_packet_size is 0 or the pipe_packet_size exceeds CL_DEVICE_PIPE_MAX_PACKET_SIZE value specified in table 4.3 (see clGetDeviceInfo) for all devices in context or if pipe_max_packets is 0."},
    {CL_INVALID_DEVICE_QUEUE,                       "Returned by clSetKernelArg for an argument declared to be of type queue_t when the specified arg_value is not a valid device queue object."},
#endif
#ifdef CL_VERSION_2_2
    {CL_INVALID_SPEC_ID,                            "Returned by clSetProgramSpecializationConstant if spec_id is not a valid specialization constant identifier."},
    {CL_MAX_SIZE_RESTRICTION_EXCEEDED,              "Returned by clSetKernelArg if the size in bytes of the memory object (if the argument is a memory object) or arg_size (if the argument is declared with local qualifier) exceeds a language- specified maximum size restriction for this argument, such as the MaxByteOffset SPIR-V decoration. This error code is missing before version 2.2"},
#endif
};

std::string getDescription(cl_int error)
{
    try
    {
        return openCLErrors.at(error);
    }
    catch (std::out_of_range)
    {
        return std::string("No description for error code: ") + std::to_string(error);
    }
}

std::string toString(cl_int error)
{
    switch (error)
    {
    case CL_SUCCESS:                                    return "CL_SUCCESS";
    case CL_DEVICE_NOT_FOUND:                           return "CL_DEVICE_NOT_FOUND";
    case CL_DEVICE_NOT_AVAILABLE:                       return "CL_DEVICE_NOT_AVAILABLE";
    case CL_COMPILER_NOT_AVAILABLE:                     return "CL_COMPILER_NOT_AVAILABLE";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:              return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case CL_OUT_OF_RESOURCES:                           return "CL_OUT_OF_RESOURCES";
    case CL_OUT_OF_HOST_MEMORY:                         return "CL_OUT_OF_HOST_MEMORY";
    case CL_PROFILING_INFO_NOT_AVAILABLE:               return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case CL_MEM_COPY_OVERLAP:                           return "CL_MEM_COPY_OVERLAP";
    case CL_IMAGE_FORMAT_MISMATCH:                      return "CL_IMAGE_FORMAT_MISMATCH";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:                 return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case CL_BUILD_PROGRAM_FAILURE:                      return "CL_BUILD_PROGRAM_FAILURE";
    case CL_MAP_FAILURE:                                return "CL_MAP_FAILURE";
#ifdef CL_VERSION_1_1
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:               return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:  return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
#endif
#ifdef CL_VERSION_1_2
    case CL_COMPILE_PROGRAM_FAILURE:                    return "CL_COMPILE_PROGRAM_FAILURE";
    case CL_LINKER_NOT_AVAILABLE:                       return "CL_LINKER_NOT_AVAILABLE";
    case CL_LINK_PROGRAM_FAILURE:                       return "CL_LINK_PROGRAM_FAILURE";
    case CL_DEVICE_PARTITION_FAILED:                    return "CL_DEVICE_PARTITION_FAILED";
    case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:              return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
#endif
    case CL_INVALID_VALUE:                              return "CL_INVALID_VALUE";
    case CL_INVALID_DEVICE_TYPE:                        return "CL_INVALID_DEVICE_TYPE";
    case CL_INVALID_PLATFORM:                           return "CL_INVALID_PLATFORM";
    case CL_INVALID_DEVICE:                             return "CL_INVALID_DEVICE";
    case CL_INVALID_CONTEXT:                            return "CL_INVALID_CONTEXT";
    case CL_INVALID_QUEUE_PROPERTIES:                   return "CL_INVALID_QUEUE_PROPERTIES";
    case CL_INVALID_COMMAND_QUEUE:                      return "CL_INVALID_COMMAND_QUEUE";
    case CL_INVALID_HOST_PTR:                           return "CL_INVALID_HOST_PTR";
    case CL_INVALID_MEM_OBJECT:                         return "CL_INVALID_MEM_OBJECT";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:            return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case CL_INVALID_IMAGE_SIZE:                         return "CL_INVALID_IMAGE_SIZE";
    case CL_INVALID_SAMPLER:                            return "CL_INVALID_SAMPLER";
    case CL_INVALID_BINARY:                             return "CL_INVALID_BINARY";
    case CL_INVALID_BUILD_OPTIONS:                      return "CL_INVALID_BUILD_OPTIONS";
    case CL_INVALID_PROGRAM:                            return "CL_INVALID_PROGRAM";
    case CL_INVALID_PROGRAM_EXECUTABLE:                 return "CL_INVALID_PROGRAM_EXECUTABLE";
    case CL_INVALID_KERNEL_NAME:                        return "CL_INVALID_KERNEL_NAME";
    case CL_INVALID_KERNEL_DEFINITION:                  return "CL_INVALID_KERNEL_DEFINITION";
    case CL_INVALID_KERNEL:                             return "CL_INVALID_KERNEL";
    case CL_INVALID_ARG_INDEX:                          return "CL_INVALID_ARG_INDEX";
    case CL_INVALID_ARG_VALUE:                          return "CL_INVALID_ARG_VALUE";
    case CL_INVALID_ARG_SIZE:                           return "CL_INVALID_ARG_SIZE";
    case CL_INVALID_KERNEL_ARGS:                        return "CL_INVALID_KERNEL_ARGS";
    case CL_INVALID_WORK_DIMENSION:                     return "CL_INVALID_WORK_DIMENSION";
    case CL_INVALID_WORK_GROUP_SIZE:                    return "CL_INVALID_WORK_GROUP_SIZE";
    case CL_INVALID_WORK_ITEM_SIZE:                     return "CL_INVALID_WORK_ITEM_SIZE";
    case CL_INVALID_GLOBAL_OFFSET:                      return "CL_INVALID_GLOBAL_OFFSET";
    case CL_INVALID_EVENT_WAIT_LIST:                    return "CL_INVALID_EVENT_WAIT_LIST";
    case CL_INVALID_EVENT:                              return "CL_INVALID_EVENT";
    case CL_INVALID_OPERATION:                          return "CL_INVALID_OPERATION";
    case CL_INVALID_GL_OBJECT:                          return "CL_INVALID_GL_OBJECT";
    case CL_INVALID_BUFFER_SIZE:                        return "CL_INVALID_BUFFER_SIZE";
    case CL_INVALID_MIP_LEVEL:                          return "CL_INVALID_MIP_LEVEL";
    case CL_INVALID_GLOBAL_WORK_SIZE:                   return "CL_INVALID_GLOBAL_WORK_SIZE";
#ifdef CL_VERSION_1_1
    case CL_INVALID_PROPERTY:                           return "CL_INVALID_PROPERTY";
#endif
#ifdef CL_VERSION_1_2
    case CL_INVALID_IMAGE_DESCRIPTOR:                   return "CL_INVALID_IMAGE_DESCRIPTOR";
    case CL_INVALID_COMPILER_OPTIONS:                   return "CL_INVALID_COMPILER_OPTIONS";
    case CL_INVALID_LINKER_OPTIONS:                     return "CL_INVALID_LINKER_OPTIONS";
    case CL_INVALID_DEVICE_PARTITION_COUNT:             return "CL_INVALID_DEVICE_PARTITION_COUNT";
#endif
#ifdef CL_VERSION_2_0
    case CL_INVALID_PIPE_SIZE:                          return "CL_INVALID_PIPE_SIZE";
    case CL_INVALID_DEVICE_QUEUE:                       return "CL_INVALID_DEVICE_QUEUE";
#endif
#ifdef CL_VERSION_2_2
    case CL_INVALID_SPEC_ID:                            return "CL_INVALID_SPEC_ID";
    case CL_MAX_SIZE_RESTRICTION_EXCEEDED:              return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
#endif
    default: return std::string("Unknown error code: ") + std::to_string(error);
    }
}
} // namespace OpenCLErrors
