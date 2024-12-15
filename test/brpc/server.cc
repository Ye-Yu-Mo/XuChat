#include <brpc/server.h>
#include <butil/logging.h>
#include "main.pb.h"

using namespace example;

// 创建子类 继承 EchoService 实现rpc调用的功能
class EchoServiceImpl : public EchoService
{
public:
    EchoServiceImpl() {}
    ~EchoServiceImpl() {}
    void Echo(google::protobuf::RpcController *controller,
              const ::example::EchoRequest *request,
              ::example::EchoResponse *response,
              ::google::protobuf::Closure *done)
    {
        brpc::ClosureGuard rpc_guard(done);
        std::cout << "收到消息：" << request->message() << std::endl;
        std::string str = request->message() + " 这是响应";
        response->set_message(str);
    }

private:
};

int main(int argc, char *argv[])
{
    // 创建服务器对象
    brpc::Server server;
    // 新增服务
    EchoServiceImpl echo_service;
    server.AddService(&echo_service, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE);
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1; // 超时时间
    options.num_threads = 1;       // io线程数量

    int ret = server.Start(8080, &options); // 启动服务

    return 0;
}
