#include "server.h"
#include "scheduler.hpp"
#include "kernel_depend_model.hpp"

long unsigned int user_id = 0;

grpc::Status Server::GenerateNewUserId(::grpc::ServerContext *context, const ::tvmgrpc::EmptyMessage *request, ::tvmgrpc::UserData *response)
{
    response->set_user_id(user_id);
    this->client_req_num[std::to_string(user_id)].store(1, std::memory_order_seq_cst);
    user_id += 1;
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelSource(grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(client_id, request->name());
    KernelDataModel::GetInstance()->AddSource(key, request->source());
    KernelDataModel::GetInstance()->SetKernelStatus(key, KERNEL_STATUS_WAITING);
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::CreateBuffer(grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::tvmgrpc::Response *response)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> key(client_id, request->id());
    ArgumentDataModel::GetInstance()->AddBuffer(key, request->size());
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetBufferData(grpc::ServerContext *context, grpc::ServerReader<::tvmgrpc::BufferData> *reader, ::tvmgrpc::Response *response)
{
    tvmgrpc::BufferData buffer_data;
    std::vector<float> data;

    while (reader->Read(&buffer_data))
    {
        data.push_back(buffer_data.data());
    }
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> key(client_id, buffer_data.id());
    ArgumentDataModel::GetInstance()->FillBufferData(key, data);
    data.clear();
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
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
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(client_id, wg.kernel_name());
    KernelDataModel::GetInstance()->SetGlobalWorkSize(key, wgs);
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelWorkDim(grpc::ServerContext *context, const ::tvmgrpc::WorkDim *request, ::tvmgrpc::Response *response)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(client_id, request->kernel_name());
    KernelDataModel::GetInstance()->SetWorkDim(key, request->dim());
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetBufferToKernel(grpc::ServerContext *context, const ::tvmgrpc::BufferSet *request, ::tvmgrpc::Response *response)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> kernel_key(client_id, request->kernel_name());
    std::tuple<std::string, size_t> buffer_key(client_id, request->buffer_id());
    KernelDataModel::GetInstance()->setKernelArgument(kernel_key, buffer_key);
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelReadyToExecute(::grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, std::string> key(client_id, request->name());
    // TODO: Burst time estimator should be added here.
    MMFScheduler::GetInstance()->EnqueueKernel(client_id, request->name(), 100);
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    response->set_status(0);
    return grpc::Status::OK;
}

grpc::Status Server::GetBufferData(::grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::grpc::ServerWriter<::tvmgrpc::BufferData> *writer)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::tuple<std::string, size_t> buffer_key(client_id, request->id());
    float *data = ArgumentDataModel::GetInstance()->GetBufferData(buffer_key);
    for (size_t i = 0; i < request->size() / sizeof(float); i++)
    {
        tvmgrpc::BufferData buffer_data;
        buffer_data.set_data(data[i]);
        writer->Write(buffer_data);
    }
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    return grpc::Status::OK;
}

grpc::Status Server::SetKernelDependency(::grpc::ServerContext *context, const ::tvmgrpc::KernelDependency *request, ::tvmgrpc::Response *response)
{
    std::string client_id = context->client_metadata().find(CLIENT_ID)->second.data();
    std::string pred = request->pred();
    std::string curr = request->curr();
    std::tuple<std::string, std::string> key(client_id, curr);
    KernelDependencyModel::GetInstance()->SetKernelDependancy(key, pred);
    this->client_req_num[client_id].fetch_add(1, std::memory_order_seq_cst);
    return grpc::Status::OK;
}
