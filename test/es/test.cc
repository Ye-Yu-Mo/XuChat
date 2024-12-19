#include "../../common/icsearch.hpp"

int main()
{
    std::shared_ptr<elasticlient::Client> client(new elasticlient::Client({"http://127.0.0.1:9200/"}));
    XuChat::ESIndex index(client, "test_user", "_doc");
    index.append("nickname").append("phone", "keyword", "standard", true).create();
    return 0;
}