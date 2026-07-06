#pragma once

#include <renderer/IGraphicObjectsFactory.hpp>


namespace Kiwi::Vulkan {
    class GraphicObjectsFactoryVK : public IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD std::shared_ptr<IRenderContext> CreateContext() const override;

        KIWI_NODISCARD std::shared_ptr<AGraphicDevice> CreateGraphicDevice() const override;

        KIWI_NODISCARD std::shared_ptr<AShaderCompiler> CreateShaderCompiler() const override;

        ~GraphicObjectsFactoryVK() override = default;
    };
}