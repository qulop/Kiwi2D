#pragma once

#include <renderer/Texture.hpp>

#include <backends/opengl/basic/TypesGL.hpp>


namespace Kiwi::OpenGL {
    class Texture2DGL : public ATexture2D {
        KIWI_CREATE_OBJECT(Texture2DGL, ATexture2D)

    public:
        explicit Texture2DGL(const ImageDesc& imageDesc);

        u32 GetWidth() const override;
        u32 GetHeight() const override;

        void Bind(u32 slot = 0) const override;

        KIWI_NODISCARD GlID GetRendererID() const { return m_id; }

        ~Texture2DGL() override;

    private:
        GlID m_id = KIWI_GL_UNDEFINED_ID;
        u32 m_width = 0;
        u32 m_height = 0;
    };

}