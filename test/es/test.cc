#include "../../common/icsearch.hpp"

int main()
{
    std::shared_ptr<elasticlient::Client> client(new elasticlient::Client({"http://127.0.0.1:9200/"}));
    XuChat::ESIndex index(client, "test_user", "_doc");
    index.append("nickname").append("phone", "keyword", "standard", true).create();
    XuChat::ESInsert insert(client, "test_user", "_doc");
    insert.append("nickname", "name1").append("phone", "123456").insert("0001");
    insert.append("nickname", "name2").append("phone", "1256").insert("0002");
    XuChat::ESRemove remove(client, "test_user", "_doc");
    XuChat::ESSearch search(client, "test_user", "_doc");
    auto resp = search.appendMastNot("nickname.keyword", {"name1"}).search();
    for (auto r : resp)
    {
        std::cout << r.str << std::endl;
    }
    remove.remove("0001");
    remove.remove("0002");
    // cpr::Response resp;
    // try
    // {
    //     auto resp = client->search("test_user", "_doc", R"({"query":{"match_all":{}}})");
    //     if (resp.status_code < 200 || resp.status_code >= 300)
    //     {
    //         std::cout << "请求失败0" << std::endl;
    //     }
    // }
    // catch (std::exception &e)
    // {
    //     std::cout << "请求失败1" << std::endl;
    // }
    // XuChat::ESSearch::Resp res;
    // std::cout << resp.text << std::endl;
    // auto ec1 = glz::read_json(res, resp.text);
    // if (ec1)
    // {
    //     std::cout << "解析失败0" << std::endl;
    // }
    // for (auto r : res.hits["hits"])
    //     std::cout << r.str << std::endl;
    // remove.remove("788C6JMBgSW9Tknpls9m");
    return 0;
}