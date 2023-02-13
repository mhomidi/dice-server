#include "argument_handler.hpp"
#include <iostream>

ArgumentHandler *ArgumentHandler::instance = 0;

ArgumentHandler *ArgumentHandler::getInstance()
{
    if (instance == nullptr)
        return instance = new ArgumentHandler();

    return instance;
}

void ArgumentHandler::addBuffer(std::tuple<std::string, size_t> key, size_t size)
{
    this->bufferSizes[key] = size;
}

void ArgumentHandler::fillBufferData(std::tuple<std::string, size_t> key, std::vector<float> &data)
{
    this->bufferData[key] = data;
}

void ArgumentHandler::setBufferMemObject(std::tuple<std::string, size_t> key, cl_mem obj)
{
    this->bufferMemObjs[key] = obj;
}

cl_mem ArgumentHandler::getBufferMemObject(std::tuple<std::string, size_t> key)
{
    return this->bufferMemObjs[key];
}

size_t ArgumentHandler::getBufferSize(std::tuple<std::string, size_t> key)
{
    if (this->bufferSizes.find(key) != this->bufferSizes.end())
        return this->bufferSizes[key];
    return (size_t)(-1);
}

float *ArgumentHandler::getBufferData(std::tuple<std::string, size_t> key)
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

void ArgumentHandler::updateData(std::tuple<std::string, size_t> key, float *data, size_t size)
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