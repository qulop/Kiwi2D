#include "ShaderCompilerVK.hpp"


namespace Kiwi::Vulkan {
    std::shared_ptr<AShader> ShaderCompilerVK::CompileFile(const File& sourceFile) {
        const auto spirV = PreprocessAndCompileToSpirV(sourceFile, ESpirVEnvironment::Vulkan, ESpirVOptimizationLevel::PERFORMANCE);
        if (spirV) {

        }

        return nullptr;
    }

    std::shared_ptr<AShader> ShaderCompilerVK::CompileSource(const String& src) {
        // The Vulkan backend compiles via the SPIR-V toolchain from files; in-memory
        // GLSL source compilation is not supported here.
        return nullptr;
    }
}