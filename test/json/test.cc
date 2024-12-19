#include <glaze/json.hpp>
// #include <glaze/glaze_exceptions.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ankerl/unordered_dense.h>
struct student
{
    std::string name;
    int age;
    std::vector<float> score;
};

struct Propertie
{
    std::string type;
    std::string analyzer;
    bool enabled;

    // Propertie(const std::string &type = "text",
    //           const std::string &analyzer = "ik_max_word",
    //           bool enabled = true)
    //     : type(type), analyzer(analyzer), enabled(enabled) {}
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
int main()
{
    student stu;
    stu.name = "张三";
    stu.age = 18;
    stu.score = {100, 80, 90};
    Config conf;
    conf.settings.analysis.analyzer.ik.tokenizer = "ik_max_word";
    conf.mappings.dynamic = true;
    conf.mappings.properties["昵称"] = {"text", "standard"};
    conf.mappings.properties["用户id"] = {"keyword", "ik_max_word"};
    std::string buffer{};
    auto ec = glz::write_json(conf, buffer);
    if (ec)
    {
        std::cout << "写入Json错误" << std::endl;
    }
    std::cout << "写入Json成功：" << buffer << std::endl;
    return 0;
}