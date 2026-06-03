#include "ShaderCompilerVK.hpp"


namespace Kiwi::Vulkan {
    std::shared_ptr<AShader> ShaderCompilerVK::CompileFile(const File& sourceFile) {
        const auto spirV = PreprocessAndCompileToSpirV(sourceFile, ESpirVEnvironment::Vulkan, ESpirVOptimizationLevel::PERFORMANCE);
        if (spirV) {

        }

        return nullptr;
    }

}