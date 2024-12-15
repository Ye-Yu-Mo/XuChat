#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <brpc/channel.h>
#include <ankerl/unordered_dense.h>
#include <unordered_map>
#include <unordered_set>
#include "logger.hpp"
namespace XuChat
{

    /// @class ServiceChannel
    /// @brief 单个服务的信道管理类
    /// 不直接管理rpc服务 而是管理rpc服务的信道
    class ServiceChannel
    {
    public:
        using ptr = std::shared_ptr<ServiceChannel>;       ///< 单个服务信道管理句柄
        using ChannelPtr = std::shared_ptr<brpc::Channel>; ///< rpc信道指针

        ServiceChannel(const std::string &name)
            : _service_name(name), _index(0) {}

        /// @brief 服务上线节点 调用append新增信道
        /// @param host 节点服务器地址
        void append(const std::string &host)
        {
            auto channel = std::make_shared<brpc::Channel>();
            brpc::ChannelOptions options;
            options.connect_timeout_ms = -1;
            options.timeout_ms = -1;
            options.max_retry = 3;
            options.protocol = "baidu_std";
            int ret = channel->Init(host.c_str(), &options);
            if (ret == -1)
            {
                log_error(logger, "初始化 %s-%s 服务信道失败", _service_name.c_str(), host.c_str());
                return;
            }
            std::unique_lock<std::mutex> lock(_mutex);
            _hosts.insert(std::make_pair(host, channel));
            _channels.push_back(channel);
        }
        /// @brief 服务下线节点 调用 remove 删除信道
        /// @param host 节点服务器地址
        void remove(const std::string &host)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _hosts.find(host);
            if (it == _hosts.end())
            {
                warn(logger, "删除 %s-%s 服务信道时 没有找到信道信息", _service_name.c_str(), host.c_str());
                return;
            }
            for (auto vit = _channels.begin(); vit != _channels.end(); ++vit)
            {
                if (*vit == it->second)
                {
                    _channels.erase(vit);
                    break;
                }
            }
            _hosts.erase(it);
        }
        /// @brief RR轮转策略 获取Channel用于发起对应服务的rpc调用
        ChannelPtr choose()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_channels.size() == 0)
            {
                log_error(logger, "当前没有任何节点提供 %s 服务", _service_name.c_str());
                return ChannelPtr();
            }
            int32_t idx = (_index++) % _channels.size();
            return _channels[idx];
        }

    private:
        std::mutex _mutex;                                            ///< 互斥锁
        int32_t _index;                                               ///< 当前轮转下标计数器
        std::string _service_name;                                    ///< 服务名称
        std::vector<ChannelPtr> _channels;                            ///< 当前服务对应的信道集合 采用RR轮转策略负载均衡 为什么不用map或者set 因为需要使用下标轮转
        ankerl::unordered_dense::map<std::string, ChannelPtr> _hosts; ///< 主机地址与信道的映射关系
    };

    class ServiceManager
    {
    public:
        using ptr = std::shared_ptr<ServiceManager>; ///< 服务管理句柄
        ServiceManager() = default;
        ServiceChannel::ChannelPtr choose(const std::string &service_name)
        {
            // std::string service_name = getServiceName(service_instance);
            std::unique_lock<std::mutex> lock(_mutex);
            auto sit = _services.find(service_name);
            if (sit == _services.end())
            {
                log_error(logger, "没有能够提供 %s 服务的节点", service_name.c_str());
                return ServiceChannel::ChannelPtr();
            }
            return sit->second->choose();
        }
        // 先声明关注哪些服务的上线下线 不关心的服务就不需要管理
        void declared(const std::string &service_name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _focus_services.insert(service_name);
            info(logger, "%s 服务已设置关心", service_name.c_str());
        }
        // 上线时的回调接口 自动插入服务管理
        void serviceOnline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto fit = _focus_services.find(service_name);
                if (fit == _focus_services.end())
                {
                    info(logger, "新增 %s-%s 服务 未设置关心", service_name.c_str(), host.c_str());
                    return;
                }
                // 获取管理对象 如果没有则 创建 如果有则 添加新节点
                auto sit = _services.find(service_name);
                if (sit == _services.end())
                {
                    service = std::make_shared<ServiceChannel>(service_name);
                    _services.insert(std::make_pair(service_name, service));
                    return;
                }
                service = sit->second;
            }
            if (!service)
            {
                log_error(logger, "新增 %s-%s 服务失败", service_name.c_str(), host.c_str());
                return;
            }
            service->append(host);
            info(logger, "%s-%s 服务已经上线", service_name.c_str(), host.c_str());

        }
        // 上线时的回调接口 自动删除服务管理
        void serviceOffline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto fit = _focus_services.find(service_name);
                if (fit == _focus_services.end())
                {
                    info(logger, "下线 %s-%s 服务 未设置关心", service_name.c_str(), host.c_str());
                    return;
                }

                auto sit = _services.find(service_name);
                if (sit == _services.end())
                {
                    warn(logger, "删除 %s-%s 服务时 没有找到管理对象", service_name.c_str(), host.c_str());
                    return;
                }
                service = sit->second;
            }
            service->remove(service_name);
            info(logger, "%s-%s 服务已经下线", service_name.c_str(), host.c_str());
        }
    private:
        std::string getServiceName(const std::string &service_name)
        {
            auto it  = service_name.find_last_of('/');
            if(it == std::string::npos)
                return service_name;
            return service_name.substr(0, it);
        }
    private:
        std::mutex _mutex; ///< 互斥锁
        ankerl::unordered_dense::set<std::string> _focus_services;
        ankerl::unordered_dense::map<std::string, ServiceChannel::ptr> _services; ///< 所有服务的信道管理接口
        // std::unordered_set<std::string> _focus_services;
        // std::unordered_map<std::string, ServiceChannel::ptr> _services; ///< 所有服务的信道管理接口
    };
}