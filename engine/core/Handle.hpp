#pragma once

#include <common/Definitions.hpp>
#include <common/meta/Concepts.hpp>


#define KIWI_INVALID_HANDLE     UINT32_MAX

#define KIWI_HANDLE(HandleName, UnderlyingHandleType)       \
    struct HandleName : HandleBase {                        \
        using UnderlyingType = UnderlyingHandleType;        \
        using UnderlyingTypePtr = UnderlyingHandleType*;    \
    }


namespace Kiwi {
    class AIndexBuffer;
    class AVertexBuffer;
    class ATexture;
    class AShader;


    struct HandleBase {
        u32 handle = KIWI_INVALID_HANDLE;
        u32 generation = 0;

        KIWI_NODISCARD constexpr bool IsValid() const {
            return handle != KIWI_INVALID_HANDLE;
        }
    };



    template<Concepts::DerivedFrom<HandleBase> THandle>
    class HandlePool {
    public:
        using HandleType = THandle;
        using HandleObjectPtr = THandle::UnderlyingTypePtr;

        struct Slot {
            HandleObjectPtr object = nullptr;
            u32 generation = 1;
        };

    public:
        KIWI_NODISCARD HandleObjectPtr Get(HandleType handle) {
            if (!IsValidHandle(handle)) {
                return nullptr;
            }

            return m_pool.at(handle.handle).object;
        }

        KIWI_NODISCARD bool IsValidHandle(HandleType handle) const {
            return handle.IsValid()
                && handle.handle < m_pool.size()
                && m_pool.at(handle.handle).object != nullptr
                && handle.generation == m_pool.at(handle.handle).generation;
        }

        KIWI_NODISCARD HandleType Add(HandleObjectPtr obj) {
            HandleType newHandle;

            
            if (!m_freeSlots.empty()) {
                newHandle.handle = *m_freeSlots.begin();
                m_freeSlots.erase(newHandle.handle);
            }
            else {

            }

            Slot& slot = m_pool.at(newHandle.handle);
            slot.object = obj;

            newHandle.generation = slot.generation;

            return newHandle;
        }


        KIWI_NODISCARD bool Remove(HandleType handle) {
            if (!IsValidHandle(handle)) {
                return false;
            }

            Slot& slot = m_pool.at(handle.handle);

            delete slot.object;
            slot.object = nullptr;

            ++slot.generation;

            m_freeSlots.insert(handle.handle);
            return true;
        }


    private:
        std::vector<Slot> m_pool;
        std::set<u32> m_freeSlots;
    };

    KIWI_HANDLE(IndexBufferHandle, AIndexBuffer);
    KIWI_HANDLE(VertexBufferHandle, AVertexBuffer);
    KIWI_HANDLE(TextureHandle, ATexture);
    KIWI_HANDLE(ShaderHandle, AShader);
}