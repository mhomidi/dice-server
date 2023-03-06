
#include <map>
#include <set>
#include <string>

class KernelDependencyModel
{
public:
    static KernelDependencyModel *GetInstance();

private:
    KernelDependencyModel() {}

    static KernelDependencyModel *instance;
    std::map<std::map<std::string, std::string>, std::set<std::string>> kernel_dep;
};