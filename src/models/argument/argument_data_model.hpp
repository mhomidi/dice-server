#include <map>
#include <string>
#include <vector>
#include <CL/opencl.h>
#include <tuple>

#define CHUNK_SIZE 1000 * 16

class ArgumentDataModel
{
public:
    static ArgumentDataModel *GetInstance();
    void AddBuffer(std::tuple<std::string, size_t> key, size_t size);
    void FillBufferData(std::tuple<std::string, size_t> key, std::vector<float> &data);
    size_t GetBufferSize(std::tuple<std::string, size_t> key);
    float *GetBufferData(std::tuple<std::string, size_t> key);
    void SetBufferMemObject(std::tuple<std::string, size_t> key, cl_mem obj);
    cl_mem GetBufferMemObject(std::tuple<std::string, size_t> key);
    void UpdateData(std::tuple<std::string, size_t> key, float *data, size_t size);

private:
    ArgumentDataModel() {}
    static ArgumentDataModel *instance;
    std::map<std::tuple<std::string, size_t>, size_t> buffer_sizes;
    std::map<std::tuple<std::string, size_t>, std::vector<float>> buffer_data;
    std::map<std::tuple<std::string, size_t>, cl_mem> buffer_mem_objs;
};