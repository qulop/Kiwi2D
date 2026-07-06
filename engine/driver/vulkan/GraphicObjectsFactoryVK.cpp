#include "GraphicObjectsFactoryVK.hpp"

#include <driver/vulkan/RenderContextVK.hpp>
#include <driver/vulkan/GraphicDeviceVK.hpp>



namespace Kiwi::Vulkan {
    std::shared_ptr<IRenderContext> GraphicObjectsFactoryVK::CreateContext() const {
        return std::make_shared<RenderContextVK>();
    }

    std::shared_ptr<AGraphicDevice> GraphicObjectsFactoryVK::CreateGraphicDevice() const {
        return std::make_shared<GraphicDeviceVK>();
    }


    std::shared_ptr<AShaderCompiler> GraphicObjectsFactoryVK::CreateShaderCompiler() const {
        return nullptr;
    }
}
