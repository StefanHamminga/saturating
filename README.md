# C++ saturating types
C++ header only library for always saturating types.
For a quick introduction to saturation arithmetic take a look at the [Wikipedia](https://en.wikipedia.org/wiki/Saturation_arithmetic) page.



## Description

This library provides the following default integral types:

```c++
int_sat8_t;
uint_sat8_t;

int_sat16_t;
uint_sat16_t;

int_sat32_t;
uint_sat32_t;

int_sat64_t;
uint_sat64_t;

// If 128 bit integers are supported:
int_sat128_t;
uint_sat128_t;
```

And the these default floating point types:

```c++
float_sat_t;
double_sat_t;
```

Custom types can be created using the universal template:

```c++
// Standard declaration: -128, …, 127
typedef x_sat_t<int8_t> int_sat8_t;

// Custom lower limit: -127, …, 127
typedef x_sat_t<int8_t, -127> custom1_t;

// Custom limit: 16, …, 32
typedef x_sat_t<int8_t, 16, 32> custom2_t;
```

### Some notable features

- Saturating signed and unsigned types can be mixed and scaled
- Customisable lower and upper bounds
- Optimized functions are chosen compile-time
- Static functions can be used to create and scale on the fly

### Usage example

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

### Tests

Run `make check` to build and run the test program.

## License, author, contributors

The saturated types library is written by [Stefan Hamminga](stefan@prjct.net), with contributions by [Toby Speight](https://codereview.stackexchange.com/questions/179172/c17-saturating-integer-arithmetic-type-library).

This work is released under the terms of the Apache 2.0 license. The complete text is included in the `LICENSE` file.

## Repository

[saturating_types (GitHub)](https://github.com/StefanHamminga/saturating_types)
