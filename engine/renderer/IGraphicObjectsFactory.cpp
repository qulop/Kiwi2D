#include "IGraphicObjectsFactory.hpp"

#include <driver/opengl/GraphicObjectsFactoryGL.hpp>
#include <driver/vulkan/GraphicObjectsFactoryVK.hpp>



namespace Kiwi {
    SharedPtr<IGraphicObjectsFactory> IGraphicObjectsFactory::Create() {
        return MakeShared<Vulkan::GraphicObjectsFactoryVK>();
    }
}
