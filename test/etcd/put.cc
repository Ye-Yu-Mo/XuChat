#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <thread>
int main()
{
    std::string etcd_host = "http://127.0.0.1:2379";
    // 实例化客户端对象
    etcd::Client client(etcd_host);
    // 获取租约保活对象 创建一个指定有效时长的租约
    auto keep_alive = client.leasekeepalive(3).get(); // 3s
    auto lease_id = keep_alive->Lease();
    // 新增数据
    auto resp1 = client.put("/service/user", "127.0.0.1:8080", lease_id).get();
    if (resp1.is_ok() == false)
    {
        std::cout << "新增数据失败" << std::endl;
        return -1;
    }

    auto resp2 = client.put("/service/friend", "127.0.0.1:8888", lease_id).get();
    if (resp2.is_ok() == false)
    {
        std::cout << "新增数据失败" << std::endl;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
