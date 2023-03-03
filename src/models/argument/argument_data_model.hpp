#include <map>
#include <string>
#include <vector>
#include <CL/opencl.h>
#include <tuple>

#define CHUNK_SIZE 1000 * 16

class ArgumentDataModel
{
public:
    static ArgumentDataModel *getInstance();
    void addBuffer(std::tuple<std::string, size_t> key, size_t size);
    void fillBufferData(std::tuple<std::string, size_t> key, std::vector<float> &data);
    size_t getBufferSize(std::tuple<std::string, size_t> key);
    float *getBufferData(std::tuple<std::string, size_t> key);
    void setBufferMemObject(std::tuple<std::string, size_t> key, cl_mem obj);
    cl_mem getBufferMemObject(std::tuple<std::string, size_t> key);
    void updateData(std::tuple<std::string, size_t> key, float *data, size_t size);

private:
    ArgumentDataModel() {}
    static ArgumentDataModel *instance;
    std::map<std::tuple<std::string, size_t>, size_t> bufferSizes;
    std::map<std::tuple<std::string, size_t>, std::vector<float>> bufferData;
    std::map<std::tuple<std::string, size_t>, cl_mem> bufferMemObjs;
};