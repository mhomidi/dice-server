#include <CL/opencl.h>
#include <string>
#include <vector>
#include <tuple>
#include <atomic>

#define SLEEP_SCHED_MS 50

extern std::atomic<size_t> kernel_queue_qize;

class ExecutionHanlder
{
public:
    static ExecutionHanlder *getInstance();
    void installKernel();
    void prepareArguments();
    void runKernel();
    void execute();
    void downloadData();

    void run();

private:
    ExecutionHanlder();

    static ExecutionHanlder *instance;

    cl_context context;
    cl_command_queue commandQueue;

    cl_platform_id platform_id;
    cl_device_id device_id = NULL;

    cl_program program;
    cl_kernel kernel;
    std::tuple<std::string, std::string> kernelKey;

    std::vector<cl_mem> kernelMemObjs;
};