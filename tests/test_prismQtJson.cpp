#define CATCH_CONFIG_RUNNER
#include "include/prism/qt/core/hpp/prismQtJson.hpp"
#include "tests/models/test_model.h"
#include <catch2/catch_all.hpp>
#include "include/prism/qt/core/hpp/prismQt.hpp"
#include <prism/qt/modular/dynamic_lib_caller.h>

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

TEST_CASE("动态库调用")
{
#ifdef _WIN32
    void* lib =dynamic_lib_caller::loadLib(R"(D:\Users\user\Documents\source\repos\dv_app_solution\prism_all\build-prism_qt_core-Desktop_Qt_5_15_2_MSVC2019_64bit\Debug\pureCppLib.dll)");
#else
    void* lib =dynamic_lib_caller::loadLib(R"(/home/deepvision/source/repos/build-pureCppLib-Desktop_Qt_5_15_2_GCC_64bit-Debug/libpureCppLib.so.1.0.0)");
#endif

    if(lib)
    {
        void* func = dynamic_lib_caller::getFunctionAddr(lib,"avg");
        if(func)
        {
            double value = -1;
            typedef int (*avgFuc)(int, int);
            avgFuc f = (avgFuc)(func);

            if(f)
            {
                value = f(6,15);

                dynamic_lib_caller::unloadLib(lib);
            }

            std::cout << value;
        }
    }


}

TEST_CASE("指针结构体绑定测试")
{

    SKIP();
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

    SKIP();

    {
        //序列化
        std::shared_ptr<prism::qt::core::prismTreeModelProxy<st_test>> modelproxy = std::make_shared<prism::qt::core::prismTreeModelProxy<st_test>>();
        std::shared_ptr<prism::qt::core::prismTreeNodeProxy<st_test>> root = std::make_shared<prism::qt::core::prismTreeNodeProxy<st_test>>();
        std::shared_ptr<prism::qt::core::prismTreeNodeProxy<st_test>> sub1 = std::make_shared<prism::qt::core::prismTreeNodeProxy<st_test>>();
        std::shared_ptr<prism::qt::core::prismTreeNodeProxy<st_test>> sub2 = std::make_shared<prism::qt::core::prismTreeNodeProxy<st_test>>();
        std::shared_ptr<prism::qt::core::prismTreeNodeProxy<st_test>> sub11 = std::make_shared<prism::qt::core::prismTreeNodeProxy<st_test>>();
        std::shared_ptr<prism::qt::core::prismTreeNodeProxy<st_test>> sub12 = std::make_shared<prism::qt::core::prismTreeNodeProxy<st_test>>();
        root->appendChild(sub1);
        root->appendChild(sub2);
        sub1->appendChild(sub11);
        sub1->appendChild(sub12);
        modelproxy->setRootNode(root);
        std::string json = prism::json::toJsonString(modelproxy,4);
        std::cout << json << std::endl;

        ////反序列化
        std::shared_ptr<prism::qt::core::prismTreeModelProxy<st_test>>  de_model =
                prism::json::fromJsonString<prism::qt::core::prismTreeModelProxy<st_test>>(json);
        //反序列化后再序列化
        std::string json2 = prism::json::toJsonString(de_model, 4);

        std::cout << json2 << std::endl;
        REQUIRE(json == json2);
    }
    SKIP();


    {
        st_test model;
        std::string json = prism::json::toJsonString(model, 4);
        std::cout << json << std::endl;

        std::shared_ptr<st_test> deserialization = prism::json::fromJsonString<st_test>(std::move(json));

        std::string json2 = prism::json::toJsonString(*deserialization, 4);
        std::cout << json2 << std::endl;
        REQUIRE(json == json2);
    }
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
