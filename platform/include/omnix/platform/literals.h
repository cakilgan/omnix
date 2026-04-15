#ifndef OMNIX_LITERALS_H
#define OMNIX_LITERALS_H
// OXL_BYTE,OXL_TIME
#include "byte.h"
#include "time.h"

#ifdef OXL_BYTE
using ox::literals::operator""_gb;
using ox::literals::operator""_mb;
using ox::literals::operator""_kb;
using ox::literals::operator""_b;
#endif
#ifdef OXL_TIME
using ox::literals::operator""_sec;
using ox::literals::operator""_ms;
using ox::literals::operator""_us;
using ox::literals::operator""_ns;
#endif

#endif // OMNIX_LITERALS_H
