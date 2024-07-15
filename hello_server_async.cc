
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

// Logic and data behind the server's behavior.
class ServerImpl final
{
    std::queue<shared_ptr<QueueData>> players;

    std::vector<std::unique_ptr<ServerCompletionQueue>> cq;
    Greeter::AsyncService service;
    std::unique_ptr<Server> server;
    std::vector<std::thread> server_threads;

public:
    ~ServerImpl()
    {
        server->Shutdown();
        // Always shutdown the completion queue after the server.
        for (auto &x : cq)
            x->Shutdown();
        for (auto &x : server_threads)
        {
            if (x.joinable())
            {
                x.join();
            }
        }
    }

    class CallData
    {
    public:
        CallData(Greeter::AsyncService *service, ServerCompletionQueue *cq, ServerImpl *serverImpl)
            : service(service),
              cq(cq),
              responder(&ctx),
              status(CREATE),
              serverImpl(serverImpl)
        {
            // Invoke the serving logic right away.
            Proceed();
        }
        void Proceed()
        {
            if (status == CREATE)
            {
                status = PROCESS;
                service->RequestSayHello(&ctx, &request, &responder, cq, cq, this);
                ABSL_LOG(INFO) << "got request: " << request.DebugString();
            }
            else if (status == PROCESS)
            {
                std::async(std::launch::async,
                           &ServerImpl::CallData::processRequestAsync, this);
            }
            else
            {
                ABSL_LOG(INFO) << "finish";
                GPR_ASSERT(status == FINISH);
                delete this;
            }
        }

        void processRequestAsync()
        {
            ABSL_LOG(INFO) << "processing request: " << request.DebugString();

            new CallData(service, cq, serverImpl);

            shared_ptr<QueueData> queueData = make_shared<QueueData>(request.name());
            serverImpl->players.push(queueData);

            ABSL_LOG(INFO) << "added";
            queueData->isMatched.wait();
            ABSL_LOG(INFO) << "future done";
            reply.set_message("hello" + request.name());
            status = FINISH;
            responder.Finish(reply, Status::OK, this);
            ABSL_LOG(INFO) << "Game created: " << reply.DebugString();
        }

        Greeter::AsyncService *service;
        ServerCompletionQueue *cq;
        ServerContext ctx;
        HelloRequest request;
        HelloReply reply;
        ServerAsyncResponseWriter<HelloReply> responder;
        enum CallStatus
        {
            CREATE,
            PROCESS,
            FINISH
        };
        CallStatus status; // The current serving state.
        ServerImpl *serverImpl;
    };

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
        std::thread consumerThread(&ServerImpl::consumer, this);
        consumerThread.detach();
    }

    void HandleRpcs(int i)
    {
        new CallData(&service, cq[i].get(), this);
        void *requestId;
        bool ok;
        while (true)
        {
            GPR_ASSERT(cq[i]->Next(&requestId, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData *>(requestId)->Proceed();
        }
    }

    void Run()
    {
        int numOfThreads = 2;
        string server_address =
            absl::StrFormat("0.0.0.0:%d", absl::GetFlag(FLAGS_port));
        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        for (int i = 0; i < numOfThreads; i++)
        {
            cq.emplace_back(builder.AddCompletionQueue());
        }
        server = builder.BuildAndStart();
        ABSL_LOG(INFO) << "Server listening on: " << server_address;
        for (int i = 0; i < numOfThreads; i++)
        {
            server_threads.emplace_back(std::thread([this, i]
                                                    { this->HandleRpcs(i); }));
        }
        while (true)
        {
        }
    }
};

int main(int argc, char **argv)
{
    absl::ParseCommandLine(argc, argv);
    ServerImpl server;
    server.RunConsumerAsync();
    server.Run();
    return 0;
}