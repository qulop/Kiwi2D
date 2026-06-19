#include "GraphicObjectsFactoryVK.hpp"

#include <driver/vulkan/RenderContextVK.hpp>
#include <driver/vulkan/GraphicDeviceVK.hpp>



namespace Kiwi::Vulkan {
    SharedPtr<AGraphicDevice> GraphicObjectsFactoryVK::CreateGraphicDevice() const {
        return MakeShared<GraphicDeviceVK>();
    }

    SharedPtr<ARenderPipeline> GraphicObjectsFactoryVK::CreateRenderPipeline() const {
        return nullptr;
    }

    SharedPtr<AShaderCompiler> GraphicObjectsFactoryVK::CreateShaderCompiler() const {
        return nullptr;
    }
}
