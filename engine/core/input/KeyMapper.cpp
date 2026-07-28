#include "KeyMapper.hpp"

#include <glfw/glfw3.h>


namespace Kiwi {
    EKeyCode KeyMapper::MapKeyGLFW(int key) {
        if (key == GLFW_KEY_UNKNOWN) {
            return EKeyCode::UNKNOWN;
        }

        return static_cast<EKeyCode>(key);
    }

    EKeyAction KeyMapper::MapActionGLFW(int action) {
        switch (action) {
        case GLFW_PRESS:
            return EKeyAction::PRESSED;
        case GLFW_RELEASE:
            return EKeyAction::RELEASED;
        case GLFW_REPEAT:
            return EKeyAction::REPEAT;
        default:
            return EKeyAction::ACTION_NONE;
        }
    }
}
