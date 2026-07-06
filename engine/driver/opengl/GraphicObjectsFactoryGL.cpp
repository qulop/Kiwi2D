#include "GraphicObjectsFactoryGL.hpp"

#include <driver/opengl/RenderContextGL.hpp>
#include <driver/opengl/shaders/ShaderCompilerGL.hpp>



namespace Kiwi::OpenGL {
    std::shared_ptr<IRenderContext> GraphicObjectsFactoryGL::CreateContext() const {
        return std::make_shared<RenderContextGL>();
    }

    std::shared_ptr<AGraphicDevice> GraphicObjectsFactoryGL::CreateGraphicDevice() const {
        return nullptr;
    }

    std::shared_ptr<AShaderCompiler> GraphicObjectsFactoryGL::CreateShaderCompiler() const {
        return std::make_shared<ShaderCompilerGL>();
    }
}
