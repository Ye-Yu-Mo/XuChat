# XuChat

C++23

微服务聊天系统的服务端

## 依赖的库

-lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb -ldl

1. [Xulog](https://github.com/Ye-Yu-Mo/LogSystem) 日志库
2. [gflags](https://github.com/gflags/gflags) 命令行参数解析框架
3. [gtest](https://github.com/google/gtest-parallel) 单元测试框架
4. [etcd](https://github.com/etcd-cpp-apiv3/etcd-cpp-apiv3) 分布式键值存储系统 用于服务注册与发现
5. [brpc](https://github.com/apache/brpc) rpc框架
6. [unordered_dense](https://github.com/martinus/unordered_dense)  高速unordered_map和unordered_set
7. [Glaze](https://github.com/stephenberry/glaze) 高速Json库 C++23
8. [cinatra](https://github.com/qicosmos/cinatra?tab=readme-ov-file) 协程http框架