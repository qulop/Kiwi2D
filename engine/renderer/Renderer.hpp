#pragma once

#include <common/meta/TypeTraits.hpp>

#include <core/Object.hpp>

#include <math/Rect.hpp>


namespace Kiwi {
    KIWI_INTERFACE IGraphicObjectsFactory;
    KIWI_INTERFACE IRenderContext;
    class ARenderPipeline;
    class AShaderCompiler;
    class AGraphicDevice;


    class Renderer : public AObject {
        KIWI_CREATE_OBJECT(Renderer, AObject);

    public:
        KIWI_NODISCARD bool Init();

        void SetViewport(const I32Rect& viewport) const;

        ~Renderer() override = default;

    private:
        std::shared_ptr<IGraphicObjectsFactory> m_factory;

        std::shared_ptr<IRenderContext> m_renderContext;
    };
}
