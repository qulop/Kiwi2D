#include "IGraphicObjectsFactory.hpp"

#include <driver/opengl/GraphicObjectsFactoryGL.hpp>
#include <driver/vulkan/GraphicObjectsFactoryVK.hpp>



namespace Kiwi {
    std::shared_ptr<IGraphicObjectsFactory> IGraphicObjectsFactory::Create(ERenderAPI::Type api) {
        if (api == ERenderAPI::OpenGL) {
            return std::make_shared<OpenGL::GraphicObjectsFactoryGL>();
        }

        return std::make_shared<Vulkan::GraphicObjectsFactoryVK>();
    }
}
