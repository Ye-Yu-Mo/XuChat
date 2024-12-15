#include "../../common/channel.hpp"
#include "../../common/etcd.hpp"
#include "main.pb.h"
#include <brpc/channel.h>
#include <gflags/gflags.h>

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    auto sm = std::make_shared<XuChat::ServiceManager>();
    sm->declared("/service/echo");
    auto online = std::bind(&XuChat::ServiceManager::serviceOnline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto offline = std::bind(&XuChat::ServiceManager::serviceOffline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    XuChat::Discovery::ptr client = std::make_shared<XuChat::Discovery>(FLAGS_etcd_host, FLAGS_base_service, online, offline);
    XuChat::ServiceChannel::ChannelPtr channel = channel = sm->choose("/service/echo");
    // std::cout << "to here" << std::endl;
    while (true)
    {
        if (!channel)
        {
            channel = sm->choose("/service/echo");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        example::EchoService_Stub stub(channel.get());
        std::cout << "输入:";
        std::string buffer;
        std::cin >> buffer;
        example::EchoRequest req;
        req.set_message(buffer);

        brpc::Controller *cntl = new brpc::Controller();
        example::EchoResponse *resp = new example::EchoResponse();
        stub.Echo(cntl, &req, resp, nullptr);
        if (cntl->Failed())
        {
            std::cout << "rpc调用失败" << std::endl;
            return -1;
        }
        std::cout << "收到响应：" << resp->message() << std::endl;
        delete cntl;
        delete resp;
    }

    std::this_thread::sleep_for(std::chrono::seconds(600));
    return 0;
}