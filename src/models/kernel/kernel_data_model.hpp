#include <map>
#include <string>
#include <queue>
#include <tuple>

class KernelDataModel
{
public:
    static KernelDataModel *getInstance();
    void addSource(std::tuple<std::string, std::string> key, std::string source);
    void setKernelArgument(std::tuple<std::string, std::string> kernelKey, std::tuple<std::string, size_t> argKey);
    void setWorkDim(std::tuple<std::string, std::string> kernelKey, size_t dim);
    void setGlobalWorkSize(std::tuple<std::string, std::string> kernelKey, std::vector<size_t> &globalWorkSize);
    void clearKernel(std::tuple<std::string, std::string> kernelKey);
    void enqueueKernel(std::tuple<std::string, std::string> key);

    std::tuple<std::string, std::string> getNextKernelForRun();
    std::string getKernelSource(std::tuple<std::string, std::string> key);
    std::string getKernelName(std::tuple<std::string, std::string> key);
    std::vector<std::tuple<std::string, size_t>> getKernelArgument(std::tuple<std::string, std::string> kernelKey);
    size_t getWorkDim(std::tuple<std::string, std::string> kernelKey);
    size_t *getGlobalWorkSize(std::tuple<std::string, std::string> kernelKey);
    size_t getReadyQueueSize();

private:
    KernelDataModel();
    static KernelDataModel *instance;

    std::map<std::tuple<std::string, std::string>, std::string> kernelSources;
    std::queue<std::tuple<std::string, std::string>> readyKernelQueue;

    std::map<std::tuple<std::string, std::string>, std::vector<std::tuple<std::string, size_t>>> kernelsArguments;
    std::map<std::tuple<std::string, std::string>, std::vector<size_t>> kernelGlobalWorkSize;
    std::map<std::tuple<std::string, std::string>, size_t> kernelWorkDim;
};