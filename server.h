
#include <service.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include "argument_handler.hpp"
#include "kernel_handler.hpp"
#include "execution.hpp"

#include <iostream>
#include <vector>

#define CLIENT_ID "client_id"

class Server final : public tvmgrpc::VortexService::Service
{
public:
    virtual grpc::Status GenerateNewUserId(::grpc::ServerContext *context, const ::tvmgrpc::EmptyMessage *request, ::tvmgrpc::UserData *response);
    virtual grpc::Status SetKernelSource(grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response);
    virtual grpc::Status CreateBuffer(grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::tvmgrpc::Response *response);
    virtual grpc::Status SetBufferData(grpc::ServerContext *context, grpc::ServerReader<::tvmgrpc::BufferData> *reader, ::tvmgrpc::Response *response);
    virtual grpc::Status SetWorkGroupData(grpc::ServerContext *context, grpc::ServerReader<::tvmgrpc::WorkGroupData> *reader, ::tvmgrpc::Response *response);
    virtual grpc::Status SetKernelWorkDim(grpc::ServerContext *context, const ::tvmgrpc::WorkDim *request, ::tvmgrpc::Response *response);
    virtual grpc::Status SetBufferToKernel(grpc::ServerContext *context, const ::tvmgrpc::BufferSet *request, ::tvmgrpc::Response *response);
    virtual grpc::Status SetKernelReadyToExecute(::grpc::ServerContext *context, const ::tvmgrpc::KernelSource *request, ::tvmgrpc::Response *response);
    virtual grpc::Status Exec(::grpc::ServerContext *context, const ::tvmgrpc::ExecMessage *request, ::tvmgrpc::Response *response);
    virtual grpc::Status GetBufferData(::grpc::ServerContext *context, const ::tvmgrpc::BufferCreation *request, ::grpc::ServerWriter<::tvmgrpc::BufferData> *writer);
};