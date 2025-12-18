#ifndef _OX_TEST_H_
#define _OX_TEST_H_
#include <omnix/oxconfig.h>
#include <omnix/oxmacros.h>
#include <omnix/oxdetects.h>
#include <omnix/oxtypes.h>
#include <omnix/format/oxcolor.h>
#include <string>
#include <unordered_map>
#include <vector>


namespace OX_NAMESPACE {
    enum class test_result{
        fail,
        success
    };
    struct test_context;

    typedef test_result(*test_func)(test_context&);

    struct test_msg{
        std::string from;
        std::string type;
        std::string action;
        test_result result;
    };

    struct test{
        public:
        std::string name;
        test_func func;
    };

    struct test_context{
        public:
        std::vector<test_msg> msgs;
        std::string group,name;

        test_result add_msg(test_msg msg){
            msgs.push_back(msg);
            return msg.result;
        }
    };


    class test_registry{
        public:
        std::unordered_map<std::string, std::vector<test>> registries;
    };

    class test_reporter{
        public:
        virtual void log(const std::string& _msg,ox::Color col=Color::BLUE) = 0;
        virtual void report_test_case(test_context& cx) = 0;
    };

    class test_runner{
        public:
        std::vector<test_reporter*> reporters;
        test_registry* Registry;
        int fail = 0;
        int success = 0;
        int run_all();

        void log_all(const std::string& _msg,Color col = Color::BLUE){
            for (auto& reporter : reporters) {
                reporter->log(_msg,col);
            }
        }
        
        void test_case_report_all(test_context& cx){
            for (auto& reporter : reporters) {
                reporter->report_test_case(cx);
            }
        }
    };

    inline ox::cstr to_str(const test_result& res){
        switch (res) {
            case ox::test_result::fail:{
                return "FAIL";
            }
            case ox::test_result::success:{
                return "SUCCESS";
            }
        }
        return "UNREACHABLE";
    }

    inline ox::test_registry& registry() {
        static ox::test_registry inst;
        return inst;
    } 

    

    class stdout_test_reporter:public test_reporter{
        void log(const std::string& _msg,ox::Color col = Color::BLUE) override{
            printf("%s OXTEST: %s %s\n",ox::color(col),_msg.c_str(),ox::color(Color::DEFAULT));
        };
        void report_test_case(test_context &cx) override{
            for (auto& msg: cx.msgs) {
                printf("%s [%s] [%s] (%s) : %s %s\n",
                      msg.result==ox::test_result::fail ? ox::color(Color::RED) : ox::color(Color::GREEN),
                      msg.from.c_str(),msg.type.c_str(),msg.action.c_str(),
                      ox::to_str(msg.result),
                      ox::color(Color::DEFAULT)
                    );
            }
        };
    };
    
}

#define OXTEST(GROUP, NAME)                                  \
    static ox::test_result                                  \
    ox_test_##GROUP##_##NAME(::ox::test_context&);             \
                                                             \
    static bool ox_test_reg_##GROUP##_##NAME = []{            \
        ox::registry().registries[#GROUP].push_back({        \
            #NAME,                                           \
            ox_test_##GROUP##_##NAME                          \
        });                                                   \
        return true;                                         \
    }();                                                      \
                                                             \
    static ox::test_result                                  \
    ox_test_##GROUP##_##NAME(::ox::test_context& cx)


#define IS(x,type)\
    if(const char* _Action = #x; cx.add_msg({cx.name,"Is" type,#x,(x) ? ox::test_result::success : ox::test_result::fail})==::ox::test_result::fail)\


#define NOTICE return ::ox::test_result::fail
#define WARN printf("WARN: action %s not satisfied on %s %i\n",_Action,__FILE__,__LINE__)
#define IGNORE {}

#define RIS_TRUE(x) IS(x==true,"EqualTrue")
#define RIS_FALSE(x) IS(x==false,"EqaulFalse")
#define RIS_GREATER(x,y) IS(x>y,"GreaterThen")
#define RIS_LESSER(x,y) IS(x<y,"LesserThen")
#define RIS_NULL(x) IS(x==nullptr,"Null")
#define RIS_EQUALS(x,y) IS(x==y,"EqualTo")
#define RIS_INBETWEEN(x,y,z) IS(x>y&&x<z,"InBetween")


#define IS_TRUE(x) RIS_TRUE(x) NOTICE
#define IS_FALSE(x) RIS_FALSE(x) NOTICE
#define IS_GREATER(x,y) RIS_GREATER(x,y) NOTICE
#define IS_LESSER(x,y) RIS_LESSER(x,y) NOTICE
#define IS_NULL(x) RIS_NULL(x) NOTICE
#define IS_EQUALS(x,y) RIS_EQUALS(x,y) NOTICE
#define IS_INBETWEEN(x,y,z) RIS_INBETWEEN(x,y,z) NOTICE


#endif

    
