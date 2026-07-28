#pragma once

#include <core/input/Keys.hpp>

#include <common/Definitions.hpp>


namespace Kiwi {
    class KeyMapper {
    public:
        KIWI_NODISCARD static EKeyCode MapKeyGLFW(int key);
        KIWI_NODISCARD static EKeyAction MapActionGLFW(int action);
    };
}
