#include "kernel_data_model.hpp"
#include "argument_data_model.hpp"
#include <iostream>
#include <string.h>

KernelDataModel *KernelDataModel::instance = 0;

KernelDataModel *KernelDataModel::GetInstance()
{
    if (instance == nullptr)
        return instance = new KernelDataModel();

    return instance;
}

KernelDataModel::KernelDataModel() {}

void KernelDataModel::AddSource(std::tuple<std::string, std::string> key, std::string source)
{
    if (this->kernel_src.find(key) == this->kernel_src.end())
        this->kernel_src[key] = source;
}

void KernelDataModel::EnqueueKernel(std::tuple<std::string, std::string> key)
{
    this->ready_kernel_queue.push(key);
}

std::tuple<std::string, std::string> KernelDataModel::GetNextKernelForRun()
{
    std::tuple<std::string, std::string> key = this->ready_kernel_queue.front();
    this->ready_kernel_queue.pop();
    return key;
}

std::string KernelDataModel::GetKernelSource(std::tuple<std::string, std::string> key)
{
    return this->kernel_src[key];
}

std::string KernelDataModel::GetKernelName(std::tuple<std::string, std::string> key)
{
    return std::get<1>(key);
}

void KernelDataModel::setKernelArgument(std::tuple<std::string, std::string> key, std::tuple<std::string, size_t> arg_key)
{
    this->kernel_arguments[key].push_back(arg_key);
}

std::vector<std::tuple<std::string, size_t>> KernelDataModel::GetKernelArgument(std::tuple<std::string, std::string> key)
{
    return this->kernel_arguments[key];
}

void KernelDataModel::SetWorkDim(std::tuple<std::string, std::string> key, size_t dim)
{
    this->kernel_work_dim[key] = dim;
}

void KernelDataModel::SetGlobalWorkSize(std::tuple<std::string, std::string> key, std::vector<size_t> &global_work_size)
{
    this->kernel_global_work_size[key] = global_work_size;
}

size_t KernelDataModel::GetWorkDim(std::tuple<std::string, std::string> key)
{
    return this->kernel_work_dim[key];
}

size_t *KernelDataModel::GetGlobalWorkSize(std::tuple<std::string, std::string> key)
{
    return this->kernel_global_work_size[key].data();
}

void KernelDataModel::ClearKernel(std::tuple<std::string, std::string> key)
{
    if (this->kernel_src.find(key) == this->kernel_src.end())
        return;

    this->kernel_src.erase(key);
    this->kernel_arguments.erase(key);
}

void KernelDataModel::SetKernelStatus(std::tuple<std::string, std::string> key, kernel_status_t status)
{
    this->kernel_status[key] = status;
}

kernel_status_t KernelDataModel::GetKernelStatus(std::tuple<std::string, std::string> key)
{
    if (this->kernel_status.find(key) == this->kernel_status.end())
        return KERNEL_STATUS_NOT_EXIST;
    return this->kernel_status[key];
}
