#pragma once

#include <core/Object.hpp>

#include <renderer/pipeline/RenderAPI.hpp>
#include <renderer/pipeline/RenderPipeline.hpp>

#include <math/vec/Vec4.hpp>



namespace Kiwi {
    KIWI_INTERFACE IVertexArray;


    struct RenderContextInitInfo {
        RenderPipelineInitInfo renderPipelineInitInfo;
    };


    KIWI_INTERFACE IRenderContext : public AObject {
        KIWI_CREATE_OBJECT(IRenderContext, AObject)

    public:
        using PFN_DebugCallback = std::function<void(StringView)>;

    public:
        KIWI_NODISCARD static SharedPtr<IRenderContext> Create();

    public:
        KIWI_NODISCARD virtual bool Init() = 0;
        KIWI_NODISCARD virtual bool SetupDebugLayerCallback(const PFN_DebugCallback& debugCallback) = 0;

        KIWI_NODISCARD virtual ARenderPipeline* GetPipeline() = 0;

        KIWI_NODISCARD virtual ERenderAPI::Type GetUsedAPI() const = 0;

        // API-agnostic immediate render commands. Implemented by the active backend.
        virtual void SetViewport(i32 x, i32 y, u32 width, u32 height) = 0;
        virtual void SetClearColor(const Vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawIndexed(const SharedPtr<IVertexArray>& vertexArray, u32 indexCount) = 0;

        ~IRenderContext() override = default;
    };
}
