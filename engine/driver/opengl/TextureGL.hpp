#pragma once

#include <renderer/Texture.hpp>

#include <driver/opengl/core/TypesGL.hpp>


namespace Kiwi::OpenGL {
    class Texture2DGL : public ATexture2D {
        KIWI_CREATE_OBJECT(Texture2DGL, ATexture2D)

    public:
        KIWI_NODISCARD static Texture2DGL* Create(const ImageDesc& desc);

        KIWI_NODISCARD u32 GetWidth() const override;
        KIWI_NODISCARD u32 GetHeight() const override;

        ~Texture2DGL() override = default;

    private:
        GlID m_id = KIWI_GL_UNDEFINED_ID;
        u32 m_width = 0;
        u32 m_height = 0;
    };

}