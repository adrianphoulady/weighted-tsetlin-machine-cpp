//
//  Created by Adrian Phoulady on 8/29/19.
//  © 2019 Adrian Phoulady
//

#include <cstdint>
#include <cmath>
#include "array.h"

// omitting inline for not requiring C++17
/*inline*/ static uint64_t mcg_state = 0xcafef00dd15ea5e5u;
/*inline*/ static uint32_t constexpr fastrand_max = UINT32_MAX;
/*inline*/ static double constexpr fastrand_max_inverse = 1. / fastrand_max;
/*inline*/ static double constexpr two_pi = 2 * 3.14159265358979323846;

// fast pcg32
// https://en.wikipedia.org/wiki/Permuted_congruential_generator
inline static uint32_t fastrand() {
    auto x = mcg_state;
    mcg_state *= 6364136223846793005u;
    return (x ^ x >> 22) >> (22 + (x >> 61)); // 22 = 32 - 3 - 7, 61 = 64 - 3
}

// random from 0. to 1.
inline static double fastrandom() {
    return fastrand_max_inverse * fastrand();
}

// a biased one suffices here
inline static uint32_t fastrandrange(uint32_t n) {
    return (uint64_t) n * fastrand() >> 32;
}

// fastrand's seed
inline static void fastsrand(uint64_t seed) {
    mcg_state = 2 * seed + 1;
    fastrand();
}

// Box–Muller transform
inline static double normal(double mean, double variance) {
    // it won't be an issue if the first random number is .0
    return mean + sqrt(-2 * log(fastrandom()) * variance) * sin(two_pi * fastrandom());
}

// approximating binomial with normal
inline static int binomial(double p, int n) {
    double b = normal(n * p, n * p * (1 - p)) + .5; // "+ .5" for rounding to the nearest integer
    return b <= 0? 0: b >= n? n: (int) b;
}

// Fisher-Yates random shuffle
inline static void shuffle(array1d<int> &a) {
    for (int i = a.columns; i; --i)
        std::swap(a(i - 1), a(fastrandrange(i)));
}
