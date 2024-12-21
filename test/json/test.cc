#include <glaze/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ankerl/unordered_dense.h>
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
Search search;

void appendMastNot(const std::string &key, const std::vector<std::string> &values)
{
    Terms t;
    t.terms[key] = values;
    search.query["bool"].mast_not.push_back(t);  // 直接添加 Terms 对象
}

void appendShouldMatch(const std::string &key, const std::string &value)
{
    Match m;
    m.match[key] = value;
    search.query["bool"].should.push_back(m);  // 直接添加 Match 对象
}

int main()
{
    search.query["bool"] = {};  // 确保 bool 字段已初始化
    appendMastNot("user_id.keyword", {"user1", "user2", "user3"});
    appendShouldMatch("user_id", "用户ID");
    appendShouldMatch("nickname", "昵称");
    appendShouldMatch("phone", "手机号");

    std::string buffer;
    auto ec = glz::write_json(search, buffer);
    if (ec)
    {
        std::cout << "写入Json错误" << std::endl;
    }
    std::cout << "写入Json成功：" << buffer << std::endl;
    return 0;
}
