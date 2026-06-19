#include "Texture.hpp"

#include <driver/opengl/TextureGL.hpp>



namespace Kiwi {
      std::shared_ptr<ATexture2D> ATexture2D::Create(const ImageDesc& desc) {
            return std::make_shared<OpenGL::Texture2DGL>(desc);
      }
}
