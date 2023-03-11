

#include "kernel_depend_model.hpp"

KernelDependencyModel *KernelDependencyModel::instance = nullptr;

KernelDependencyModel *KernelDependencyModel::GetInstance()
{
    if (instance == nullptr)
        return instance = new KernelDependencyModel();
    return instance;
}

