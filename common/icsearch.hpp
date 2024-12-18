#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <glaze/json.hpp>
#include <ankerl/unordered_dense.h>
#include <iostream>
#include <memory>
#include "logger.hpp"

namespace XuChat
{
    class ESIndex
    {
    public:
        using Client = std::shared_ptr<elasticlient::Client>;

        ESIndex(Client client, const std::string &idx_name, const std::string &type)
            : _name(idx_name), _type(type), _client(client)
        {
            _conf.settings.analysis.analyzer.ik.tokenizer = "ik_max_word";
        }
        ESIndex& append(const std::string &key, const std::string &type = "text",
                    const std::string &analyzer = "ik_max_word", bool enabled = true)
        {
            _conf.mappings.properties[key] =
                {.type = type,
                 .analyzer = analyzer,
                 .enabled = enabled};
            return *this;
        }

        bool create()
        {
            _conf.mappings.dynamic = true;
            std::string body{};
            auto ec = glz::write_json(_conf, body);
            if (ec)
                log_error(logger, "索引序列化失败! %s", ec.custom_error_message.data());
            // info(logger, "请求正文: %s", body.c_str());
            try
            {
                auto resp = _client->index(_name, _type, "", body);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    log_error(logger, "创建ES索引 %s 请求失败! 状态码异常 %d:%s", _name, resp.status_code, resp.error.message.c_str());
                    return false;
                }
            }
            catch (std::exception &e)
            {
                log_error(logger, "创建ES索引 %s 请求失败! %s", _name, e.what());
                return false;
            }
            debug(logger, "测试索引创建成功!");
            return true;
        }

        struct Propertie
        {
            std::string type;
            std::string analyzer;
            bool enabled;
        };

        struct Mappings
        {
            bool dynamic;
            ankerl::unordered_dense::map<std::string, Propertie> properties;
        };

        struct Ik
        {
            std::string tokenizer;
        };

        struct Analyzer
        {
            Ik ik;
        };

        struct Analysis
        {
            Analyzer analyzer;
        };

        struct Settings
        {
            Analysis analysis;
        };

        struct Config
        {
            Mappings mappings;
            Settings settings;
        };

    private:
        std::string _name;
        std::string _type;
        Config _conf;
        Client _client;
    };
}