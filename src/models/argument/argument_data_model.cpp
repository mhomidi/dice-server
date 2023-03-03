#include "argument_data_model.hpp"
#include <iostream>

ArgumentDataModel *ArgumentDataModel::instance = 0;

ArgumentDataModel *ArgumentDataModel::getInstance()
{
    if (instance == nullptr)
        return instance = new ArgumentDataModel();

    return instance;
}

void ArgumentDataModel::addBuffer(std::tuple<std::string, size_t> key, size_t size)
{
    this->bufferSizes[key] = size;
}

void ArgumentDataModel::fillBufferData(std::tuple<std::string, size_t> key, std::vector<float> &data)
{
    this->bufferData[key] = data;
}

void ArgumentDataModel::setBufferMemObject(std::tuple<std::string, size_t> key, cl_mem obj)
{
    this->bufferMemObjs[key] = obj;
}

cl_mem ArgumentDataModel::getBufferMemObject(std::tuple<std::string, size_t> key)
{
    return this->bufferMemObjs[key];
}

size_t ArgumentDataModel::getBufferSize(std::tuple<std::string, size_t> key)
{
    if (this->bufferSizes.find(key) != this->bufferSizes.end())
        return this->bufferSizes[key];
    return (size_t)(-1);
}

float *ArgumentDataModel::getBufferData(std::tuple<std::string, size_t> key)
{
    if (this->bufferData[key].size() != (this->bufferSizes[key] / sizeof(float)))
    {
        for (size_t i = 0; i < (this->bufferSizes[key] / sizeof(float)); i++)
        {
            this->bufferData[key].push_back(0.0);
        }
    }
    return this->bufferData[key].data();
}

void ArgumentDataModel::updateData(std::tuple<std::string, size_t> key, float *data, size_t size)
{
    if (this->bufferData.find(key) == this->bufferData.end())
    {
        std::cout << "BAD update data ..." << std::endl;
        throw 1;
    }

    this->bufferData[key].clear();
    for (int i = 0; i < size / sizeof(float); i++)
    {
        this->bufferData[key].push_back(data[i]);
    }
}