#include "ShaderCompilerVK.hpp"


namespace Kiwi::Vulkan {
    Result<std::shared_ptr<AShader>> ShaderCompilerVK::CompileFile(const ShaderCompilationRequest& request) {
        return Error::Create(EGeneralError::UNKNOWN);
    }

}