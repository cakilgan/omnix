#ifndef _OX_LOG_H_
#define _OX_LOG_H_
#include <map>
#include <memory>
#include <omnix/oxconfig.h>
#include <omnix/oxmacros.h>
#include <omnix/oxdetects.h>
#include <omnix/oxtypes.h>
#include <string>
#include <vector>

namespace OX_NAMESPACE {
    enum class log_level{
        INFO,
        WARNING,
        DEBUG,
        TRACE,
        ERROR,
        FATAL,
        NO_LEVEL
    };

    struct log_message{
        struct log_pair{public:std::string key; std::string value;};
        log_pair pairs[32];
        size_t count;
    };


    struct format_element{
        int ordinal = 0;
        virtual std::string format(std::string& _use) = 0;
    };
    
    struct formatter{
        public:
        std::map<std::string, std::unique_ptr<format_element>> elements;

        std::vector<std::string> format_all(log_message& msg){
            std::vector<std::string> _value;
            OX_FOR_I(msg.count){
                auto pair = msg.pairs[i];
                if(elements.count(pair.key)==0){
                    printf("OXLOGWARN:: ignoring key: %s\n",pair.key.c_str());
                    continue;
                }
                _value.push_back(elements[pair.key]->format(pair.value));
            }
            return _value;
        }
    };

    struct channel{
        virtual void message(const log_message& msg) = 0;
        virtual ~channel() = default;
    };

    struct processor{
        public:
        std::vector<channel*> _Channels;
        processor(size_t _Size){
            _Channels.reserve(_Size);
        }        
    };
}
#endif