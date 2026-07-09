#ifndef OMNIX_LOG_H
#define OMNIX_LOG_H

#define log(l,...) ox::logger::write<ox::logger::log_event::level::l>(__VA_ARGS__)
#define inf(...) log(info,__VA_ARGS__)
#define warn(...) log(warning,__VA_ARGS__)
#define error(...) log(error,__VA_ARGS__)
#define fatal(...) log(fatal,__VA_ARGS__)
#define dbg(...) log(debug,__VA_ARGS__)
#define lifecycle(...) log(lifecycle,__VA_ARGS__)
#define trace(...) log(trace,__VA_ARGS__)
#define notice(...) log(notice,__VA_ARGS__)

#if !OX_IS(BUILD,DEBUG)
#undef trace
#undef dbg
#define trace(...) ((void)0)
#define dbg(...) ((void)0)
#endif

#define KEY(str)\
        ::ox::logger::log_field_key{\
            ox::hash<ox::cstr>{}(str),str\
        }\


#endif // OMNIX_LOG_H
