#pragma once

#include <core/Object.hpp>

#include <math/vec/Vec2.hpp>

#include <input/InputEvents.hpp>

#include <unordered_map>
#include <functional>
#include <vector>


namespace Kiwi {
    class InputSubsystem : public ASubsystem {
        KIWI_CREATE_OBJECT(InputSubsystem, ASubsystem);

    public:
        bool Init() override;
        void DeInit() override;

        KIWI_NODISCARD bool IsKeyPressed(int keyCode) const;
        KIWI_NODISCARD bool IsMouseButtonPressed(int buttonCode) const;
        KIWI_NODISCARD Vec2 GetMousePosition() const;

        void UpdateKeyState(int keyCode, int action);
        void UpdateMouseState(int buttonCode, int action);
        void UpdateMousePosition(const Vec2& pos);
        void UpdateCharInput(unsigned int codepoint);

        using KeyCallback = std::function<void(const KeyEvent&)>;
        using CharCallback = std::function<void(const CharEvent&)>;
        using MouseButtonCallback = std::function<void(const MouseButtonEvent&)>;

        void SubscribeKeyEvent(KeyCallback callback);
        void SubscribeCharEvent(CharCallback callback);
        void SubscribeMouseButtonEvent(MouseButtonCallback callback);

        ~InputSubsystem() override = default;

    private:
        KIWI_NODISCARD static EInputAction TranslateAction(int action);

    private:
        std::unordered_map<int, bool> m_keyStates;
        std::unordered_map<int, bool> m_mouseStates;
        Vec2 m_mousePosition{ 0.0f, 0.0f };

        std::vector<KeyCallback> m_keyCallbacks;
        std::vector<CharCallback> m_charCallbacks;
        std::vector<MouseButtonCallback> m_mouseButtonCallbacks;
    };
}
