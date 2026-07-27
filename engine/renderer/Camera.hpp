#pragma once

#include <core/Object.hpp>

#include <math/Vec3.hpp>
#include <math/Mat4.hpp>
#include <math/Transform.hpp>


namespace Kiwi {
    class ACamera : public AObject {
        KIWI_CREATE_OBJECT(ACamera, AObject)

    public:
        


    private:
        Transform m_transform;
        Vec3 m_cameraTarget;
    };
}