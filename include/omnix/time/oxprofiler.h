#ifndef _OX_PROFILER_H_
#define _OX_PROFILER_H_

#define OX_PROFILE_SCOPE(Reference) ox::scope_profiler{Reference};

#include <omnix/oxconfig.h>
#include <omnix/oxmacros.h>
#include <omnix/oxdetects.h>
#include <omnix/oxtypes.h>

#include "oxtime.h"

namespace OX_NAMESPACE {
    struct timer{
        private:
        ox::time _Ref{0,time_t::nanoseconds};
        public:
        void reset(){_Ref = ox::now();}
        ox::time elapsed(){return (ox::now()-_Ref);}
    };

    struct scope_profiler{
        private:
        time_v _RawStart;
        time& _Result;
        public:
        inline scope_profiler(time& use) noexcept
        :_Result(use),_RawStart(now().value()){}

        inline ~scope_profiler() noexcept{
            _Result.value_ref() = now().value()-_RawStart;
        }
    };
}
#endif