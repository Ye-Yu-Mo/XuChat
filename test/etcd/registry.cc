#include "../../common/etcd.hpp"
#include <gflags/gflags.h>

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    XuChat::Registry::ptr client = std::make_shared<XuChat::Registry>(FLAGS_etcd_host);
    std::string service_instance = "/friend/instance";
    client->registry(FLAGS_base_service + service_instance, "127.0.0.1:8888");

    std::this_thread::sleep_for(std::chrono::seconds(600));
    return 0;
}