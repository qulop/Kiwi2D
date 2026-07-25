#pragma once

#include <renderer/IRenderContext.hpp>


namespace Kiwi::Vulkan {
    class RenderPipelineVK;


    class RenderContextVK : public IRenderContext {
        KIWI_CREATE_OBJECT(RenderContextVK, IRenderContext)

    public:
        RenderContextVK() = default;

        KIWI_NODISCARD bool Init() override;
        KIWI_NODISCARD bool SetupDebugCallback(const PFN_DebugCallback& debugCallback) override;

        void SetClearColor(const Color& color) override;

        KIWI_NODISCARD ARenderPipeline* GetPipeline() override;
        KIWI_NODISCARD ERenderAPI::Type GetUsedAPI() const override;

        KIWI_NODISCARD TextureHandle CreateTexture(const ImageDesc& imageDesc)override;
        KIWI_NODISCARD bool DestroyTexture(TextureHandle handle)override;

        KIWI_NODISCARD ShaderHandle CreateShader()override;
        KIWI_NODISCARD bool DestroyShader(ShaderHandle handle)override;

        ~RenderContextVK() override = default;

    private:
        RenderPipelineVK* m_pipeline = nullptr;
    };
}