#include <string>
#include <vector>
#include <tuple>
#include <atomic>

#define CL_TARGET_OPENCL_VERSION 220
#include <CL/opencl.h>

#define SLEEP_SCHED_MS 50

extern std::atomic<size_t> kernel_queue_qize;

class ExecutionHanlder
{
public:
    static ExecutionHanlder *GetInstance();
    void InstallKernel();
    void PrepareArguments();
    void RunKernel();
    void Execute();
    void DownloadData();

    void Run();

private:
    ExecutionHanlder();

    static ExecutionHanlder *instance;

    cl_context context;
    cl_command_queue command_queue;

    cl_platform_id platform_id;
    cl_device_id device_id = NULL;

    cl_program program;
    cl_kernel kernel;
    std::tuple<std::string, std::string> kernel_key;
};