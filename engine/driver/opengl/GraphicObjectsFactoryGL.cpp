#include "GraphicObjectsFactoryGL.hpp"

#include <driver/opengl/RenderContextGL.hpp>
#include <driver/opengl/shaders/ShaderCompilerGL.hpp>



namespace Kiwi::OpenGL {
    std::shared_ptr<AShaderCompiler> GraphicObjectsFactoryGL::CreateShaderCompiler() const {
        return std::make_shared<ShaderCompilerGL>();
    }
}
