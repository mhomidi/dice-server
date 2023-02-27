
#include <map>
#include <queue>
#include <tuple>
#include <atomic>

class MMFScheduler
{
public:
    static MMFScheduler *getInstance();
    void startSchedule();
    void enqueueKernel(std::string clientId, std::string kernelName, size_t size);

private:
    MMFScheduler() {}
    std::tuple<std::string, std::string> chooseKernel();

    static MMFScheduler *instance;
    std::map<std::string, std::atomic<size_t>> queueSizes;
    std::map<std::string, std::queue<std::string>> clientQueue;
    std::map<std::string, std::tuple<size_t, size_t>> clientBurstTime;
};