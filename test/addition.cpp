#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <limits>
#include "../functions.hpp"
#include "../types.hpp"

template <typename T, typename A, typename B>
constexpr void test_add_impl(const A& a, const B& b) {
    const auto temp1 = static_cast<double>(a) + b;
    const auto temp2 = (temp1 > T::max_val)
                        ? (T::max_val)
                        : (std::is_unsigned_v<decltype(temp1)>
                            ? temp1
                            : (temp1 < T::min_val
                                ? T::min_val
                                : temp1));

    // Properly round a floating point 'plain' result if required
    const volatile auto r1 = std::is_floating_point_v<decltype(temp2)> && std::is_integral_v<typename T::value_type>
                                ? (typename T::value_type)(std::llround(temp2))
                                : temp2;
    // const auto r2 = T::add(a, b);
    const auto r2 = saturating::add<typename T::value_type>(a, b);

    if (r1 != r2) {
        // This actually exposes a lot of pitfalls in the *standard* arithemetics expectations, interesting!
        std::cout << std::fixed
                  << "Error calculating "
                  << a
                  << " + "
                  << b
                  << ". Temp1: "
                  << temp1
                  << ", temp2: "
                  << temp2
                  << ". Plain result: "
                  << r1
                  << ", saturating ("
                  << T::min_val
                  << "..."
                  << T::max_val
                  << "): "
                  << r2
                  << std::endl;
        assert(r1 == r2); // To get the compiler to abort and show type information
    }
}

template <typename... T, typename A, typename B>
constexpr void test_add(const A& a, const B& b) {
    (test_add_impl<T, A, B>(a, b), ...);
}

int main() {
    const unsigned samples = 1'000'000;
    // const unsigned limit = 100;
    const unsigned limit = std::numeric_limits<int>::max() / 2;

    std::random_device rd;
    std::uniform_int_distribution<> dis(0, limit);

    auto start = std::chrono::system_clock::now();
    for (unsigned i = 0; i <= samples; ++i) {
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), dis(rd));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd),  static_cast<int>(dis(rd)));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), -static_cast<int>(dis(rd)));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<int>(dis(rd)), dis(rd));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<int>(dis(rd)), static_cast<int>(dis(rd)));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), static_cast<double>(dis(rd)));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<double>(dis(rd)), dis(rd));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), static_cast<double>(dis(rd))/10);
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<double>(dis(rd))/10, dis(rd));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), 1/static_cast<double>(dis(rd)));
        test_add<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(1/static_cast<double>(dis(rd)), dis(rd));
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Addition took " << elapsed.count() << " ms" << std::endl;
}
