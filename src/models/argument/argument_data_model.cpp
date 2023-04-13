#include "argument_data_model.hpp"
#include <iostream>

ArgumentDataModel *ArgumentDataModel::instance = 0;

ArgumentDataModel *ArgumentDataModel::GetInstance()
{
    if (instance == nullptr)
        return instance = new ArgumentDataModel();

    return instance;
}

void ArgumentDataModel::AddBuffer(std::tuple<std::string, size_t> key, size_t size)
{
    this->buffer_sizes[key] = size;
}

void ArgumentDataModel::FillBufferData(std::tuple<std::string, size_t> key, std::vector<float> &data)
{
    this->buffer_data[key] = data;
}

void ArgumentDataModel::SetBufferMemObject(std::tuple<std::string, size_t> key, cl_mem obj)
{
    this->buffer_mem_objs[key] = obj;
}

cl_mem ArgumentDataModel::GetBufferMemObject(std::tuple<std::string, size_t> key)
{
    return this->buffer_mem_objs[key];
}

size_t ArgumentDataModel::GetBufferSize(std::tuple<std::string, size_t> key)
{
    if (this->buffer_sizes.find(key) != this->buffer_sizes.end())
        return this->buffer_sizes[key];
    return (size_t)(-1);
}

float *ArgumentDataModel::GetBufferData(std::tuple<std::string, size_t> key)
{
    if (this->buffer_data[key].size() != (this->buffer_sizes[key] / sizeof(float)))
    {
        for (size_t i = 0; i < (this->buffer_sizes[key] / sizeof(float)); i++)
        {
            this->buffer_data[key].push_back(0.0);
        }
    }
    return this->buffer_data[key].data();
}

void ArgumentDataModel::UpdateData(std::tuple<std::string, size_t> key, float *data, size_t size)
{
    if (this->buffer_data.find(key) == this->buffer_data.end())
    {
        std::cout << "BAD update data ..." << std::endl;
        throw 1;
    }

    this->buffer_data[key].clear();
    for (int i = 0; i < size / sizeof(float); i++)
    {
        this->buffer_data[key].push_back(data[i]);
    }
}