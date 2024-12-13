#include "../../common/etcd.hpp"
#include <gflags/gflags.h>

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");

void online(const std::string &service_name, const std::string &service_host)
{
    debug(XuChat::logger, "新上线服务：%s-%s", service_name.c_str(), service_host.c_str());
}

void offline(const std::string &service_name, const std::string &service_host)
{
    debug(XuChat::logger, "下线服务：%s-%s", service_name.c_str(), service_host.c_str());
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    XuChat::Discovery::ptr client = std::make_shared<XuChat::Discovery>(FLAGS_etcd_host, FLAGS_base_service, online, offline);

    std::this_thread::sleep_for(std::chrono::seconds(600));
    return 0;
}