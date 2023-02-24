#include "server.h"
#include <thread>

void sched()
{
    ExecutionHanlder::getInstance()->schedule();
}

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "debug def" << std::endl;
#endif
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