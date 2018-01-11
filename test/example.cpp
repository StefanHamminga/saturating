#include <cstddef>
#include <cstdint>
#include <cassert>
#include "saturating/types.hpp"

uint8_t x[] { 101, 27, 3, 95 };

int main () {
    uint_sat8_t s = 25;

    for (const auto& v : x) {
        s -= v;
    } // s == 0
    s++; // s == 1
    for (const auto& v : x) {
        s *= v;
    }

    volatile unsigned j = s; // s == 255
    assert(s == 255);
}
