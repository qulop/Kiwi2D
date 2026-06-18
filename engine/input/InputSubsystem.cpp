#include "InputSubsystem.hpp"


namespace {
    // Mirrors GLFW action codes (GLFW_RELEASE / GLFW_PRESS / GLFW_REPEAT)
    constexpr int ACTION_RELEASE = 0;
    constexpr int ACTION_PRESS = 1;
    constexpr int ACTION_REPEAT = 2;
}


namespace Kiwi {
    bool InputSubsystem::Init() {
        m_keyStates.clear();
        m_mouseStates.clear();
        m_mousePosition = Vec2{ 0.0f, 0.0f };

        return true;
    }

    void InputSubsystem::DeInit() {
        m_keyCallbacks.clear();
        m_charCallbacks.clear();
        m_mouseButtonCallbacks.clear();

        m_keyStates.clear();
        m_mouseStates.clear();
    }

    bool InputSubsystem::IsKeyPressed(int keyCode) const {
        const auto it = m_keyStates.find(keyCode);
        return it != m_keyStates.end() && it->second;
    }

    bool InputSubsystem::IsMouseButtonPressed(int buttonCode) const {
        const auto it = m_mouseStates.find(buttonCode);
        return it != m_mouseStates.end() && it->second;
    }

    Vec2 InputSubsystem::GetMousePosition() const {
        return m_mousePosition;
    }

    void InputSubsystem::UpdateKeyState(int keyCode, int action) {
        m_keyStates[keyCode] = (action != ACTION_RELEASE);

        const KeyEvent event{ keyCode, TranslateAction(action) };
        for (const KeyCallback& callback : m_keyCallbacks) {
            callback(event);
        }
    }

    void InputSubsystem::UpdateMouseState(int buttonCode, int action) {
        m_mouseStates[buttonCode] = (action != ACTION_RELEASE);

        const MouseButtonEvent event{ buttonCode, TranslateAction(action) };
        for (const MouseButtonCallback& callback : m_mouseButtonCallbacks) {
            callback(event);
        }
    }

    void InputSubsystem::UpdateMousePosition(const Vec2& pos) {
        m_mousePosition = pos;
    }

    void InputSubsystem::UpdateCharInput(unsigned int codepoint) {
        const CharEvent event{ codepoint };
        for (const CharCallback& callback : m_charCallbacks) {
            callback(event);
        }
    }

    void InputSubsystem::SubscribeKeyEvent(KeyCallback callback) {
        m_keyCallbacks.push_back(std::move(callback));
    }

    void InputSubsystem::SubscribeCharEvent(CharCallback callback) {
        m_charCallbacks.push_back(std::move(callback));
    }

    void InputSubsystem::SubscribeMouseButtonEvent(MouseButtonCallback callback) {
        m_mouseButtonCallbacks.push_back(std::move(callback));
    }

    EInputAction InputSubsystem::TranslateAction(int action) {
        switch (action) {
            case ACTION_PRESS:   return EInputAction::Press;
            case ACTION_REPEAT:  return EInputAction::Repeat;
            case ACTION_RELEASE:
            default:             return EInputAction::Release;
        }
    }
}
