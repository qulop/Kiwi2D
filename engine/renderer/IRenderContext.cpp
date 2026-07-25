#include "IRenderContext.hpp"

#include <driver/vulkan/RenderContextVK.hpp>
#include <driver/opengl/RenderContextGL.hpp>



namespace Kiwi {
    std::shared_ptr<IRenderContext> IRenderContext::Create(ERenderAPI::Type renderAPI) {
        if (renderAPI == ERenderAPI::OpenGL) {
            return std::make_shared<OpenGL::RenderContextGL>();
        }
        else {
            return std::make_shared<Vulkan::RenderContextVK>();
        }
    }

}
