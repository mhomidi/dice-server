
#include <map>
#include <queue>
#include <tuple>
#include <atomic>
#include "readerwriterqueue.h"

typedef struct
{
    std::atomic<size_t> start = {0};
    std::atomic<size_t> end = {0};
} BurstTime;

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
    std::map<std::string, moodycamel::ReaderWriterQueue<std::tuple<std::string, size_t>>> clientQueue;
    std::map<std::string, BurstTime> clientBurstTime;
};