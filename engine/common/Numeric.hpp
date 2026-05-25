#pragma once

#include <common/PCH.hpp>
#include <common/meta/Concepts.hpp>
#include <common/types/UnsignedWideInteger.hpp>



namespace Kiwi::Numeric {
    template<typename Tx>
    constexpr f64 Pow2(Tx&& val) {
        return std::pow(val, 2);
    }

    template<typename Tx>
    constexpr f64 Pow3(Tx&& val) {
        return std::pow(val, 3);
    }

    template<typename Head, typename... Tail>
        requires std::conjunction_v<std::is_same<Head, Tail...>> && std::is_arithmetic_v<Head>
    constexpr auto AccumulateVariadic(Head&& head, Tail&&... tail) {
        if constexpr (sizeof...(Tail) > 0)
            return head + accumulateVariadic(tail...);
        else
            return head;
    }

    constexpr i8 HexCharToNibble(const char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f') {
            return (c - 'a') + 10;
        }
        if (c >= 'A' && c <= 'F') {
            return (c - 'A') + 10;
        }

        return -1;
    }
}

namespace Kiwi {
    using UInt128 = boost::wide_integer::uint128_t;


    class Random {
    public:
        KIWI_NODISCARD static f32 GetInRangeFloat32(f32 min, f32 max);
        KIWI_NODISCARD static f64 GetInRangeFloat64(f64 min, f64 max);

        KIWI_NODISCARD static i32 GetInRangeInt32(i32 min, i32 max);
        KIWI_NODISCARD static u32 GetInRangeUnsignedInt32(u32 min, u32 max);
        KIWI_NODISCARD static i64 GetInRangeInt64(i64 min, i64 max);
        KIWI_NODISCARD static u64 GetInRangeUnsignedInt64(u64 min, u64 max);


        KIWI_NODISCARD static f32 GetFloat32();
        KIWI_NODISCARD static f64 GetFloat64();

        KIWI_NODISCARD static i32 GetInt32();
        KIWI_NODISCARD static u32 GetUnsignedInt32();
        KIWI_NODISCARD static i64 GetInt64();
        KIWI_NODISCARD static u64 GetUnsignedInt64();

        KIWI_NODISCARD static UInt128 GetUnsignedInt128();

        KIWI_NODISCARD static bool GetBool();

    private:
        static std::random_device s_device;
        static std::mt19937_64 s_generator;
    };
}