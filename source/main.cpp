#include "tests/test.h"
#include "tracy/Tracy.hpp"

#ifndef TRACY_ENABLE
#define TRACY_ENABLE
#endif

#undef TRACY_ENABLE

int main(void)
{
    ZoneScoped;
    #ifdef TRACY_ENABLE
    while(!TracyIsConnected)
    {
        // Wait for Tracy to connect
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    #endif
    // test_transmitter();
    test_receiver();
}