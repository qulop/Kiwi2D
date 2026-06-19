#include "IRenderContext.hpp"

#include <driver/vulkan/RenderContextVK.hpp>


namespace Kiwi {
    std::shared_ptr<IRenderContext> IRenderContext::Create() {
        return std::make_shared<Vulkan::RenderContextVK>();
    }
}
