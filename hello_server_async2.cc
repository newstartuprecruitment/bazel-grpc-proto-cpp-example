
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <sys/sysinfo.h>

#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <semaphore>
#include <string>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "hello.grpc.pb.h"

using grpc::CallbackServerContext;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::ServerUnaryReactor;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

using bazel_grpc_proto_cpp_example::Greeter;
using bazel_grpc_proto_cpp_example::HelloReply;
using bazel_grpc_proto_cpp_example::HelloRequest;

using namespace std;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

class QueueData
{
public:
    string playerId;
    string otherPlayerId;
    string gameId;
    bool isFirstPlayer;
    promise<void> p1;
    shared_future<void> isMatched;
    absl::Time addTime;
    QueueData(string playerId)
        : playerId(playerId), isMatched(p1.get_future()), addTime(absl::Now()) {}
};

class GreeterServiceImpl final : public Greeter::CallbackService
{
public:
    std::queue<shared_ptr<QueueData>> players;

    ServerUnaryReactor *SayHello(CallbackServerContext *context,
                                 const HelloRequest *request,
                                 HelloReply *reply) override
    {
        shared_ptr<QueueData> queueData = make_shared<QueueData>(request->name());
        players.push(queueData);

        ABSL_LOG(INFO) << "added";
        queueData->isMatched.wait();
        ABSL_LOG(INFO) << "future done";
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());

        ServerUnaryReactor *reactor = context->DefaultReactor();
        reactor->Finish(Status::OK);
        return reactor;
    }
    void consumer()
    {
        while (true)
        {
            if (players.size() >= 2)
            {
                shared_ptr<QueueData> entry1 = players.front();
                players.pop();
                shared_ptr<QueueData> entry2 = players.front();
                players.pop();
                entry1->p1.set_value();
                entry2->p1.set_value();
            }
        }
    }
    void RunConsumerAsync()
    {
        std::thread consumerThread(&GreeterServiceImpl::consumer, this);
        consumerThread.detach();
    }
};

void Run()
{
    string server_address =
        absl::StrFormat("0.0.0.0:%d", absl::GetFlag(FLAGS_port));
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    GreeterServiceImpl service;
    service.RunConsumerAsync();
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    ABSL_LOG(INFO) << "Server listening on: " << server_address;
    server->Wait();
}

int main(int argc, char **argv)
{
    absl::ParseCommandLine(argc, argv);
    Run();
    return 0;
}