#pragma once

#include <renderer/IGraphicObjectsFactory.hpp>


namespace Kiwi::OpenGL {
    class GraphicObjectsFactoryGL : public IGraphicObjectsFactory {
    public:
        KIWI_NODISCARD std::shared_ptr<IRenderContext> CreateContext() const override;
        
        KIWI_NODISCARD std::shared_ptr<AGraphicDevice> CreateGraphicDevice() const override;
        KIWI_NODISCARD std::shared_ptr<AShaderCompiler> CreateShaderCompiler() const override;

        ~GraphicObjectsFactoryGL() override = default;
    };
}