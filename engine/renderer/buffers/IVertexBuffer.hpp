#pragma once

#include <core/Object.hpp>

#include <renderer/buffers/BufferLayout.hpp>


namespace Kiwi {
    KIWI_INTERFACE IVertexBuffer : public AObject {
        KIWI_CREATE_OBJECT(IVertexBuffer, AObject)

    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, u32 size) = 0;

        KIWI_NODISCARD virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;

        ~IVertexBuffer() override = default;
    };
}
