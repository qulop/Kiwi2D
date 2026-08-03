#pragma once

#include <core/Object.hpp>

#include <math/Vec3.hpp>
#include <math/Mat4.hpp>
#include <math/Transform.hpp>


namespace Kiwi {
    class InputState;


    class ACamera : public AObject {
        KIWI_CREATE_OBJECT(ACamera, AObject)

    public:
        KIWI_NODISCARD virtual const Transform& GetTransform() const;

        ~ACamera() override = default;

    private:
        Transform m_transform;
    };
}