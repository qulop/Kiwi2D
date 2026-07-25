#pragma once

#include <common/meta/TypeTraits.hpp>

#include <core/Object.hpp>

#include <renderer/pipeline/RenderAPI.hpp>

#include <math/Rect.hpp>


namespace Kiwi {
    KIWI_INTERFACE IRenderContext;
    class ARenderPipeline;
    class AShaderCompiler;
    class AGraphicDevice;


    class Renderer : public AObject {
        KIWI_CREATE_OBJECT(Renderer, AObject);

    public:
        KIWI_NODISCARD bool Init(ERenderAPI::Type renderAPI);

        void SetViewport(const I32Rect& viewport) const;

        void OnFramebufferResized(U32Rect newSize);

        KIWI_NODISCARD bool BeginScene();
        void Render();
        void EndScene();

        ~Renderer() override = default;

    private:
        std::shared_ptr<IRenderContext> m_renderContext;
    };
}
