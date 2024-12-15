#include <brpc/channel.h>
#include "main.pb.h"

using namespace example;

void callback(google::protobuf::RpcController *cntl,
              const ::example::EchoResponse *response)
{
    std::cout << response->message() << std::endl;
    delete cntl;
    delete response;
}

int main(int argc, char *argv[])
{
    // 构造channel信道 连接服务器
    brpc::ChannelOptions options;
    options.connect_timeout_ms = -1;
    options.timeout_ms = -1;
    options.max_retry = 3;
    options.protocol = "baidu_std";
    brpc::Channel channel();
    channel.Init("127.0.0.1:8080", &options);
    // 构造 EchoService_Stub 对象 用于rpc调用
    EchoService_Stub stub(&channel);
    // rpc调用 获取响应输出
    EchoRequest req;
    req.set_message("hello world!");
    brpc::Controller *cntl = new brpc::Controller;
    EchoResponse *resp = new EchoResponse;
    auto clusure = google::protobuf::NewCallback(callback, cntl, resp);
    stub.Echo(cntl, &req, resp, clusure);

    return 0;
}