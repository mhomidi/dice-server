

#include "kernel_depend_model.hpp"

KernelDependencyModel *KernelDependencyModel::instance = nullptr;

KernelDependencyModel *KernelDependencyModel::GetInstance()
{
    if (instance == nullptr)
        return instance = new KernelDependencyModel();
    return instance;
}

void KernelDependencyModel::SetKernelDependancy(std::tuple<std::string, std::string> key, std::string dep_kernel)
{
    this->kernel_dep[key].insert(dep_kernel);
}

std::set<std::string> KernelDependencyModel::GetKernelDependecies(std::tuple<std::string, std::string> key)
{
    if (this->kernel_dep.find(key) == this->kernel_dep.end())
        return std::set<std::string>();
    return this->kernel_dep[key];
}