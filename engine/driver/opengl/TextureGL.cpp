#include "TextureGL.hpp"


namespace Kiwi::OpenGL {
    Texture2DGL* Texture2DGL::Create(const ImageDesc& desc) {
        Texture2DGL* newTexture = new Texture2DGL();

        GlID& id = newTexture->m_id;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, )

        // glTexImage2D(GL_TEXTURE_2D, 0, )

        glBindTexture(GL_TEXTURE_2D, 0);

        newTexture->m_width = desc.width;
        newTexture->m_height = desc.height;

        return newTexture;
    }

    u32 Texture2DGL::GetWidth() const {
        return m_width;
    }

    u32 Texture2DGL::GetHeight() const {
        return m_height;
    }
}
