#pragma once
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Value.hpp>
#include "logger.hpp"

namespace XuChat
{
    /// @brief 服务注册客户端类
    class Registry
    {
    public:
        using ptr = std::shared_ptr<Registry>;
        Registry(const std::string &host)
            : _client(std::make_shared<etcd::Client>(host)), _keep_alive(_client->leasekeepalive(3).get()), _lease_id(_keep_alive->Lease()) {}
        ~Registry() { _keep_alive->Cancel(); }
        bool registry(const std::string &key, const std::string &val)
        {
            auto resp = _client->put(key, val, _lease_id).get();
            if (resp.is_ok() == false)
            {
                log_error(logger, "新增数据失败：%s", resp.error_message().c_str());
                return false;
            }
            return true;
        }

    private:
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::KeepAlive> _keep_alive;
        uint64_t _lease_id;
    };

    /// @brief 服务发现客户端类
    class Discovery
    {
    public:
        using ptr = std::shared_ptr<Discovery>;
        using NotifyCallback = std::function<void(std::string, std::string)>; ///< 服务发现回调函数 第一个参数是服务信息 第二个参数是主机地址
        Discovery(const std::string &host, const std::string &basedir,
                  const NotifyCallback &put_cb, const NotifyCallback &del_cb)
            : _client(std::make_shared<etcd::Client>(host)),
              _put_cb(put_cb),
              _del_cb(del_cb)
        {
            // 定义服务发现 获取到当前已有的信息
            auto resp = _client->ls(basedir).get();
            if (resp.is_ok() == false)
            {
                log_error(logger, "获取服务信息数据失败：%s", resp.error_message().c_str());
            }
            int sz = resp.keys().size();
            for (int i = 0; i < sz; ++i)
            {
                if (_put_cb)
                    _put_cb(resp.key(i), resp.value(i).as_string());
            }
            // 进行事件监控 监控数据发生的改变 进行处理
            _watcher = std::make_shared<etcd::Watcher>(*_client.get(), basedir,
                                                       std::bind(&Discovery::callback, this, std::placeholders::_1), true);
        }

    private:
        void callback(const etcd::Response &resp)
        {
            if (resp.is_ok() == false)
            {
                warn(logger, "收到一个错误的事件通知：%d", resp.error_message().c_str());
                return;
            }
            for (auto const &ev : resp.events())
            {
                if (ev.event_type() == etcd::Event::EventType::PUT)
                {
                    if(_put_cb)
                    {
                        _put_cb(ev.kv().key(), ev.kv().as_string());
                        info(logger, "新增服务：%s-%s", ev.kv().key().c_str(), ev.kv().as_string().c_str());
                    }
                }
                else if (ev.event_type() == etcd::Event::EventType::DELETE_)
                {
                    if(_del_cb)
                    {
                        _del_cb(ev.prev_kv().key(), ev.prev_kv().as_string());
                        info(logger, "删除服务：%s-%s", ev.prev_kv().key().c_str(), ev.prev_kv().as_string().c_str());
                    }
                }
            }
        }

    private:
        NotifyCallback _put_cb;
        NotifyCallback _del_cb;
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::Watcher> _watcher;
    };
}