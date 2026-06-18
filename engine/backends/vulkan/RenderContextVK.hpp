#pragma once

#include <renderer/IRenderContext.hpp>


namespace Kiwi::Vulkan {
    class RenderPipelineVK;


    class RenderContextVK : public IRenderContext {
        KIWI_CREATE_OBJECT(RenderContextVK, IRenderContext)

    public:
        RenderContextVK() = default;

        KIWI_NODISCARD bool Init() override;
        KIWI_NODISCARD bool SetupDebugLayerCallback(const PFN_DebugCallback& debugCallback) override { return true; }


        KIWI_NODISCARD ERenderAPI::Type GetUsedAPI() const override;

        KIWI_NODISCARD ARenderPipeline* GetPipeline() override;

        void SetViewport(i32 x, i32 y, u32 width, u32 height) override {}
        void SetClearColor(const Vec4& color) override {}
        void Clear() override {}
        void DrawIndexed(const SharedPtr<IVertexArray>& vertexArray, u32 indexCount) override {}

        ~RenderContextVK() override = default;

    private:
        RenderPipelineVK* m_pipeline = nullptr;
    };
}