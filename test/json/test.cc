#include <glaze/json.hpp>
// #include <glaze/glaze_exceptions.hpp>
#include <string>
#include <iostream>
#include <vector>
struct student
{
    std::string name;
    int age;
    std::vector<float> score;
};

int main()
{
    student stu;
    stu.name = "张三";
    stu.age = 18;
    stu.score = {100, 80, 90};

    std::string buffer{};
    auto ec = glz::write_json(stu, buffer);
    if (ec) {
        std::cout << "写入Json错误" << std::endl;
    }
    std::cout << "写入Json成功：" << buffer << std::endl;
    return 0;
}