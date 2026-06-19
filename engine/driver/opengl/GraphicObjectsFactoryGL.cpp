#include "GraphicObjectsFactoryGL.hpp"

#include <driver/opengl/RenderContextGL.hpp>
#include <driver/opengl/shaders/ShaderCompilerGL.hpp>



namespace Kiwi::OpenGL {
    SharedPtr<AShaderCompiler> GraphicObjectsFactoryGL::CreateShaderCompiler() const {
        return MakeShared<ShaderCompilerGL>();
    }
}
