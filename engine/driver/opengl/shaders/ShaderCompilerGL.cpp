#include "ShaderCompilerGL.hpp"

#include <renderer/shaders/SpirV.hpp>

#include "ShaderGL.hpp"


namespace {
    void ReleaseShaderResources(Kiwi::OpenGL::GlID programId, const std::map<Kiwi::EShaderStage, Kiwi::OpenGL::ShaderModuleGL>& modules) {
        for (const auto& module : std::views::values(modules)) {
            glDeleteShader(module.moduleID);
        }
        glDeleteProgram(programId);
    }
}


namespace Kiwi::OpenGL {
    Result<std::shared_ptr<AShader>> ShaderCompilerGL::CompileFile(const ShaderCompilationRequest& request) {
        PreprocessorGLSL preprocessor;
        Result<PreprocessorGLSL::SourcesMap> preprocessedSrc = preprocessor.Preprocess(request.path);
        if (!preprocessedSrc) {
            return preprocessedSrc.GetError();
        }

        bool completedWithoutErrors = true;
        std::map<EShaderStage, ShaderModuleGL> shaderModules;
        GlID shaderProgramId = glCreateProgram();

        for (const auto& [stage, src] : *preprocessedSrc) {
            GlID id = CompileShaderStage(stage, src.ToStringView());
            if (id == KIWI_GL_UNDEFINED_ID) {
                completedWithoutErrors = false;
                continue;
            }

            shaderModules[stage].moduleID = id;
            glAttachShader(shaderProgramId, id);
        }

        if (!completedWithoutErrors) {
            ReleaseShaderResources(shaderProgramId, shaderModules);
            return Error::Create(EGeneralError::COMPILE_ERROR);
        }

        glLinkProgram(shaderProgramId);
        if (!CheckCompilationOrLinkingResult(shaderProgramId, EShaderStage::SHADER_PROGRAM)) {
            ReleaseShaderResources(shaderProgramId, shaderModules);
            return Error::Create(EGeneralError::LINKAGE_ERROR);
        }

        for (const auto& module : std::views::values(shaderModules)) {
            glDetachShader(shaderProgramId, module.moduleID);
        }

        return Success(std::shared_ptr<AShader>(new ShaderGL(shaderProgramId, std::move(shaderModules))));
    }

    bool ShaderCompilerGL::CheckCompilationOrLinkingResult(GLuint target, EShaderStage type) const {
        GLint hasNoErrors = 0;
        bool isProgramReceived = (type == EShaderStage::SHADER_PROGRAM);

        if (isProgramReceived) {
            glGetProgramiv(target, GL_LINK_STATUS, &hasNoErrors);
        }
        else {
            glGetShaderiv(target, GL_COMPILE_STATUS, &hasNoErrors);
        }

        if (hasNoErrors) {
            return true;
        }

        GLchar buffer[256] = { '\0' };
        auto bufferSize = static_cast<GLsizei>(std::size(buffer));
        if (isProgramReceived) {
            glGetProgramInfoLog(target, bufferSize, nullptr, buffer);
        }
        else {
            glGetShaderInfoLog(target, bufferSize, nullptr, buffer);
        }

        KIWI_CTX_LOG(ERROR, "An error occurred while {} shader. {}",
            isProgramReceived ? "linking" : "compiling",
            buffer
        );

        return false;
    }

    GlID ShaderCompilerGL::CompileShaderStage(EShaderStage stage, StringView src) const {
        KIWI_ASSERT(stage != EShaderStage::SHADER_PROGRAM && stage != EShaderStage::NONE,
                    "Either EShaderStage::SHADER_PROGRAM or EShaderStage::NONE passed here"
        );

        Hash64 hashedShaderSource = Hash64::FromData(src).GetOrDefault();
        if (hashedShaderSource.IsEmpty()) {
            KIWI_CTX_LOG(ERROR, "Failed to cast integer hash of the shader source into the string");
            return KIWI_GL_UNDEFINED_ID;
        }

        // std::filesystem::path outputFilePath = shaderCacheManager.GetCacheDirAbsolutePath() / hashedShaderSource.ToString().ToStdString();
        //
        // SpirV::CompilationDetails cDetails;
        // cDetails.stage = stage;
        // cDetails.src = src;
        // cDetails.environment = ESpirVEnvironment::OpenGL;
        // cDetails.optimizationLevel = ESpirVOptimizationLevel::PERFORMANCE;
        // cDetails.outputFile = outputFilePath.string();
        //
        // std::vector<u32> byteCode = SpirV::CompileGLSL(cDetails).ValueOr(std::vector<u32>{});
        // GlID id = CreateFromSpirVByteCode(stage, "main", byteCode);
        // if (id == KIWI_GL_UNDEFINED_ID) {
        //     return KIWI_GL_UNDEFINED_ID;
        // }
        //
        // if (!shaderCacheManager.AddToCache(hashedShaderSource, ShaderCacheEntry{ byteCode })) KIWI_UNLIKELY {
        //     KIWI_CTX_LOG(WARNING, "Failed to add {} in to the local or global cache!",
        //         hashedShaderSource
        //     );
        // }

        return 0;
    }


    GlID ShaderCompilerGL::CreateFromSpirVByteCode(EShaderStage stage, StringView entryPoint, const std::vector<u32>& byteCode) const {
        if (byteCode.empty()) {
            return KIWI_GL_UNDEFINED_ID;
        }

        GlID id = glCreateShader(Cast<EShaderStage>::ToGLenum(stage).ValueOr(KIWI_GL_UNDEFINED_ID));

        glShaderBinary(1, &id,
                        GL_SHADER_BINARY_FORMAT_SPIR_V, byteCode.data(),
                        static_cast<GLsizei>(byteCode.size() * sizeof(u32))
        );
        glSpecializeShader(id, entryPoint.data(), 0, nullptr, nullptr);


        if (!CheckCompilationOrLinkingResult(id, stage)) {
            glDeleteShader(id);
            return KIWI_GL_UNDEFINED_ID;
        }

        return id;
    }
}
