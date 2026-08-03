#pragma once

#include <renderer/Camera.hpp>


namespace Kiwi {
    class InputState;


    class ACameraController {
    public:
        virtual void UpdateInput(f32 deltaTime, const InputState& inputState) = 0;
        virtual void AttachCamera(std::shared_ptr<ACamera> camera);

        virtual ~ACameraController() = default;

    protected:
        std::shared_ptr<ACamera> m_camera;
        f32 m_cameraSpeed = 0.f;
    };



    class FreeCameraController : public ACameraController {
    public:
        void UpdateInput(f32 deltaTime, const InputState& inputState) override;
    };
}