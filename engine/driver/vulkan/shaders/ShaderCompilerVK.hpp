#pragma once

#include <renderer/shaders/ShaderCompiler.hpp>


namespace Kiwi::Vulkan {
    class ShaderCompilerVK : public AShaderCompiler {
        KIWI_CREATE_OBJECT(ShaderCompilerVK, AShaderCompiler);

    public:
        KIWI_NODISCARD Result<std::shared_ptr<AShader>> CompileFile(const ShaderCompilationRequest& request) override;

        ~ShaderCompilerVK() override = default;
    };
}
