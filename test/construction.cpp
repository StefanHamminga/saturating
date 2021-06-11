#include <cstddef>
#include <cstdint>
#include <cassert>
#include <iostream>
#include "saturating/types.hpp"

using std::cerr;

void test_no_overflow_on_unsigned_construction() {
    uint_sat8_t s = 256;
    
    constexpr auto expected = 255;
    
    if (expected != s)
    {
        cerr << "failed. expected = " << expected << ", actual = " << +s << "\n";
        assert(false);
    } 
}

void test_no_underflow_on_unsigned_construction() {
    uint_sat8_t s = -1;
    
    constexpr auto expected = 0;
    
    if (expected != s)
    {
        cerr << "failed. expected = " << expected << ", actual = " << +s << "\n";
        assert(false);
    } 
}

void test_no_overflow_on_signed_construction() {
    int_sat8_t s = 129;
    
    constexpr auto expected = 127;
    
    if (expected != s)
    {
        cerr << "failed. expected = " << expected << ", actual = " << +s << "\n";
        assert(false);
    } 
}

void test_no_underflow_on_signed_construction() {
    int_sat8_t s = -129;
    
    constexpr auto expected = -128;
    
    if (expected != s)
    {
        cerr << "failed. expected = " << expected << ", actual = " << +s << "\n";
        assert(false);
    } 
}

int main () {
    test_no_overflow_on_unsigned_construction();
    test_no_underflow_on_unsigned_construction();
    test_no_overflow_on_signed_construction();
    test_no_underflow_on_signed_construction();
}
