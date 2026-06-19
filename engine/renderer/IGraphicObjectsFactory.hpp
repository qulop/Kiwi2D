#pragma once

#include <common/meta/TypeTraits.hpp>


namespace Kiwi {
    class AGraphicDevice;
    class ARenderPipeline;
    class AShaderCompiler;
    class GraphicDevice;


    KIWI_INTERFACE IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD static std::shared_ptr<IGraphicObjectsFactory> Create();

    public:
        KIWI_NODISCARD virtual std::shared_ptr<AGraphicDevice> CreateGraphicDevice() const = 0;
        KIWI_NODISCARD virtual std::shared_ptr<ARenderPipeline> CreateRenderPipeline() const = 0;
        KIWI_NODISCARD virtual std::shared_ptr<AShaderCompiler> CreateShaderCompiler() const = 0;


        virtual ~IGraphicObjectsFactory() = default;
    };
}
