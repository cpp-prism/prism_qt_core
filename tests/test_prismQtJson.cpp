#define CATCH_CONFIG_RUNNER
#include "../prism_qt_core/include/hpp/prismQtJson.hpp"
#include "tests/models/test_model.h"
#include <catch2/catch_all.hpp>
#include <hpp/prismQt.hpp>

#ifdef _MSC_VER
#define _AMD64_
#include <ConsoleApi2.h>
#include <winnls.h>
#endif

using prism::qt::core::prismModelProxy;

struct t2 {
    int my_int = 1;
};
PRISM_FIELDS(t2,my_int);
PRISMQT_TYPE(t2);

struct t1{
    ~t1()
    {
        delete ptr_t2;
    }
    std::shared_ptr<t2> sptr_t2 = std::make_shared<t2>();
    t2* ptr_t2 = new t2{};
};
PRISM_FIELDS(t1,sptr_t2,ptr_t2);
PRISMQT_TYPE(t1);


TEST_CASE("指针结构体绑定测试")
{
    prismModelProxy<t1> proxy = new prismModelProxy<t1>(nullptr);
    int backup = proxy.instance()->ptr_t2->my_int;
    int backup2 = proxy.instance()->sptr_t2->my_int;
    std::cout << backup << std::endl;
    std::cout << backup2 << std::endl;
    REQUIRE(backup == proxy.get("ptr_t2.my_int").toInt());
    REQUIRE(backup2 == proxy.get("sptr_t2.my_int").toInt());


}

TEST_CASE("qt json测试")
{
    st_test model;
    std::string json = prism::json::toJsonString(model, 4);
    std::cout << json << std::endl;

    std::shared_ptr<st_test> deserialization = prism::json::fromJsonString<st_test>(std::move(json));

    std::string json2 = prism::json::toJsonString(*deserialization, 4);
    std::cout << json2 << std::endl;
    REQUIRE(json == json2);
}

int main(int argc, const char** argv)
{
#ifdef _MSC_VER
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    int result = Catch::Session().run(argc, argv);
    return result;
}
