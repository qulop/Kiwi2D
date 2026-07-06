#pragma once

#include <common/meta/TypeTraits.hpp>

#include <renderer/pipeline/RenderAPI.hpp>


namespace Kiwi {
    class AGraphicDevice;
    class ARenderPipeline;
    class AShaderCompiler;
    class GraphicDevice;
    KIWI_INTERFACE IRenderContext;


    KIWI_INTERFACE IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD static std::shared_ptr<IGraphicObjectsFactory> Create(ERenderAPI::Type api);

    public:
        KIWI_NODISCARD virtual std::shared_ptr<IRenderContext> CreateContext() const = 0;

        KIWI_NODISCARD virtual std::shared_ptr<AGraphicDevice> CreateGraphicDevice() const = 0;
        KIWI_NODISCARD virtual std::shared_ptr<AShaderCompiler> CreateShaderCompiler() const = 0;


        virtual ~IGraphicObjectsFactory() = default;
    };
}
