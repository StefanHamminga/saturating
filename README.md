# C++ saturating types
C++ header only library for always saturating integral types.

## Description

This library provides the following new integral types:
```
int_sat8_t;
uint_sat8_t;

int_sat16_t;
uint_sat16_t;

int_sat32_t;
uint_sat32_t;

int_sat64_t;
uint_sat64_t;
```
And if 128 bit integers are supported:
```
int_sat128_t;
uint_sat128_t;
```

Custom types can be declared by calling the universal template:

```c++
// Standard declaration: -128, …, 127
typedef xint_sat_t<int8_t> int_sat8_t;

// Custom lower limit: -127, …, 127
typedef xint_sat_t<int8_t, -127> custom1_t;

// Custom limit: 16, …, 32
typedef xint_sat_t<int8_t, 16, 32> custom2_t;
```

Some notable features:
    - Saturating signed and unsigned types can be mixed

## Usage example

```c++
#include <cstddef>
#include <cstdint>
#include "saturating_types.hpp"

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
}
```

## License, author, contributors

The saturated types library is written by [Stefan Hamminga](stefan@prjct.net), with contributions by [Toby Speight](https://codereview.stackexchange.com/questions/179172/c17-saturating-integer-arithmetic-type-library).

This work is released under the terms of the Apache 2.0 license. The complete text is included in the `LICENSE` file.

## Repository

https://github.com/StefanHamminga/saturating_types
