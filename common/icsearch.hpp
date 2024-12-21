#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <glaze/json.hpp>
#include <ankerl/unordered_dense.h>
#include <iostream>
#include <memory>
#include "logger.hpp"

namespace XuChat
{
    /// @brief ES索引 用于创建索引和字段
    class ESIndex
    {
    public:
        using Client = std::shared_ptr<elasticlient::Client>;

        ESIndex(Client client, const std::string &idx_name, const std::string &type)
            : _name(idx_name), _type(type), _client(client)
        {
            _conf.settings.analysis.analyzer.ik.tokenizer = "ik_max_word";
        }
        ESIndex &append(const std::string &key, const std::string &type = "text",
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

    class ESInsert
    {
    public:
        using Client = std::shared_ptr<elasticlient::Client>;

        ESInsert(Client client, const std::string &idx_name, const std::string &type)
            : _name(idx_name), _type(type), _client(client)
        {
        }

        ESInsert &append(const std::string &key, const std::string &value)
        {
            _items[key] = value;
            return *this;
        }

        bool insert(const std::string &id = "")
        {
            std::string body{};
            auto ec = glz::write_json(_items, body);
            if (ec)
                log_error(logger, "新增数据序列化失败! %s", ec.custom_error_message.data());
            // info(logger, "请求正文: %s", body.c_str());
            try
            {
                auto resp = _client->index(_name, _type, id, body);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    log_error(logger, "新增数据 %s 请求失败! 状态码异常 %d:%s", _name, resp.status_code, resp.error.message.c_str());
                    return false;
                }
            }
            catch (std::exception &e)
            {
                log_error(logger, "新增数据 %s 请求失败! %s", _name, e.what());
                return false;
            }
            debug(logger, "新增数据创建成功!");
            return true;
        }

    private:
        std::string _name;
        std::string _type;
        Client _client;
        ankerl::unordered_dense::map<std::string, std::string> _items;
    };

    class ESRemove
    {
    public:
        using Client = std::shared_ptr<elasticlient::Client>;

        ESRemove(Client client, const std::string &idx_name, const std::string &type)
            : _name(idx_name), _type(type), _client(client)
        {
        }

        bool remove(const std::string &id)
        {
            try
            {
                auto resp = _client->remove(_name, _type, id);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    log_error(logger, "删除数据 %s 请求失败! 状态码异常 %d:%s", _name, resp.status_code, resp.error.message.c_str());
                    return false;
                }
            }
            catch (std::exception &e)
            {
                log_error(logger, "删除数据 %s 请求失败! %s", _name, e.what());
                return false;
            }
            debug(logger, "删除数据创建成功!");
            return true;
        }

    private:
        std::string _name;
        std::string _type;
        Client _client;
    };

    class ESSearch
    {
    public:
        using Client = std::shared_ptr<elasticlient::Client>;

        ESSearch(Client client, const std::string &idx_name, const std::string &type)
            : _name(idx_name), _type(type), _client(client)
        {
            _search.query["bool"] = {};
        }
        ESSearch &appendMastNot(const std::string &key, const std::vector<std::string> &values)
        {
            Terms t;
            t.terms[key] = values;
            _search.query["bool"].mast_not.push_back(t);
            return *this;
        }
        ESSearch &appendShouldMatch(const std::string &key, const std::string &value)
        {
            Match m;
            m.match[key] = value;
            _search.query["bool"].should.push_back(m);
            return *this;
        }
        bool search()
        {
            std::string body;
            auto ec = glz::write_json(_search, body);
            if (ec)
                log_error(logger, "检索请求序列化失败! %s", ec.custom_error_message.data());
            try
            {
                auto resp = _client->search(_name, _type, body);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    log_error(logger, "检索 %s 请求失败! 状态码异常 %d:%s", _name, resp.status_code, resp.error.message.c_str());
                    return false;
                }
            }
            catch (std::exception &e)
            {
                log_error(logger, "检索数据 %s 请求失败! %s", _name, e.what());
                return false;
            }
            debug(logger, "检索数据成功!");
            return true;
        }

        struct Terms
        {
            ankerl::unordered_dense::map<std::string, std::vector<std::string>> terms;
        };

        struct Match
        {
            ankerl::unordered_dense::map<std::string, std::string> match;
        };

        struct Bool
        {
            std::vector<Terms> mast_not;
            std::vector<Match> should;
        };

        struct Search
        {
            ankerl::unordered_dense::map<std::string, Bool> query;
        };

    private:
        std::string _name;
        std::string _type;
        Client _client;
        Search _search;
    };
}