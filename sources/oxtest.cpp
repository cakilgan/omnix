#include "omnix/format/oxcolor.h"
#include <iostream>
#include <omnix/test/oxtest.h>
#include <string>
#include <vector>


ox::test_runner _Runner{};

int ox::test_runner::run_all(){
    int g_f_c = 0;
    int g_s_c = 0;
    for (auto& reg : ox::registry().registries) {
        log_all("GROUP "+reg.first);
        for (auto& c : reg.second) {
            log_all("TEST "+c.name);

            ox::test_context cx{};
            cx.group = reg.first;
            cx.name = c.name;
            cx.msgs.push_back({c.name.c_str(),"MAIN_RESULT","return",c.func(cx)});
            
            if(cx.msgs.back().result==test_result::fail){
                g_f_c++;
            }else{
                g_s_c++;
            }
            test_case_report_all(cx);
        }
    }

    log_all("END OXTEST success:"+std::to_string(g_s_c)+" fail:"+std::to_string(g_f_c),g_f_c>0 ? Color::RED : Color::GREEN);
    return g_f_c>0 ? 1 :0;
}
int main(){
    ox::stdout_test_reporter rep{};
    _Runner.reporters.push_back(
        &rep
    );
    _Runner.Registry = &ox::registry();
    return _Runner.run_all();
}