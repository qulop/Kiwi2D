#include "Numeric.hpp"

#include <common/Debug.hpp>


namespace {
    template<Kiwi::Concepts::Number T>
    T GetNumberInRange(std::mt19937_64& generator, const T min, const T max) {
        KIWI_ASSERT_BASIC(min <= max);

        if constexpr (Kiwi::Concepts::Integral<T>) {
            std::uniform_int_distribution<T> d(min, max);
            return d(generator);
        }
        else {
            std::uniform_real_distribution<T> d(min, max);
            return d(generator);
        }
    }
}


namespace Kiwi {
    std::random_device Random::s_device;
    std::mt19937_64 Random::s_generator(s_device());


    f32 Random::GetInRangeFloat32(const f32 min, const f32 max) {
        return GetNumberInRange<f32>(s_generator, min, max);
    }

    f64 Random::GetInRangeFloat64(const f64 min, const f64 max) {
        return GetNumberInRange<f64>(s_generator, min, max);
    }

    i32 Random::GetInRangeInt32(const i32 min, const i32 max) {
        return GetNumberInRange<i32>(s_generator, min, max);
    }

    u32 Random::GetInRangeUnsignedInt32(const u32 min, const u32 max) {
        return GetNumberInRange<u32>(s_generator, min, max);
    }

    i64 Random::GetInRangeInt64(const i64 min, const i64 max) {
        return GetNumberInRange<i64>(s_generator, min, max);
    }

    u64 Random::GetInRangeUnsignedInt64(const u64 min, const u64 max) {
        return GetNumberInRange<u64>(s_generator, min, max);
    }

    f32 Random::GetFloat32() {
        return GetInRangeFloat32(std::numeric_limits<f32>::min(), std::numeric_limits<f32>::max());
    }

    f64 Random::GetFloat64() {
        return GetInRangeFloat64(std::numeric_limits<f64>::min(), std::numeric_limits<f64>::max());
    }

    i32 Random::GetInt32() {
        return GetInRangeInt32(std::numeric_limits<i32>::min(), std::numeric_limits<i32>::max());
    }

    u32 Random::GetUnsignedInt32() {
        return GetInRangeUnsignedInt32(std::numeric_limits<u32>::min(), std::numeric_limits<u32>::max());
    }

    i64 Random::GetInt64() {
        return GetInRangeInt64(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max());
    }

    u64 Random::GetUnsignedInt64() {
        return GetInRangeUnsignedInt64(std::numeric_limits<u64>::min(), std::numeric_limits<u64>::max());
    }

    UInt128 Random::GetUnsignedInt128() {
        boost::wide_integer::uniform_int_distribution<128> d;

        return d(s_generator);
    }

    bool Random::GetBool() {
        return GetInRangeUnsignedInt32(0, 1) == 1;
    }
}
