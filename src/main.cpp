#include "server.h"
#include <thread>
#include "scheduler.hpp"

void exec()
{
    ExecutionHanlder::GetInstance()->Run();
}

void sched()
{
    MMFScheduler::GetInstance()->StartSchedule();
}

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "debug def" << std::endl;
#endif
    std::thread executor(exec);
    std::thread scheduler(sched);

    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());

    std::cout << "Server listen on localhost:50051" << std::endl;

    Server myServer;
    builder.RegisterService(&myServer);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
}