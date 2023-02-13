#include "kernel_handler.hpp"
#include "argument_handler.hpp"
#include <iostream>
#include <string.h>

KernelHandler *KernelHandler::instance = 0;

KernelHandler *KernelHandler::getInstance()
{
    if (instance == nullptr)
        return instance = new KernelHandler();

    return instance;
}

KernelHandler::KernelHandler() {}

void KernelHandler::addSource(std::tuple<std::string, std::string> key, std::string source)
{
    if (this->kernelSources.find(key) == this->kernelSources.end())
        this->kernelSources[key] = source;
}

void KernelHandler::enqueueKernel(std::tuple<std::string, std::string> key)
{
    this->readyKernelQueue.push(key);
}

std::tuple<std::string, std::string> KernelHandler::getNextKernelForRun()
{
    std::tuple<std::string, std::string> kernelKey = this->readyKernelQueue.front();
    this->readyKernelQueue.pop();
    return kernelKey;
}

std::string KernelHandler::getKernelSource(std::tuple<std::string, std::string> key)
{
    return this->kernelSources[key];
}

std::string KernelHandler::getKernelName(std::tuple<std::string, std::string> key)
{
    return std::get<1>(key);
}

void KernelHandler::setKernelArgument(std::tuple<std::string, std::string> kernelKey, std::tuple<std::string, size_t> argKey)
{
    this->kernelsArguments[kernelKey].push_back(argKey);
}

std::vector<std::tuple<std::string, size_t>> KernelHandler::getKernelArgument(std::tuple<std::string, std::string> kernelKey)
{
    return this->kernelsArguments[kernelKey];
}

void KernelHandler::setWorkDim(std::tuple<std::string, std::string> kernelKey, size_t dim)
{
    this->kernelWorkDim[kernelKey] = dim;
}

void KernelHandler::setGlobalWorkSize(std::tuple<std::string, std::string> kernelKey, std::vector<size_t> &globalWorkSize)
{
    this->kernelGlobalWorkSize[kernelKey] = globalWorkSize;
}

size_t KernelHandler::getWorkDim(std::tuple<std::string, std::string> kernelKey)
{
    return this->kernelWorkDim[kernelKey];
}

size_t *KernelHandler::getGlobalWorkSize(std::tuple<std::string, std::string> kernelKey)
{
    return this->kernelGlobalWorkSize[kernelKey].data();
}

void KernelHandler::clearKernel(std::tuple<std::string, std::string> kernelKey)
{
    if (this->kernelSources.find(kernelKey) == this->kernelSources.end())
        return;

    this->kernelSources.erase(kernelKey);
    this->kernelsArguments.erase(kernelKey);
}

size_t KernelHandler::getReadyQueueSize()
{
    return this->readyKernelQueue.size();
}