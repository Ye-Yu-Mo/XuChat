#include <cinatra.hpp>

using namespace cinatra;

int main()
{
    int max_thread_num = std::thread::hardware_concurrency();
    coro_http_server server(max_thread_num, 8080);
    server.set_http_handler<GET, POST>("/", [](coro_http_request &req, coro_http_response &res)
                                       { res.set_status_and_content(status_type::ok, "hello world"); });

    server.sync_start();
    return 0;
}