#include "IGraphicObjectsFactory.hpp"

#include <driver/opengl/GraphicObjectsFactoryGL.hpp>
#include <driver/vulkan/GraphicObjectsFactoryVK.hpp>



namespace Kiwi {
    std::shared_ptr<IGraphicObjectsFactory> IGraphicObjectsFactory::Create() {
        return std::make_shared<Vulkan::GraphicObjectsFactoryVK>();
    }
}
