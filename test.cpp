#include <iostream>
#include <cassert>
#include "saturating_types.hpp"

using namespace std;

template <typename T>
void sweep() {
    for (__int128_t i = (__int128_t)T::min_val - 2; i < (__int128_t)T::max_val + 2; ++i) {
        const auto temp = T::from(i);
        typename T::type check = std::max((__int128_t)T::min_val, std::min(i, (__int128_t)T::max_val));
        assert(check == temp);
    }
}

int main() {
    cout << "Running some random saturated type tests..." << endl;

    cout << "Doing an sweep of creating a clamped type for: " << flush;

    cout << "i8 " << flush;
    sweep<int_sat8_t>();
    cout << "✓, u8 " << flush;
    sweep<uint_sat8_t>();
    cout << "✓, i16 " << flush;
    sweep<int_sat16_t>();
    cout << "✓, u16 " << flush;
    sweep<uint_sat16_t>();
    // Enable these to extend coffee break:
    // cout << "✓, i32 " << flush;
    // sweep<int_sat32_t>();
    // cout << "✓, u32 " << flush;
    // sweep<uint_sat32_t>();
    // cout << "✓, i64 " << flush;
    // sweep<int_sat64_t>();
    // cout << "✓, u64 " << flush;
    // sweep<uint_sat64_t>();
    cout << "✓." << endl;


    {
        // The constructor doesn't force any default into the accepted range:
        uint_sat8_t u8a = -5 ;
        cout << "Saturating 8-bit unsigned int directly set to -5: " << (int)u8a << endl;

        // But the `from()` function does:
        auto u8b = uint_sat8_t::from(-5);
        cout << "Saturating 8-bit unsigned int converted from -5: " << (int)u8b << endl;

        int_sat32_t i32a { -1000'000 };

        auto i16a1 = int_sat16_t::from(i32a);
        auto i16a2 = int_sat16_t::scale_from(i32a);

        cout << "Converting a 32-bit int, " << i32a << ", to a 16-bit int can be done in two ways:" << endl;
        cout << "Clamping using `from()`: " << i16a1 << endl;
        cout << "Scaling using `scale_from()`: " << i16a2 << endl;

        xint_sat_t<int, -1024, 1023> b { -640 };
        cout << "Scale 1:8: " << (int)b << " to " << (int)int_sat8_t::scale_from(b) << endl;

        // Inverting scales is possible, though I don't know why you should.
        xint_sat_t<int16_t, 127, -128> c {4};
        cout << "Inverting scale: " << (int)c << " to " << (int)int_sat8_t::scale_from(c) << endl;

        auto scaled = int_sat8_t::scale_from(0.25f); // Uses defaults: -1 and 1
        cout << "Converting a -1 to 1 ranged float, 0.25, to a saturating 8-bit int: " << (int)scaled << endl;
        scaled = uint_sat8_t::scale_from(0.25f); // Uses defaults: 0 and 1
        cout << "Converting a 0 to 1 ranged float, 0.25, to a saturating 8-bit uint: " << (int)scaled << endl;
        scaled = int_sat8_t::scale_from(0.25f, -1.5f, 1.5f); // Supply your own, integrals give better performance
        cout << "Converting a -1.5 to 1.5 ranged float, 0.25, to a saturating 8-bit int: " << (int)scaled << endl;
    }
}
