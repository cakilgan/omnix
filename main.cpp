//
// Created by cakilgan on 3/5/26.
//

#include <omnix/platform.h>
#include <fstream>
#include <cstdio>

int main() {
    auto memhndl = ox::memory::alloc(ox::megabytes(1));
    if (!memhndl) {
        return memhndl.err();
    }
    auto &mem = memhndl.value();

    return 0;
}