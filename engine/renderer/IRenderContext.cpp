#include "IRenderContext.hpp"

#include <driver/vulkan/RenderContextVK.hpp>


namespace Kiwi {
    SharedPtr<IRenderContext> IRenderContext::Create() {
        return MakeShared<Vulkan::RenderContextVK>();
    }
}
