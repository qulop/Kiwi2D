#include "CameraController.hpp"

#include <core/input/InputSubsystem.hpp>

#include <platform/io/SystemConsole.hpp>


namespace Kiwi {
    void ACameraController::AttachCamera(std::shared_ptr<ACamera> camera) {
        m_camera = camera;
    }


    void FreeCameraController::UpdateInput(f32 deltaTime, const InputState& inputState) {
        const Transform& cameraTransform = m_camera->GetTransform();

        const Vec3 forward = cameraTransform.GetForward();
        const Vec3 right = cameraTransform.GetRight();
        const Vec3 up = cameraTransform.GetUp();

        f32 finalMovementSpeed = m_cameraSpeed * deltaTime;
        Vec3 movement = Vec3::ZeroVector();

        if (inputState.IsKeyPressed(EKeyCode::KEY_W)) {
            movement += forward;
            Console::WriteLine("W");
        }
        if (inputState.IsKeyPressed(EKeyCode::KEY_S)) {
            movement -= forward;
            Console::WriteLine("S");
        }
        if (inputState.IsKeyPressed(EKeyCode::KEY_D)) {
            movement += right;
            Console::WriteLine("D");
        }
        if (inputState.IsKeyPressed(EKeyCode::KEY_A)) {
            movement -= right;
            Console::WriteLine("A");
        }
        if (inputState.IsKeyPressed(EKeyCode::KEY_SPACE)) {
            movement += up;
            Console::WriteLine("SPACE");
        }
        if (inputState.IsKeyPressed(EKeyCode::KEY_LEFT_SHIFT)) {
            movement -= up;
            Console::WriteLine("SHIFT");
        }


    }
}
