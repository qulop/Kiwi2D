#pragma once

#include <common/Definitions.hpp>


#define KIWI_INVALID_HANDLE     UINT32_MAX

#define KIWI_HANDLE(HandleName)                         \
    struct HandleName {                                 \
        u32 handle = 0;                                 \
                                                        \
        KIWI_NODISCARD constexpr bool IsValid() const { \
            return handle != KIWI_INVALID_HANDLE;       \
        }                                               \
    };

namespace Kiwi {
    KIWI_HANDLE(IndexBufferHandle)
    KIWI_HANDLE(VertexBufferHandle)
    KIWI_HANDLE(TextureHandle)
    KIWI_HANDLE(ShaderHandle)
}