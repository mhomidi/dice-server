#include "server.h"

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
    KernelHandler::getInstance()->addSource(key, request->source());
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::CreateBuffer(grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> key(clientId, request->id());
    ArgumentHandler::getInstance()->addBuffer(key, request->size());
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
    ArgumentHandler::getInstance()->fillBufferData(key, data);
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
    KernelHandler::getInstance()->setGlobalWorkSize(key, wgs);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelWorkDim(grpc::ServerContext *context, const ::tvmgrpc::WorkDim *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(clientId, request->kernel_name());
    KernelHandler::getInstance()->setWorkDim(key, request->dim());
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetBufferToKernel(grpc::ServerContext *context, const ::tvmgrpc::BufferSet *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> kernelKey(clientId, request->kernel_name());
    std::tuple<std::string, size_t> bufferKey(clientId, request->buffer_id());
    KernelHandler::getInstance()->setKernelArgument(kernelKey, bufferKey);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelReadyToExecute(::grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(clientId, request->name());
    KernelHandler::getInstance()->enqueueKernel(key);
    kernelQueueSize.fetch_add(1, std::memory_order_seq_cst);
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::GetBufferData(::grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::grpc::ServerWriter<::tvmgrpc::BufferData> *writer)
{
    std::string clientId = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> bufferKey(clientId, request->id());
    float *data = ArgumentHandler::getInstance()->getBufferData(bufferKey);
    for (size_t i = 0; i < request->size() / sizeof(float); i++)
    {
        tvmgrpc::BufferData bufferData;
        bufferData.set_data(data[i]);
        writer->Write(bufferData);
    }
    this->clientReqNum[clientId].fetch_add(1, std::memory_order_seq_cst);
    return grpc::Status::OK;
}
