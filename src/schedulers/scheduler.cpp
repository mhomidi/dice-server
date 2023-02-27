
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
    std::map<std::string, std::tuple<size_t, size_t>>::iterator iter;
    for (iter = this->clientBurstTime.begin(); iter != this->clientBurstTime.end(); iter++)
    {
        size_t startBurst = std::get<0>(iter->second);
        size_t totalBurst = std::get<1>(iter->second);
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
    std::string kernelName = this->clientQueue[clientId].front();
    this->clientQueue[clientId].pop();
    return std::tuple<std::string, std::string>(clientId, kernelName);
}

void MMFScheduler::enqueueKernel(std::string clientId, std::string kernelName, size_t size)
{
    if (this->queueSizes.find(clientId) == this->queueSizes.end())
    {
        this->queueSizes[clientId].store(0, std::memory_order_seq_cst);
        this->clientBurstTime[clientId] = std::tuple<size_t, size_t>(0, 0);
    }
    this->clientQueue[clientId].push(kernelName);
    size_t startBurst = std::get<0>(clientBurstTime[clientId]);
    size_t totalBurst = std::get<1>(clientBurstTime[clientId]) + size;
    this->clientBurstTime[clientId] = std::tuple<size_t, size_t>(startBurst, totalBurst);
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