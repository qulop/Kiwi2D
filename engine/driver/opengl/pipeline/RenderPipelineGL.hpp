#pragma once

#include <renderer/pipeline/RenderPipeline.hpp>


namespace Kiwi::OpenGL {
    class RenderPipelineGL : public ARenderPipeline {
        KIWI_CREATE_OBJECT(RenderPipelineGL, ARenderPipeline)

    public:
        KIWI_NODISCARD bool Init(const RenderPipelineInitInfo& initInfo) override {
            return Super::Init(initInfo);
        }

    private:
        ARenderPipeline* m_pipeline = nullptr;
    };
}