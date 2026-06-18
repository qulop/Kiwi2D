#pragma once

#include <renderer/IGraphicObjectsFactory.hpp>


namespace Kiwi::Vulkan {
    class GraphicObjectsFactoryVK : public IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD SharedPtr<AGraphicDevice> CreateGraphicDevice() const override;

        KIWI_NODISCARD SharedPtr<ARenderPipeline> CreateRenderPipeline() const override;

        KIWI_NODISCARD SharedPtr<AShaderCompiler> CreateShaderCompiler() const override;

        KIWI_NODISCARD SharedPtr<IVertexArray> CreateVertexArray() const override;

        KIWI_NODISCARD SharedPtr<IVertexBuffer> CreateVertexBuffer(u32 size) const override;

        KIWI_NODISCARD SharedPtr<IIndexBuffer> CreateIndexBuffer(u32* indices, u32 count) const override;

        ~GraphicObjectsFactoryVK() override = default;
    };
}