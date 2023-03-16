
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
    static MMFScheduler *GetInstance();
    void StartSchedule();
    void EnqueueKernel(std::string client_id, std::string kernel_name, size_t size);

private:
    MMFScheduler() {}
    std::tuple<std::string, std::string> ChooseKernel();
    bool IsDependencyPassed(std::string client_id, std::string kernel_name);

    static MMFScheduler *instance;
    std::map<std::string, std::atomic<size_t>> queue_sizes;
    std::map<std::string, moodycamel::ReaderWriterQueue<std::tuple<std::string, size_t>>> client_queue;
    std::map<std::string, BurstTime> client_burst_time;
};