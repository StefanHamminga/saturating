#include <iostream>
#include <iomanip>
#include <cassert>
#include <chrono>
#include <random>
#include <limits>
#include "../functions.hpp"
#include "../types.hpp"

template <typename T, typename A, typename B>
constexpr void test_multiply_impl(const A& a, const B& b) {
    const double temp1 = (saturating::fp_safe_equals(a, (double)0) || saturating::fp_safe_equals(b, (double)0)) ? 0 : static_cast<std::conditional_t<(std::is_integral_v<A> && std::is_integral_v<B>), __int128_t, double>>(a) * b;
    const auto temp2 = temp1 > T::max_val
                            ? T::max_val
                            : (temp1 < T::min_val
                                    ? T::min_val
                                    : temp1);

    // Properly round a floating point 'plain' result if required
    const volatile auto r1 = std::is_floating_point_v<decltype(temp2)> && std::is_integral_v<typename T::value_type>
                                ? (typename T::value_type)std::llround(temp2)
                                : temp2;
    const auto r2 = T::multiply(a, b);

    if (!saturating::fp_safe_equals((float)r1, (float)r2)) {
        std::cout << std::fixed << std::setprecision(12)
                  << "Error calculating " << a
                  << " * " << b
                  << ". Temp1: " << temp1
                  << ", temp2: " << temp2
                  << ". Plain result: "<< r1
                  << ", saturating (" << (int64_t)T::min_val << "..." << (int64_t)T::max_val << "): " << (int)r2 << std::endl;
        assert(r1 == r2); // To get the compiler to abort and show type information
    }
}

template <typename... T, typename A, typename B>
constexpr void test_multiply(const A& a, const B& b) {
    (test_multiply_impl<T, A, B>(a, b), ...);
}


int main() {
    const unsigned samples = 1'000'000;
    // const unsigned limit = 100;
    const unsigned limit = std::numeric_limits<int>::max() / 2;

    std::random_device rd;
    std::uniform_int_distribution<> dis(0, limit);

    auto start = std::chrono::system_clock::now();
    for (unsigned i = 0; i <= samples; ++i) {
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), dis(rd));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd),  static_cast<int>(dis(rd)));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), -static_cast<int>(dis(rd)));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<int>(dis(rd)), dis(rd));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<int>(dis(rd)), static_cast<int>(dis(rd)));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), static_cast<double>(dis(rd)));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<double>(dis(rd)), dis(rd));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), static_cast<double>(dis(rd))/10);
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(static_cast<double>(dis(rd))/10, dis(rd));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(dis(rd), 1/static_cast<double>(dis(rd)));
        test_multiply<int_sat8_t, int_sat16_t, int_sat32_t, int_sat64_t, uint_sat8_t, uint_sat16_t, uint_sat32_t, uint_sat64_t, float_sat_t, double_sat_t>(1/static_cast<double>(dis(rd)), dis(rd));
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Multiplication took " << elapsed.count() << " ms" << std::endl;
}
