#pragma once

#include <core/Object.hpp>

#include <renderer/pipeline/RenderAPI.hpp>
#include <renderer/pipeline/RenderPipeline.hpp>



namespace Kiwi {
    struct RenderContextInitInfo {
        RenderPipelineInitInfo renderPipelineInitInfo;
    };


    KIWI_INTERFACE IRenderContext : public AObject {
        KIWI_CREATE_OBJECT(IRenderContext, AObject)

    public:
        using PFN_DebugCallback = std::function<void(StringView)>;

    public:
        KIWI_NODISCARD static std::shared_ptr<IRenderContext> Create();

    public:
        KIWI_NODISCARD virtual bool Init() = 0;
        KIWI_NODISCARD virtual bool SetupDebugLayerCallback(const PFN_DebugCallback& debugCallback) = 0;

        KIWI_NODISCARD virtual ARenderPipeline* GetPipeline() = 0;

        KIWI_NODISCARD virtual ERenderAPI::Type GetUsedAPI() const = 0;

        ~IRenderContext() override = default;
    };
}
