#include "UUID.hpp"

#include <common/types/String.hpp>
#include <common/Debug.hpp>

#include <algorithm>

namespace {
    constexpr size_t UUID_STRING_LENGTH_WITH_DASHES = 36;
    constexpr std::array<size_t, 4> UUID_DASH_POSITIONS = { 8, 13, 18, 23 };
}


namespace Kiwi {
    UUID UUID::Generate() {
        const UInt128 r = Random::GetUnsignedInt128();

        return UUID(r);
    }

    Opt<UUID> UUID::FromString(const String& s) {
        if (s.Size() != UUID_STRING_LENGTH_WITH_DASHES) KIWI_UNLIKELY {
            return nullopt;
        }

        if (!std::ranges::all_of(UUID_DASH_POSITIONS, [&s](const size_t& idx) { return s.CharAt(idx) == '-'; })) {
            return nullopt;
        }

        const std::vector<String> uuidFragments = s.Split("-");
        u64 low = 0;
        u64 high = 0;

        // TODO: Add support for both endian's
        high |= (
            (ParseHexFragment(uuidFragments[0]) << 32) |
            (ParseHexFragment(uuidFragments[1]) << 16) |
            (ParseHexFragment(uuidFragments[2]))
        );

        low |= (
            (ParseHexFragment(uuidFragments[3]) << 48) |
            (ParseHexFragment(uuidFragments[4]))
        );

        return UUID(UInt128(high) << 64 | UInt128(low));
    }

    bool UUID::IsEmpty() const noexcept {
        return m_uuid == 0;
    }

    UInt128 UUID::ToUInt128() const noexcept {
        return m_uuid;
    }

    String UUID::ToString() const {
        static constexpr char HEX_CHARS[] = "0123456789abcdef";

        const u64 high = static_cast<u64>(m_uuid >> 64);
        const u64 low = static_cast<u64>(m_uuid);

        char buffer[UUID_STRING_LENGTH_WITH_DASHES + 1];
        buffer[UUID_STRING_LENGTH_WITH_DASHES] = '\0';

        for (const size_t&  idx : UUID_DASH_POSITIONS) {
            buffer[idx] = '-';
        }

        size_t buf_idx = 0;
        for (int i = 60; i >= 0; i -= 4) {
            if (buf_idx == 8 || buf_idx == 13) {
                buf_idx++;
            }

            buffer[buf_idx++] = HEX_CHARS[(high >> i) & 0xF];
        }

        for (int i = 60; i >= 0; i -= 4) {
            if (buf_idx == 18 || buf_idx == 23) {
                buf_idx++;
            }
            buffer[buf_idx++] = HEX_CHARS[(low >> i) & 0xF];
        }

        return String(buffer);
    }

    std::array<u8, 16> UUID::ToBytes() const noexcept {
        std::array<u8, 16> bytes;

        const u64 high = static_cast<u64>(m_uuid >> 64);
        const u64 low = static_cast<u64>(m_uuid);

        for (size_t i = 0; i < 8; ++i) {
            const size_t shift = 56 - (i * 8);
            bytes[i]     = static_cast<u8>(high >> shift);
            bytes[i + 8] = static_cast<u8>(low  >> shift);
        }

        return bytes;
    }

    bool UUID::operator==(const UUID& other) const noexcept {
        return m_uuid == other.m_uuid;
    }

    bool UUID::operator==(UInt128 u) const noexcept {
        return m_uuid == u;
    }

    bool UUID::operator!=(const UUID& other) const noexcept {
        return m_uuid != other.m_uuid;
    }

    bool UUID::operator!=(UInt128 u) const noexcept {
        return m_uuid != u;
    }

    bool UUID::operator<(const UUID& other) const noexcept {
        return m_uuid < other.m_uuid;
    }

    bool UUID::operator<=(const UUID& other) const noexcept {
        return m_uuid <= other.m_uuid;
    }

    bool UUID::operator>(const UUID& other) const noexcept {
        return m_uuid > other.m_uuid;
    }

    bool UUID::operator>=(const UUID& other) const noexcept {
        return m_uuid >= other.m_uuid;
    }


    u64 UUID::ParseHexFragment(const String& fragment) {
        u64 result = 0;
        for (const char c : fragment) {
            if (const i8 nibble = Numeric::HexCharToNibble(c); nibble != -1) {
                result = (result << 4) | nibble;
            }
            else {
                return 0;
            }
        }

        return result;
    }
}
