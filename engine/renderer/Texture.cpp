#include "Texture.hpp"

#include <backends/opengl/TextureGL.hpp>



namespace Kiwi {
      std::shared_ptr<ATexture2D> ATexture2D::Create(const ImageDesc& desc) {
            return std::make_shared<OpenGL::Texture2DGL>(desc);
      }
}
