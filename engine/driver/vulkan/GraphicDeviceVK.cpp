#include "GraphicDeviceVK.hpp"

#include <driver/vulkan/RenderContextVK.hpp>
#include <driver/vulkan/core/Device.hpp>


namespace Kiwi::Vulkan {
    bool GraphicDeviceVK::Init() {
        if (!Super::Init()) {
            return false;
        }

        return true;
    }
}
