#include <map>
#include <string>
#include <queue>
#include <tuple>

#include "config.hpp"

class KernelDataModel
{
public:
    static KernelDataModel *GetInstance();
    void AddSource(std::tuple<std::string, std::string> key, std::string source);
    void setKernelArgument(std::tuple<std::string, std::string> key, std::tuple<std::string, size_t> argKey);
    void SetWorkDim(std::tuple<std::string, std::string> key, size_t dim);
    void SetGlobalWorkSize(std::tuple<std::string, std::string> key, std::vector<size_t> &globalWorkSize);
    void ClearKernel(std::tuple<std::string, std::string> key);
    void EnqueueKernel(std::tuple<std::string, std::string> key);
    void SetKernelStatus(std::tuple<std::string, std::string> key, kernel_status_t status);

    std::tuple<std::string, std::string> GetNextKernelForRun();
    std::string GetKernelSource(std::tuple<std::string, std::string> key);
    std::string GetKernelName(std::tuple<std::string, std::string> key);
    std::vector<std::tuple<std::string, size_t>> GetKernelArgument(std::tuple<std::string, std::string> key);
    size_t GetWorkDim(std::tuple<std::string, std::string> key);
    size_t *GetGlobalWorkSize(std::tuple<std::string, std::string> key);
    kernel_status_t GetKernelStatus(std::tuple<std::string, std::string> key);

private:
    KernelDataModel();
    static KernelDataModel *instance;

    std::map<std::tuple<std::string, std::string>, std::string> kernel_src;
    std::queue<std::tuple<std::string, std::string>> ready_kernel_queue;

    std::map<std::tuple<std::string, std::string>, std::vector<std::tuple<std::string, size_t>>> kernel_arguments;
    std::map<std::tuple<std::string, std::string>, std::vector<size_t>> kernel_global_work_size;
    std::map<std::tuple<std::string, std::string>, size_t> kernel_work_dim;
    std::map<std::tuple<std::string, std::string>, kernel_status_t> kernel_status;
};