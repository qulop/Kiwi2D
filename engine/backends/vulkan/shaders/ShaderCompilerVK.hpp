#pragma once

#include <renderer/shaders/ShaderCompiler.hpp>


namespace Kiwi::Vulkan {
    class ShaderCompilerVK : public AShaderCompiler {
        KIWI_CREATE_OBJECT(ShaderCompilerVK, AShaderCompiler);

    public:
        KIWI_NODISCARD std::shared_ptr<AShader> CompileFile(const File& sourceFile) override;
        KIWI_NODISCARD std::shared_ptr<AShader> CompileSource(const String& src) override;

        ~ShaderCompilerVK() override = default;
    };
}
