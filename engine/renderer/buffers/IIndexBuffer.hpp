#pragma once

#include <core/Object.hpp>


namespace Kiwi {
    KIWI_INTERFACE IIndexBuffer : public AObject {
        KIWI_CREATE_OBJECT(IIndexBuffer, AObject)

    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        KIWI_NODISCARD virtual u32 GetCount() const = 0;

        ~IIndexBuffer() override = default;
    };
}
