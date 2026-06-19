#pragma once

#include <renderer/IGraphicObjectsFactory.hpp>


namespace Kiwi::OpenGL {
    class GraphicObjectsFactoryGL : public IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD std::shared_ptr<ARenderPipeline> CreateRenderPipeline() const override;

        KIWI_NODISCARD std::shared_ptr<AShaderCompiler> CreateShaderCompiler() const override;

        ~GraphicObjectsFactoryGL() override = default;
    };
}