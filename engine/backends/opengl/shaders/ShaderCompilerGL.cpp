#include "ShaderCompilerGL.hpp"

#include <renderer/shaders/SpirV.hpp>

#include "ShaderGL.hpp"


namespace {
    void ReleaseShaderResources(Kiwi::OpenGL::GlID programId, const Kiwi::Map<Kiwi::EShaderStage, Kiwi::OpenGL::ShaderModuleGL>& modules) {
        for (const auto& module : std::views::values(modules)) {
            glDeleteShader(module.moduleID);
        }
        glDeleteProgram(programId);
    }
}


namespace Kiwi::OpenGL {
    std::shared_ptr<AShader> ShaderCompilerGL::CompileFile(const File& sourceFile) {
        FileContent src = sourceFile.ReadAll().ValueOr(FileContent{});
        if (src.IsEmpty()) {
            return nullptr;
        }

        return CompileSource(src.GetAsString());
    }

    std::shared_ptr<AShader> ShaderCompilerGL::CompileSource(const String& src) {
        auto optPreprocessedSrc = PreprocessSource(src);
        if (!optPreprocessedSrc) {
            return nullptr;
        }

        bool completedWithoutErrors = true;
        Map<EShaderStage, ShaderModuleGL> shaderModules;
        GlID shaderProgramId = glCreateProgram();

        for (const auto& [stage, src] : *optPreprocessedSrc) {
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
            return nullptr;
        }

        glLinkProgram(shaderProgramId);
        if (!CheckCompilationOrLinkingResult(shaderProgramId, EShaderStage::SHADER_PROGRAM)) {
            ReleaseShaderResources(shaderProgramId, shaderModules);
            return nullptr;
        }

        for (const auto& module : std::views::values(shaderModules)) {
            glDetachShader(shaderProgramId, module.moduleID);
        }

        return std::shared_ptr<ShaderGL>(new ShaderGL(shaderProgramId, std::move(shaderModules)));
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
        auto bufferSize = BasicCast::To<GLsizei>(std::size(buffer));
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

        const Opt<GLenum> glStage = Cast<EShaderStage>::ToGLenum(stage);
        if (!glStage) {
            KIWI_CTX_LOG(ERROR, "Unsupported shader stage passed to CompileShaderStage");
            return KIWI_GL_UNDEFINED_ID;
        }

        const GlID id = glCreateShader(*glStage);
        if (id == KIWI_GL_UNDEFINED_ID) {
            KIWI_CTX_LOG(ERROR, "glCreateShader failed to create a shader object");
            return KIWI_GL_UNDEFINED_ID;
        }

        // StringView is not guaranteed to be null-terminated, so the length is provided explicitly.
        const GLchar* sourcePtr = src.data();
        const GLint sourceLen = BasicCast::To<GLint>(src.size());
        glShaderSource(id, 1, &sourcePtr, &sourceLen);
        glCompileShader(id);

        if (!CheckCompilationOrLinkingResult(id, stage)) {
            glDeleteShader(id);
            return KIWI_GL_UNDEFINED_ID;
        }

        return id;
    }


    GlID ShaderCompilerGL::CreateFromSpirVByteCode(EShaderStage stage, StringView entryPoint, const Vector<u32>& byteCode) const {
        if (byteCode.empty()) {
            return KIWI_GL_UNDEFINED_ID;
        }

        GlID id = glCreateShader(Cast<EShaderStage>::ToGLenum(stage).value_or(KIWI_GL_UNDEFINED_ID));

        glShaderBinary(1, &id,
                        GL_SHADER_BINARY_FORMAT_SPIR_V, byteCode.data(),
                        BasicCast::To<GLsizei>(byteCode.size() * sizeof(u32))
        );
        glSpecializeShader(id, entryPoint.data(), 0, nullptr, nullptr);


        if (!CheckCompilationOrLinkingResult(id, stage)) {
            glDeleteShader(id);
            return KIWI_GL_UNDEFINED_ID;
        }

        return id;
    }
}
