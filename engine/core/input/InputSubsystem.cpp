#include "InputSubsystem.hpp"



namespace Kiwi {
    bool InputState::IsKeyPressed(EKeyCode keyCode) const {
        return IsPressed(GetKeyState(keyCode));
    }

    KeyState InputState::GetKeyState(EKeyCode keyCode) const {
        if (m_keyStates.contains(keyCode)) {
            return m_keyStates.at(keyCode);
        }
        return {};
    }

    Vec2 InputState::GetMousePosition() const {
        return m_mousePosition;
    }

    bool InputState::IsPressed(KeyState state) {
        return state.action == EKeyAction::PRESSED;
    }



    bool InputSubsystem::Init() {
        if (!Super::Init()) {
            return false;
        }

        return true;
    }

    void InputSubsystem::BeginTick() {
        for (KeyState& state : std::views::values(m_inputState.m_keyStates)) {
            state.changedThisTick = false;

            // if (state.action == EKeyAction::PRESSED || state.action == EKeyAction::REPEAT) {
            //     state.action = EKeyAction::HOLD;
            // }
            // else if (state.action == EKeyAction::RELEASED) {
            //     state.action = EKeyAction::ACTION_NONE;
            // }
        }
    }

    const InputState& InputSubsystem::GetState() const {
        return m_inputState;
    }

    void InputSubsystem::AddGamepadConnectedCallback(const PFN_GamepadConnectedCallback& callback) {
        m_gamepadConnectedCallbacks.push_back(callback);
    }

    void InputSubsystem::UpdateKeyState(EKeyCode keyCode, EKeyAction keyAction) {
        m_inputState.m_keyStates[keyCode] = KeyState{
            .action = keyAction,
            .changedThisTick = true
        };
    }

    void InputSubsystem::UpdateMousePosition(const Vec2& newPosition) {
        m_inputState.m_mousePosition = newPosition;
    }

    void InputSubsystem::OnWindowFocusChanged(bool isFocused) {
        if (!isFocused) {
            for (KeyState& state : std::views::values(m_inputState.m_keyStates)) {
                state.action = EKeyAction::RELEASED;
            }
        }
    }
}
