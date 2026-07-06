#pragma once

#include <common/types/String.hpp>


namespace Kiwi {
    namespace ERenderAPI {
        enum Type : u16 {
            NONE,
            OpenGL,
            Vulkan,
            DirectX11,
            DirectX12
        };


        KIWI_FORCEINLINE String ToString(Type type) {
            switch (type) {
            case OpenGL:
                return "OpenGL";
            case Vulkan:
                return "Vulkan";
            case DirectX11:
                return "DirectX11";
            case DirectX12:
                return "DirectX12";
            default:
                return "NONE";
            }
        }


        KIWI_FORCEINLINE Type FromString(StringView str) {
            if (str == "OpenGL") {
                return OpenGL;
            }
            if (str == "Vulkan") {
                return Vulkan;
            }
            if (str == "DirectX11") {
                return DirectX11;
            }
            if (str == "DirectX12") {
                return DirectX12;
            }

            return NONE;
        }
    }
}