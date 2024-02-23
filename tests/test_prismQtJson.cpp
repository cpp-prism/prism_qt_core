#define CATCH_CONFIG_RUNNER
#include "include/prism/qt/core/hpp/prismQt.hpp"
#include "include/prism/qt/core/hpp/prismQtJson.hpp"
#include "include/prism/qt/core/hpp/prismDataAccessLayer.hpp"
#include "tests/models/test_model.h"
#include <prism/prismSql.hpp>
#include <catch2/catch_all.hpp>
#include <prism/qt/modular/dynamic_lib_caller.h>
#include <QApplication>
#include <thread>
#include <sstream>

#ifdef _MSC_VER
#define _AMD64_
#include <ConsoleApi2.h>
#include <winnls.h>
#endif

using prism::qt::core::prismModelProxy;


int argc = 0;
QApplication a(argc,(char**)nullptr);

TEST_CASE("tst qt core")
{
    qRegisterMetaType<prism::qt::core::Sql_logic_base*>("Sql_logic_base*");
    qRegisterMetaType<std::condition_variable*>("std::condition_variable*");
    qRegisterMetaType<bool*>("bool*");


    SECTION("tst database access")
    {
        //读线分离
        prism::qt::core::prismDataAccessLayer<prism::sql::sqlite3::Sqlite3> dal_write ("tst_sqlite_write_cached_name","test_sqlite3.db");
        prism::qt::core::prismDataAccessLayer<prism::sql::sqlite3::Sqlite3> dal_read ("tst_sqlite_read_cached_name","test_sqlite3.db");

        //删除表
        dal_write.deleteTable<testTable>();


        //建表
        dal_write.createTable<testTable>();

        //插入数据
        int id = 100;
        dal_write.insert<testTable>({
                                  std::make_shared<testTable>(++id),
                                  std::make_shared<testTable>(++id),
                                  std::make_shared<testTable>(++id),
                                  std::make_shared<testTable>(++id)
                              });



        //查询数据
        std::shared_ptr<std::vector<std::shared_ptr<testTable>>> query_result =  dal_read.queryTable<testTable>();
        for(std::shared_ptr<testTable> item : *query_result)
        {
            prism::reflection::for_each_fields(*item,[](const char* fname,auto&&value){
                if(std::strcmp(fname,"my_int") ==0 || std::strcmp(fname,"my_str") ==0 )
                    std::cout << fname << " : " << value << std::endl;
            });
        }

    }










    SECTION("动态库调用")
    {
        SKIP();
#ifdef _WIN32
        void* lib =prism::qt::modular::dynamic_lib_caller::loadLib(R"(D:\Users\user\Documents\source\repos\dv_app_solution\prism_all\build-prism_qt_core-Desktop_Qt_5_15_2_MSVC2019_64bit\Debug\pureCppLib.dll)");
#else
        void* lib =prism::qt::modular::dynamic_lib_caller::loadLib(R"(/home/deepvision/source/repos/build-pureCppLib-Desktop_Qt_5_15_2_GCC_64bit-Debug/libpureCppLib.so.1.0.0)");
#endif
        if(lib)
        {
            void* func = prism::qt::modular::dynamic_lib_caller::getFunctionAddr(lib,"avg");
            if(func)
            {
                double value = -1;
                typedef int (*avgFuc)(int, int);
                avgFuc f = (avgFuc)(func);

                if(f)
                {
                    value = f(6,15);

                    prism::qt::modular::dynamic_lib_caller::unloadLib(lib);
                }

                std::cout << value;
            }
        }
    }

    SECTION("指针结构体绑定测试")
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

    SECTION("qt json测试")
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
