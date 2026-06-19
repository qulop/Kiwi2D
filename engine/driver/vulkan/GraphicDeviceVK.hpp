#pragma once

#include <driver/vulkan/core/VulkanSubsystem.hpp>

#include <renderer/GraphicDevice.hpp>


namespace Kiwi::Vulkan {
    class GraphicObjectsFactoryVK;
    class Device;
    class Surface;


    class GraphicDeviceVK final : public AGraphicDevice {
        KIWI_CREATE_OBJECT(GraphicDeviceVK, AGraphicDevice)

    public:
        KIWI_NODISCARD bool Init() override;

    private:
        friend class GraphicObjectsFactoryVK;

        std::shared_ptr<Device> m_vulkanDevice;

        std::shared_ptr<Surface> m_surface;
    };
}