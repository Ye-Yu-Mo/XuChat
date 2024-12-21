#include <glaze/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ankerl/unordered_dense.h>
#include <array>
#include <map>


// 数据结构定义

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

// Global variable
Search search;

void appendMastNot(const std::string &key, const std::vector<std::string> &values)
{
    Terms t;
    t.terms[key] = values;
    search.query["bool"].mast_not.push_back(t); // 直接添加 Terms 对象
}

void appendShouldMatch(const std::string &key, const std::string &value)
{
    Match m;
    m.match[key] = value;
    search.query["bool"].should.push_back(m); // 直接添加 Match 对象
}

struct Shards{
    uint64_t total;
    uint64_t successful;
    uint64_t skipped;
    uint64_t failed;
};

struct Hits
{
    glz::raw_json_view total;
    float max_score;
    std::vector<glz::raw_json_view> hits;
};

struct my_struct
{
    int took;
    bool timed_out;
    glz::raw_json_view _shards;
    Hits hits;
};

int main()
{
    // 解析 my_struct 数据
    std::string buffer2 = R"({
    "took":0,"timed_out":false,
    "_shards":{"total":1,"successful":1,"skipped":0,"failed":0},
    "hits":{
        "total":{"value":1,"relation":"eq"},
        "max_score":0.6931471,
        "hits":[{"_index":"test_user","_type":"_doc","_id":"0001","_score":0.6931471,"_source":{"nickname":"name1","phone":"123456"}}]
    }
    })";
    my_struct s{};
    auto ec1 = glz::read_json(s, buffer2); // populates s from buffer
    if (ec1)
    {
        // handle error
        std::cout << "失败 " << std::endl;
    }
    else
    {
        std::cout << "成功\n"; 

    }
    return 0;
}
