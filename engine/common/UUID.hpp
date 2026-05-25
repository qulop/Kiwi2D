#pragma once

#include <common/Definitions.hpp>
#include <common/meta/TypeTraits.hpp>
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
            return std::hash<size_t>{}(0);
        }
    };
}