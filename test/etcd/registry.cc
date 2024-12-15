#include "../../common/etcd.hpp"
#include "../../common/channel.hpp"
#include "main.pb.h"
#include <gflags/gflags.h>
#include <brpc/server.h>
#include <butil/logging.h>

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");

// 创建子类 继承 EchoService 实现rpc调用的功能
class EchoServiceImpl : public example::EchoService
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
    google::ParseCommandLineFlags(&argc, &argv, true);

    logging::LoggingSettings settings;
    settings.logging_dest = logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);

    brpc::Server server;

    EchoServiceImpl echo_service;
    server.AddService(&echo_service, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE);
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1; // 超时时间
    options.num_threads = 1;       // io线程数量

    int ret = server.Start(8080, &options); // 启动服务
    XuChat::Registry::ptr client = std::make_shared<XuChat::Registry>(FLAGS_etcd_host);
    std::string service_instance = "/echo/instance";
    client->registry(FLAGS_base_service + service_instance, "127.0.0.1:8080");


    server.RunUntilAskedToQuit();
    return 0;
}