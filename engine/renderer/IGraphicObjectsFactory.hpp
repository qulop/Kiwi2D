#pragma once

#include <common/meta/TypeTraits.hpp>


namespace Kiwi {
    class AGraphicDevice;
    class ARenderPipeline;
    class AShaderCompiler;
    class GraphicDevice;

    KIWI_INTERFACE IVertexArray;
    KIWI_INTERFACE IVertexBuffer;
    KIWI_INTERFACE IIndexBuffer;


    KIWI_INTERFACE IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD static SharedPtr<IGraphicObjectsFactory> Create();

    public:
        KIWI_NODISCARD virtual SharedPtr<AGraphicDevice> CreateGraphicDevice() const = 0;
        KIWI_NODISCARD virtual SharedPtr<ARenderPipeline> CreateRenderPipeline() const = 0;
        KIWI_NODISCARD virtual SharedPtr<AShaderCompiler> CreateShaderCompiler() const = 0;

        KIWI_NODISCARD virtual SharedPtr<IVertexArray> CreateVertexArray() const = 0;
        KIWI_NODISCARD virtual SharedPtr<IVertexBuffer> CreateVertexBuffer(u32 size) const = 0;
        KIWI_NODISCARD virtual SharedPtr<IIndexBuffer> CreateIndexBuffer(u32* indices, u32 count) const = 0;


        virtual ~IGraphicObjectsFactory() = default;
    };
}
