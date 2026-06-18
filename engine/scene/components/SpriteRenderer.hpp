#pragma once

#include <scene/Component.hpp>

#include <math/vec/Vec4.hpp>

#include <renderer/Texture.hpp>

#include <memory>


namespace Kiwi {
    class SpriteRenderer : public Component {
        KIWI_CREATE_OBJECT(SpriteRenderer, Component);

    public:
        std::shared_ptr<ATexture2D> texture;
        Vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA

        ~SpriteRenderer() override = default;
    };
}
