
#include "scheduler.hpp"
#include "server.h"
#include "config.hpp"

MMFScheduler *MMFScheduler::instance = nullptr;

MMFScheduler *MMFScheduler::getInstance()
{
    if (instance == nullptr)
    {
        return instance = new MMFScheduler();
    }
    return instance;
}

std::tuple<std::string, std::string> MMFScheduler::chooseKernel()
{
    std::string clientId = NULL_STR;
    size_t minStartTime = -1;
    std::map<std::string, BurstTime>::iterator iter;
    for (iter = this->clientBurstTime.begin(); iter != this->clientBurstTime.end(); iter++)
    {
        size_t startBurst = iter->second.start.load(std::memory_order_relaxed);
        size_t totalBurst = iter->second.end.load(std::memory_order_relaxed);
        if (startBurst < minStartTime && startBurst != totalBurst)
        {
            minStartTime = startBurst;
            clientId = iter->first;
        }
    }
    if (clientId.empty())
    {
        return std::make_tuple(NULL_STR, NULL_STR);
    }
    std::tuple<std::string, size_t> kernelData;
    this->clientQueue[clientId].try_dequeue(kernelData);
    std::string kernelName = std::get<0>(kernelData);
    size_t kernelSize = std::get<1>(kernelData);
    this->clientBurstTime[clientId].start.fetch_add(kernelSize, std::memory_order_seq_cst);
    return std::tuple<std::string, std::string>(clientId, kernelName);
}

void MMFScheduler::enqueueKernel(std::string clientId, std::string kernelName, size_t size)
{
    if (this->queueSizes.find(clientId) == this->queueSizes.end())
    {
        this->queueSizes[clientId].store(0, std::memory_order_seq_cst);
    }
    this->clientQueue[clientId].enqueue(std::tuple<std::string, size_t>(kernelName, size));
    this->clientBurstTime[clientId].end.fetch_add(size, std::memory_order_relaxed);
    this->queueSizes[clientId].fetch_add(1, std::memory_order_seq_cst);
}

void MMFScheduler::startSchedule()
{
    while (true)
    {
        std::tuple<std::string, std::string> key = this->chooseKernel();
        std::string clientId = std::get<0>(key);
        std::string kernelName = std::get<1>(key);
        if (clientId.empty())
        {
            continue;
        }
        KernelHandler::getInstance()->enqueueKernel(key);
        kernelQueueSize.fetch_add(1, std::memory_order_seq_cst);
    }
}