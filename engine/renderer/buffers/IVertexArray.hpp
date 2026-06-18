#pragma once

#include <core/Object.hpp>

#include <common/meta/TypeTraits.hpp>


namespace Kiwi {
    KIWI_INTERFACE IVertexBuffer;
    KIWI_INTERFACE IIndexBuffer;


    KIWI_INTERFACE IVertexArray : public AObject {
        KIWI_CREATE_OBJECT(IVertexArray, AObject)

    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const SharedPtr<IVertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const SharedPtr<IIndexBuffer>& indexBuffer) = 0;

        KIWI_NODISCARD virtual const Vector<SharedPtr<IVertexBuffer>>& GetVertexBuffers() const = 0;
        KIWI_NODISCARD virtual const SharedPtr<IIndexBuffer>& GetIndexBuffer() const = 0;

        ~IVertexArray() override = default;
    };
}
