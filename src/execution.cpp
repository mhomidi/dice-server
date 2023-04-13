
#include <iostream>
#include <string>
#include <unistd.h>

#include "execution.hpp"
#include "kernel_data_model.hpp"
#include "argument_data_model.hpp"

std::atomic<size_t> kernel_queue_qize = {0};

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

ExecutionHanlder *ExecutionHanlder::GetInstance()
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
    this->command_queue = clCreateCommandQueue(this->context, this->device_id, 0, &error);
}

void ExecutionHanlder::InstallKernel()
{
    this->kernel_key = KernelDataModel::GetInstance()->GetNextKernelForRun();
    std::string kernel_src = KernelDataModel::GetInstance()->GetKernelSource(this->kernel_key);
    std::string kernel_name = KernelDataModel::GetInstance()->GetKernelName(this->kernel_key);

    const char *kernel_str = kernel_src.c_str();
    size_t length = kernel_src.size() + 1;

    this->program = clCreateProgramWithSource(this->context, 1, &kernel_str, &length, &error);
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

    this->kernel = clCreateKernel(this->program, kernel_name.c_str(), &error);
    if (error != CL_SUCCESS)
    {
        std::cout << "BAD Kernel ..." << std::endl;
        throw 1;
    }
    std::cout << "Install Kernel Done ..." << std::endl;
}

void ExecutionHanlder::PrepareArguments()
{
    std::vector<std::tuple<std::string, size_t>> args = KernelDataModel::GetInstance()->GetKernelArgument(this->kernel_key);
    for (size_t i = 0; i < args.size(); i++)
    {
        size_t arg_size = ArgumentDataModel::GetInstance()->GetBufferSize(args[i]);
        float *arg_data;
        arg_data = ArgumentDataModel::GetInstance()->GetBufferData(args[i]);

        cl_mem mem_object = clCreateBuffer(this->context, CL_MEM_READ_WRITE, arg_size, NULL, &error);
        OPENCL_CALL(clSetKernelArg(this->kernel, i, sizeof(cl_mem), (void *)&mem_object));
        ArgumentDataModel::GetInstance()->SetBufferMemObject(args[i], mem_object);

        OPENCL_CALL(clEnqueueWriteBuffer(this->command_queue, mem_object, CL_TRUE, 0, arg_size, arg_data, 0, NULL, NULL));
    }
    std::cout << "Prepare Args Done ..." << std::endl;
}

void ExecutionHanlder::RunKernel()
{
    size_t *global_work_size = KernelDataModel::GetInstance()->GetGlobalWorkSize(this->kernel_key);
    size_t work_dim = KernelDataModel::GetInstance()->GetWorkDim(this->kernel_key);
    OPENCL_CALL(clEnqueueNDRangeKernel(this->command_queue, this->kernel, work_dim, nullptr, global_work_size,
                                       global_work_size + 3, 0, nullptr, nullptr));
    OPENCL_CALL(clFinish(this->command_queue));
    std::cout << "Run kernel Done ... " << std::endl;
}

void ExecutionHanlder::DownloadData()
{
    std::vector<std::tuple<std::string, size_t>> args = KernelDataModel::GetInstance()->GetKernelArgument(this->kernel_key);
    for (size_t i = 0; i < args.size(); i++)
    {
        size_t arg_size = ArgumentDataModel::GetInstance()->GetBufferSize(args[i]);
        float *arg_data;
        arg_data = ArgumentDataModel::GetInstance()->GetBufferData(args[i]);
        cl_mem obj = ArgumentDataModel::GetInstance()->GetBufferMemObject(args[i]);

        OPENCL_CALL(clEnqueueReadBuffer(this->command_queue, obj, CL_TRUE, 0, arg_size, arg_data, 0, NULL, NULL));
        ArgumentDataModel::GetInstance()->UpdateData(args[i], arg_data, arg_size);
        std::cout << "Update Arg " << i << std::endl;
    }
    std::cout << "Update all argument ... " << std::endl;
    KernelDataModel::GetInstance()->SetKernelStatus(this->kernel_key, KERNEL_STATUS_DONE);
}

void ExecutionHanlder::Execute()
{
    this->InstallKernel();
    this->PrepareArguments();
    this->RunKernel();
    this->DownloadData();
}

void ExecutionHanlder::Run()
{
    KernelDataModel *kernel_handler = KernelDataModel::GetInstance();
    while (true)
    {
        size_t size = kernel_queue_qize.load(std::memory_order_seq_cst);
        if (size > 0)
        {
            this->Execute();
            kernel_queue_qize.fetch_sub(1, std::memory_order::memory_order_seq_cst);
        }
    }
}