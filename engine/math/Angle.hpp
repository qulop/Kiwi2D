#pragma once

#include <common/Definitions.hpp>

#include <glm/glm.hpp>


namespace Kiwi {
    struct Radians {
    public:
        KIWI_NODISCARD static constexpr Radians FromDegree(f32 angle) {
            return Radians(glm::radians(angle));
        }

    public:
        f32 value = 0.f;

        Radians() = default;
        explicit Radians(f32 rad) :
            value(rad)
        {}

        KIWI_NODISCARD constexpr f32 ToDegrees() const {
            return glm::degrees(value);
        }

        KIWI_NODISCARD constexpr f32 operator*() const noexcept {
            return value;
        }

        KIWI_NODISCARD constexpr operator float() const noexcept {
            return value;
        }
    };
}

namespace std {
    float cos(const Kiwi::Radians& radAndle) {
        return cos(*radAndle);
    }

    float sin(const Kiwi::Radians& radAngle) {
        return sin(*radAngle);
    }
}
