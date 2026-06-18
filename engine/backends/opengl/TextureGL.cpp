#include "TextureGL.hpp"

#include <glad/glad.h>


namespace {
    struct GLTextureFormat {
        GLenum internalFormat = GL_RGBA8;
        GLenum dataFormat = GL_RGBA;
    };

    KIWI_NODISCARD GLTextureFormat ResolveFormat(Kiwi::EImageFormat::Type format) {
        switch (format) {
            case Kiwi::EImageFormat::GRAYSCALE:
                return { GL_R8, GL_RED };
            case Kiwi::EImageFormat::RGB_8:
                return { GL_RGB8, GL_RGB };
            case Kiwi::EImageFormat::RGBA_8:
            default:
                return { GL_RGBA8, GL_RGBA };
        }
    }
}


namespace Kiwi::OpenGL {
    Texture2DGL::Texture2DGL(const ImageDesc& imageDesc)
        : m_width(static_cast<u32>(imageDesc.width)),
          m_height(static_cast<u32>(imageDesc.height))
    {
        const GLTextureFormat glFormat = ResolveFormat(imageDesc.format);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureStorage2D(m_id, 1, glFormat.internalFormat,
            static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));

        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (imageDesc.data != nullptr) {
            // GRAYSCALE/RGB rows may not be 4-byte aligned; relax the unpack alignment.
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTextureSubImage2D(m_id, 0, 0, 0,
                static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height),
                glFormat.dataFormat, GL_UNSIGNED_BYTE, imageDesc.data);
        }
    }

    u32 Texture2DGL::GetWidth() const {
        return m_width;
    }

    u32 Texture2DGL::GetHeight() const {
        return m_height;
    }

    void Texture2DGL::Bind(u32 slot) const {
        glBindTextureUnit(slot, m_id);
    }

    Texture2DGL::~Texture2DGL() {
        glDeleteTextures(1, &m_id);
    }
}
