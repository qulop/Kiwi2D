#pragma once

#include <core/Object.hpp>
#include <core/Handle.hpp>

#include <renderer/pipeline/RenderAPI.hpp>
#include <renderer/pipeline/RenderPipeline.hpp>



namespace Kiwi {
    struct ImageDesc;


    struct RenderContextInitInfo {
        RenderPipelineInitInfo renderPipelineInitInfo;
    };


    KIWI_INTERFACE IRenderContext : public AObject {
        KIWI_CREATE_OBJECT(IRenderContext, AObject)

    public:
        using PFN_DebugCallback = std::function<void(StringView)>;

    public:
        KIWI_NODISCARD virtual bool Init() = 0;
        KIWI_NODISCARD virtual bool SetupDebugLayerCallback(const PFN_DebugCallback& debugCallback) = 0;

        KIWI_NODISCARD virtual ARenderPipeline* GetPipeline() = 0;
        KIWI_NODISCARD virtual ERenderAPI::Type GetUsedAPI() const = 0;

        KIWI_NODISCARD virtual TextureHandle CreateTexture(const ImageDesc& imageDesc) = 0;
        virtual void DestroyTexture(TextureHandle handle) = 0;

        KIWI_NODISCARD virtual ShaderHandle CreateShader() = 0;
        virtual void DestroyShader(ShaderHandle handle) = 0;

        
        ~IRenderContext() override = default;
    };
}
