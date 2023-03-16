
#include "scheduler.hpp"
#include "server.h"
#include "config.hpp"
#include "kernel_depend_model.hpp"

MMFScheduler *MMFScheduler::instance = nullptr;

MMFScheduler *MMFScheduler::GetInstance()
{
    if (instance == nullptr)
    {
        return instance = new MMFScheduler();
    }
    return instance;
}

std::tuple<std::string, std::string> MMFScheduler::ChooseKernel()
{
    std::string client_id = NULL_STR;
    size_t min_start_time = -1;
    std::map<std::string, BurstTime>::iterator iter;
    std::set<std::string> banned_clients;
    while (true)
    {
        for (iter = this->client_burst_time.begin(); iter != this->client_burst_time.end(); iter++)
        {
            if (banned_clients.find(iter->first) != banned_clients.end())
                continue;
            size_t start_burst = iter->second.start.load(std::memory_order_relaxed);
            size_t total_burst = iter->second.end.load(std::memory_order_relaxed);
            if (start_burst < min_start_time && start_burst != total_burst)
            {
                min_start_time = start_burst;
                client_id = iter->first;
            }
        }
        if (client_id.empty())
            return std::make_tuple(NULL_STR, NULL_STR);
        std::tuple<std::string, size_t> *kernel_ptr = this->client_queue[client_id].peek();
        if (this->IsDependencyPassed(client_id, std::get<0>(*kernel_ptr)))
            break;
        banned_clients.insert(client_id);
    }
    std::tuple<std::string, size_t> kernel_data;
    this->client_queue[client_id].try_dequeue(kernel_data);
    std::string kernel_name = std::get<0>(kernel_data);
    size_t kernel_size = std::get<1>(kernel_data);
    this->client_burst_time[client_id].start.fetch_add(kernel_size, std::memory_order_seq_cst);
    return std::tuple<std::string, std::string>(client_id, kernel_name);
}

void MMFScheduler::EnqueueKernel(std::string client_id, std::string kernel_name, size_t size)
{
    if (this->queue_sizes.find(client_id) == this->queue_sizes.end())
    {
        this->queue_sizes[client_id].store(0, std::memory_order_seq_cst);
    }
    this->client_queue[client_id].enqueue(std::tuple<std::string, size_t>(kernel_name, size));
    this->client_burst_time[client_id].end.fetch_add(size, std::memory_order_relaxed);
    this->queue_sizes[client_id].fetch_add(1, std::memory_order_seq_cst);
    std::tuple<std::string, std::string> key(client_id, kernel_name);
    KernelDataModel::getInstance()->SetKernelStatus(key, KERNEL_STATUS_RUNNING);
}

void MMFScheduler::StartSchedule()
{
    while (true)
    {
        std::tuple<std::string, std::string> key = this->ChooseKernel();
        std::string client_id = std::get<0>(key);
        std::string kernel_name = std::get<1>(key);
        if (client_id.empty())
        {
            continue;
        }
        KernelDataModel::getInstance()->enqueueKernel(key);
        kernel_queue_qize.fetch_add(1, std::memory_order_seq_cst);
    }
}

bool MMFScheduler::IsDependencyPassed(std::string client_id, std::string kernel_name)
{
    std::tuple<std::string, std::string> key(client_id, kernel_name);
    std::set<std::string> kernel_deps = KernelDependencyModel::GetInstance()->GetKernelDependecies(key);
    for (std::string dep : kernel_deps)
    {
        std::tuple<std::string, std::string> dep_key(client_id, dep);
        kernel_status_t dep_status = KernelDataModel::getInstance()->GetKernelStatus(dep_key);
        if (dep_status != KERNEL_STATUS_RUNNING && dep_status != KERNEL_STATUS_DONE)
            return false;
    }
    return true;
}