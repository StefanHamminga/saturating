# C++ saturating arithmetic functions and types
C++ header only library for always saturating functions and types.
For a quick introduction to saturation arithmetic take a look at the [Wikipedia](https://en.wikipedia.org/wiki/Saturation_arithmetic) page.

## Description

The library features two entry points: [`functions.hpp`](https://github.com/StefanHamminga/saturating/blob/master/functions.hpp) and [`types.hpp`](https://github.com/StefanHamminga/saturating/blob/master/types.hpp).

### functions.hpp
The functions header provides the namespace `saturating` containing `add`, `subtract`, `multiply`, and `divide`, each taking two arguments and returning a _plain_ value of a type that can fit either argument. Simplified this comes down to a combination of promotion to signed or floating point, and increasing the type size. The library aims to remove as many type conversions pitfalls as possible. This includes avoiding unintended `int` => `unsigned` promotions and properly rounding floating point results back to integrals.

Several smaller utility functions are provided in the namespace, for a quick overview check [`utilities.hpp`](https://github.com/StefanHamminga/saturating/blob/master/utilities.hpp)

### types.hpp

This header includes the above functions header and extends this to provide the `saturating::type` template class, allowing to create automatically saturating types. The types use saturating operators by default, but returning saturating types where possible, allowing saturation to be respected throughout a chain of operations.

Several `std` namespaced functions, like `std::is_arithmetic` or `std::numeric_limits<T>` are specialized to integrate saturating types more seamlessly.

The following default integral types are introduced to the global namespace:

```cpp
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

// And these floating point types, if their primitives are available:

float_sat_t;
double_sat_t;
long_double_sat_t;
```

This functionality can be disabled by defining `SATURATING_TYPES_h_NO_GLOBALS`.

Custom types can be created using the universal template:

```cpp
// Standard declaration: -128, …, 127
typedef saturating::type<int8_t> int_sat8_t;

// Custom lower limit: -127, …, 127
typedef saturating::type<int8_t, -127> custom1_t;

// Custom limit: 16, …, 32
typedef saturating::type<int8_t, 16, 32> custom2_t;

// Floating points have a default -1 … 1 range, but can take any integral limits
typedef saturating::type<double, -10, 10> custom3_t;

```

## Dependencies

Other than a modern C++17 compiler this library depends on:

- [arithmetic_type_tools](https://github.com/StefanHamminga/arithmetic_type_tools) - Tools used to manage the required type upgrades.
- [cpp_lib_scripts](https://github.com/StefanHamminga/cpp_lib_scripts) - Automate building and installation.

## Usage example

If you want to install the libraries systems wide (Linux) do:

```bash
git clone https://github.com/StefanHamminga/saturating saturating
cd saturating
mkdir build
cd build/

cmake -DCMAKE_INSTALL_PREFIX=/usr ..

# Optionally run the tests
make check

sudo make install

# Libraries are now available as:
# #include <saturating/functions.hpp>
# or
# #include <saturating/types.hpp>
```

A small example C++ source:

```cpp
#include <cstddef>
#include <cstdint>
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
}
```

### Tests

Run `make check` to build and run the test program.

## License, author, contributors

The saturated types library is written by [Stefan Hamminga](stefan@prjct.net), with contributions by [Toby Speight](https://codereview.stackexchange.com/questions/179172/c17-saturating-integer-arithmetic-type-library).

This work is released under the terms of the Apache 2.0 license. The complete text is included in the `LICENSE` file.

## Repository

[saturating (GitHub)](https://github.com/StefanHamminga/saturating)
