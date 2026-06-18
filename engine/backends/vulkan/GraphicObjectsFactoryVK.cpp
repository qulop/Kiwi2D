#include "GraphicObjectsFactoryVK.hpp"

#include <backends/vulkan/RenderContextVK.hpp>
#include <backends/vulkan/GraphicDeviceVK.hpp>



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

    SharedPtr<IVertexArray> GraphicObjectsFactoryVK::CreateVertexArray() const {
        return nullptr;
    }

    SharedPtr<IVertexBuffer> GraphicObjectsFactoryVK::CreateVertexBuffer(u32 size) const {
        return nullptr;
    }

    SharedPtr<IIndexBuffer> GraphicObjectsFactoryVK::CreateIndexBuffer(u32* indices, u32 count) const {
        return nullptr;
    }
}
