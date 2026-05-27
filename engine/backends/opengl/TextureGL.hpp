#pragma once

#include <renderer/Texture.hpp>


namespace Kiwi::OpenGL {
    class Texture2DGL : public ATexture2D {
        KIWI_CREATE_OBJECT(Texture2DGL, ATexture2D)

    public:
        explicit Texture2DGL(const ImageDesc& imageDesc);

        u32 GetWidth() const override;
        u32 GetHeight() const override;

        ~Texture2DGL() override = default;
    };

}