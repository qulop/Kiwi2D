#pragma once

#include <common/Definitions.hpp>
#include <common/meta/TypeTraits.hpp>
#include <common/types/Opt.hpp>
#include <common/Numeric.hpp>



namespace Kiwi {
    class String;
}


namespace Kiwi {
    class UUID {
    public:
        KIWI_NODISCARD static UUID Generate();

        KIWI_NODISCARD static Opt<UUID> FromString(const String& s);

    public:
        constexpr UUID() = default;
        explicit constexpr UUID(const UInt128 v) :
            m_uuid(v)
        {}

        UUID(const UUID& other) = default;
        UUID(UUID&& other) noexcept = default;

        KIWI_NODISCARD bool IsEmpty() const noexcept;

        KIWI_NODISCARD UInt128 ToUInt128() const noexcept;

        KIWI_NODISCARD String ToString() const;

        KIWI_NODISCARD std::array<u8, 16> ToBytes() const noexcept;

        KIWI_NODISCARD UUID& operator=(const UUID& other) = default;
        KIWI_NODISCARD UUID& operator=(UUID&& other) noexcept = default;

        KIWI_NODISCARD bool operator==(const UUID& other) const noexcept;
        KIWI_NODISCARD bool operator==(UInt128 u) const noexcept;

        KIWI_NODISCARD bool operator!=(const UUID& other) const noexcept;
        KIWI_NODISCARD bool operator!=(UInt128 u) const noexcept;

        KIWI_NODISCARD bool operator<(const UUID& other) const noexcept;
        KIWI_NODISCARD bool operator<=(const UUID& other) const noexcept;

        KIWI_NODISCARD bool operator>(const UUID& other) const noexcept;
        KIWI_NODISCARD bool operator>=(const UUID& other) const noexcept;

    private:
        static u64 ParseHexFragment(const String& fragment);

    private:
        UInt128 m_uuid;
    };
}


namespace std {
    template<>
    struct hash<Kiwi::UUID> {
        std::size_t operator()(const Kiwi::UUID& uuid) const noexcept {
            const Kiwi::UInt128 uuidValue = uuid.ToUInt128();

            const Kiwi::u64 high = static_cast<Kiwi::u64>(uuidValue >> 64);
            const Kiwi::u64 low  = static_cast<Kiwi::u64>(uuidValue);

            const size_t hash1 = std::hash<Kiwi::u64>{}(high);
            const size_t hash2 = std::hash<Kiwi::u64>{}(low);

            // Golden Ratio magic number
            return hash1 ^ (hash2 + 0x9e3779b97f4a7c15ULL + (hash1 << 6) + (hash1 >> 2));
        }
    };
}