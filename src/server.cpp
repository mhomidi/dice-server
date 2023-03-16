#include "server.h"
#include "scheduler.hpp"
#include "kernel_depend_model.hpp"

long unsigned int userId = 0;

grpc::Status Server::GenerateNewUserId(::grpc::ServerContext *context, const ::tvmgrpc::EmptyMessage *request, ::tvmgrpc::UserData *response)
{
    response->set_user_id(userId);
    this->clientReqNum[std::to_string(userId)].store(1, std::memory_order_seq_cst);
    userId += 1;
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelSource(grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(clientId, request->name());
    KernelDataModel::getInstance()->addSource(key, request->source());
    KernelDataModel::getInstance()->SetKernelStatus(key, KERNEL_STATUS_WAITING);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::CreateBuffer(grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> key(clientId, request->id());
    ArgumentDataModel::getInstance()->addBuffer(key, request->size());
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetBufferData(grpc::ServerContext *context, grpc::ServerReader<::tvmgrpc::BufferData> *reader, ::tvmgrpc::Response *response)
{
    tvmgrpc::BufferData bufferData;
    std::vector<float> data;

    while (reader->Read(&bufferData))
    {
        data.push_back(bufferData.data());
    }
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> key(clientId, bufferData.id());
    ArgumentDataModel::getInstance()->fillBufferData(key, data);
    data.clear();
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetWorkGroupData(grpc::ServerContext *context, grpc::ServerReader<::tvmgrpc::WorkGroupData> *reader, ::tvmgrpc::Response *response)
{
    tvmgrpc::WorkGroupData wg;
    std::vector<size_t> wgs;
    while (reader->Read(&wg))
    {
        wgs.push_back(wg.size());
    }
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(clientId, wg.kernel_name());
    KernelDataModel::getInstance()->setGlobalWorkSize(key, wgs);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelWorkDim(grpc::ServerContext *context, const ::tvmgrpc::WorkDim *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(clientId, request->kernel_name());
    KernelDataModel::getInstance()->setWorkDim(key, request->dim());
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetBufferToKernel(grpc::ServerContext *context, const ::tvmgrpc::BufferSet *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> kernelKey(clientId, request->kernel_name());
    std::tuple<std::string, size_t> bufferKey(clientId, request->buffer_id());
    KernelDataModel::getInstance()->setKernelArgument(kernelKey, bufferKey);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelReadyToExecute(::grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(clientId, request->name());
    // TODO: Burst time estimator should be added here.
    MMFScheduler::GetInstance()->EnqueueKernel(clientId, request->name(), 100);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::GetBufferData(::grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::grpc::ServerWriter<::tvmgrpc::BufferData> *writer)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> bufferKey(clientId, request->id());
    float *data = ArgumentDataModel::getInstance()->getBufferData(bufferKey);
    for (size_t i = 0; i < request->size() / sizeof(float); i++)
    {
        tvmgrpc::BufferData bufferData;
        bufferData.set_data(data[i]);
        writer->Write(bufferData);
    }
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelDependency(::grpc::ServerContext *context, const ::tvmgrpc::KernelDependency *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::string pred = request->pred();
    std::string curr = request->curr();
    std::tuple<std::string, std::string> key(clientId, curr);
    KernelDependencyModel::GetInstance()->SetKernelDependancy(key, pred);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    return grpc::Status::OK;
}
