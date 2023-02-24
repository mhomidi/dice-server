
#include <iostream>
#include <string>
#include <unistd.h>

#include "execution.hpp"
#include "kernel_handler.hpp"
#include "argument_handler.hpp"

std::atomic<size_t> kernelQueueSize = {0};

#define OPENCL_CHECK_ERROR(e)                                     \
    {                                                             \
        if (e != CL_SUCCESS)                                      \
            std::cout << "OpenCL Error, code=" << e << std::endl; \
    }

#define OPENCL_CALL(func)      \
    {                          \
        cl_int e = (func);     \
        OPENCL_CHECK_ERROR(e); \
    }

static cl_int error = 0;

ExecutionHanlder *ExecutionHanlder::instance = 0;

ExecutionHanlder *ExecutionHanlder::getInstance()
{
    if (ExecutionHanlder::instance == nullptr)
        return instance = new ExecutionHanlder();

    return instance;
}

ExecutionHanlder::ExecutionHanlder()
{
    OPENCL_CALL(clGetPlatformIDs(1, &(this->platform_id), NULL));
    OPENCL_CALL(clGetDeviceIDs(this->platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &(this->device_id), NULL));
    this->context = clCreateContext(NULL, 1, &(this->device_id), NULL, NULL, &error);
    this->commandQueue = clCreateCommandQueue(this->context, this->device_id, 0, &error);
}

void ExecutionHanlder::installKernel()
{
    this->kernelKey = KernelHandler::getInstance()->getNextKernelForRun();
    std::string kernelSource = KernelHandler::getInstance()->getKernelSource(this->kernelKey);
    std::string kernelName = KernelHandler::getInstance()->getKernelName(this->kernelKey);

    std::cout << kernelName << std::endl;

    const char *kernelStr = kernelSource.c_str();
    size_t length = kernelSource.size() + 1;

    this->program = clCreateProgramWithSource(this->context, 1, &kernelStr, &length, &error);
    if (this->program == NULL)
    {
        std::cout << "BAD source ..." << std::endl;
        throw 1;
    }

    error = clBuildProgram(this->program, 1, &(this->device_id), nullptr, nullptr, nullptr);
    if (error != CL_SUCCESS)
    {
        std::cout << "BAD Program build ..." << std::endl;
        throw 1;
    }

    this->kernel = clCreateKernel(this->program, kernelName.c_str(), &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "BAD Kernel ..." << std::endl;
        throw 1;
    }
    std::cout << "Install Kernel Done ..." << std::endl;
}

void ExecutionHanlder::prepareArguments()
{
    std::vector<std::tuple<std::string, size_t>> args = KernelHandler::getInstance()->getKernelArgument(this->kernelKey);
    for (size_t i = 0; i < args.size(); i++)
    {
        size_t argSize = ArgumentHandler::getInstance()->getBufferSize(args[i]);
        float *argData;
        argData = ArgumentHandler::getInstance()->getBufferData(args[i]);

        cl_mem memObject = clCreateBuffer(this->context, CL_MEM_READ_WRITE, argSize, NULL, &error);
        OPENCL_CALL(clSetKernelArg(this->kernel, i, sizeof(cl_mem), (void *)&memObject));
        ArgumentHandler::getInstance()->setBufferMemObject(args[i], memObject);

        OPENCL_CALL(clEnqueueWriteBuffer(this->commandQueue, memObject, CL_TRUE, 0, argSize, argData, 0, NULL, NULL));
    }
    std::cout << "Prepare Args Done ..." << std::endl;
}

void ExecutionHanlder::runKernel()
{
    size_t *globalWorkSize = KernelHandler::getInstance()->getGlobalWorkSize(this->kernelKey);
    size_t workDim = KernelHandler::getInstance()->getWorkDim(this->kernelKey);
    OPENCL_CALL(clEnqueueNDRangeKernel(this->commandQueue, this->kernel, workDim, nullptr, globalWorkSize,
                                       globalWorkSize + 3, 0, nullptr, nullptr));
    OPENCL_CALL(clFinish(this->commandQueue));
    std::cout << "Run kernel Done ... " << std::endl;
}

void ExecutionHanlder::downloadData()
{
    std::vector<std::tuple<std::string, size_t>> args = KernelHandler::getInstance()->getKernelArgument(this->kernelKey);
    for (size_t i = 0; i < args.size(); i++)
    {
        size_t argSize = ArgumentHandler::getInstance()->getBufferSize(args[i]);
        float *argData;
        argData = ArgumentHandler::getInstance()->getBufferData(args[i]);
        cl_mem obj = ArgumentHandler::getInstance()->getBufferMemObject(args[i]);

        OPENCL_CALL(clEnqueueReadBuffer(this->commandQueue, obj, CL_TRUE, 0, argSize, argData, 0, NULL, NULL));
        ArgumentHandler::getInstance()->updateData(args[i], argData, argSize);
        // std::cout << "Update Arg " << i << std::endl;
    }
    std::cout << "Update all argument ... " << std::endl;

    KernelHandler::getInstance()->clearKernel(this->kernelKey);
}

void ExecutionHanlder::execute()
{
    this->installKernel();
    this->prepareArguments();
    this->runKernel();
    this->downloadData();
}

void ExecutionHanlder::schedule()
{
    KernelHandler *kernelHandler = KernelHandler::getInstance();
    while (true)
    {
        size_t size = kernelQueueSize.load(std::memory_order_relaxed);
        if (size > 0)
        {
            this->execute();
            kernelQueueSize.fetch_sub(1, std::memory_order::memory_order_seq_cst);
        }
        else
        {
            usleep(1000 * SLEEP_SCHED_MS);
        }
    }
}