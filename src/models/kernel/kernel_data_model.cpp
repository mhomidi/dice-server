#include "kernel_data_model.hpp"
#include "argument_data_model.hpp"
#include <iostream>
#include <string.h>

KernelDataModel *KernelDataModel::instance = 0;

KernelDataModel *KernelDataModel::getInstance()
{
    if (instance == nullptr)
        return instance = new KernelDataModel();

    return instance;
}

KernelDataModel::KernelDataModel() {}

void KernelDataModel::addSource(std::tuple<std::string, std::string> key, std::string source)
{
    if (this->kernelSources.find(key) == this->kernelSources.end())
        this->kernelSources[key] = source;
}

void KernelDataModel::enqueueKernel(std::tuple<std::string, std::string> key)
{
    this->readyKernelQueue.push(key);
}

std::tuple<std::string, std::string> KernelDataModel::getNextKernelForRun()
{
    std::tuple<std::string, std::string> kernelKey = this->readyKernelQueue.front();
    this->readyKernelQueue.pop();
    return kernelKey;
}

std::string KernelDataModel::getKernelSource(std::tuple<std::string, std::string> key)
{
    return this->kernelSources[key];
}

std::string KernelDataModel::getKernelName(std::tuple<std::string, std::string> key)
{
    return std::get<1>(key);
}

void KernelDataModel::setKernelArgument(std::tuple<std::string, std::string> kernelKey, std::tuple<std::string, size_t> argKey)
{
    this->kernelsArguments[kernelKey].push_back(argKey);
}

std::vector<std::tuple<std::string, size_t>> KernelDataModel::getKernelArgument(std::tuple<std::string, std::string> kernelKey)
{
    return this->kernelsArguments[kernelKey];
}

void KernelDataModel::setWorkDim(std::tuple<std::string, std::string> kernelKey, size_t dim)
{
    this->kernelWorkDim[kernelKey] = dim;
}

void KernelDataModel::setGlobalWorkSize(std::tuple<std::string, std::string> kernelKey, std::vector<size_t> &globalWorkSize)
{
    this->kernelGlobalWorkSize[kernelKey] = globalWorkSize;
}

size_t KernelDataModel::getWorkDim(std::tuple<std::string, std::string> kernelKey)
{
    return this->kernelWorkDim[kernelKey];
}

size_t *KernelDataModel::getGlobalWorkSize(std::tuple<std::string, std::string> kernelKey)
{
    return this->kernelGlobalWorkSize[kernelKey].data();
}

void KernelDataModel::clearKernel(std::tuple<std::string, std::string> kernelKey)
{
    if (this->kernelSources.find(kernelKey) == this->kernelSources.end())
        return;

    this->kernelSources.erase(kernelKey);
    this->kernelsArguments.erase(kernelKey);
}

size_t KernelDataModel::getReadyQueueSize()
{
    return this->readyKernelQueue.size();
}

void KernelDataModel::SetKernelStatus(std::tuple<std::string, std::string> key, kernel_status_t status)
{
    this->kernelStatus[key] = status;
}

kernel_status_t KernelDataModel::GetKernelStatus(std::tuple<std::string, std::string> key)
{
    if (this->kernelStatus.find(key) == this->kernelStatus.end())
        return KERNEL_STATUS_NOT_EXIST;
    return this->kernelStatus[key];
}
