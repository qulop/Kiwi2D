#pragma once

#include <core/Object.hpp>
#include <core/input/Keys.hpp>

#include <math/Vec2.hpp>


namespace Kiwi {
    class AWindow;
    class InputSubsystem;


    struct KeyState {
        EKeyAction action = EKeyAction::ACTION_NONE;
        bool changedThisTick = false;
    };


    class InputState {
    public:
        KIWI_NODISCARD bool IsKeyPressed(EKeyCode keyCode) const;
        KIWI_NODISCARD KeyState GetKeyState(EKeyCode keyCode) const;

        KIWI_NODISCARD Vec2 GetMousePosition() const;

    private:
        KIWI_NODISCARD static bool IsPressed(KeyState state);

    private:
        friend class InputSubsystem;

        std::map<EKeyCode, KeyState> m_keyStates;
        Vec2 m_mousePosition;
    };



    class InputSubsystem : public ASubsystem {
        KIWI_CREATE_OBJECT(InputSubsystem, ASubsystem)

    public:
        using PFN_GamepadConnectedCallback = std::function<void()>;

    public:
        KIWI_NODISCARD bool Init() override;
        void BeginTick();

        KIWI_NODISCARD const InputState& GetState() const;

        void AddGamepadConnectedCallback(const PFN_GamepadConnectedCallback& callback);

        void UpdateKeyState(EKeyCode keyCode, EKeyAction keyAction);
        void UpdateMousePosition(const Vec2& newPosition);

        void OnWindowFocusChanged(bool isFocused);

    private:
        InputState m_inputState;

        std::vector<PFN_GamepadConnectedCallback> m_gamepadConnectedCallbacks;
    };
}