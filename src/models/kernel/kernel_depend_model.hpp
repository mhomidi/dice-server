
#include <map>
#include <set>
#include <string>

class KernelDependencyModel
{
public:
    static KernelDependencyModel *GetInstance();
    void SetKernelDependancy(std::tuple<std::string, std::string> key, std::string dep_kernel);
    std::set<std::string> GetKernelDependecies(std::tuple<std::string, std::string> key);

private:
    KernelDependencyModel() {}

    static KernelDependencyModel *instance;
    std::map<std::tuple<std::string, std::string>, std::set<std::string>> kernel_dep;
};