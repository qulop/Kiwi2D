#pragma once

#include <renderer/shaders/ShaderCompiler.hpp>
#include <renderer/shaders/PreprocessorGLSL.hpp>

#include <driver/opengl/basic/TypesGL.hpp>



namespace Kiwi::OpenGL {
    class ShaderCompilerGL : public AShaderCompiler {
        KIWI_CREATE_OBJECT(ShaderCompilerGL, AShaderCompiler)

    private:
        using SourcesMap = typename PreprocessorGLSL::SourcesMap;

    public:
        KIWI_NODISCARD std::shared_ptr<AShader> CompileFile(const File& sourceFile) override;

        KIWI_NODISCARD bool CheckCompilationOrLinkingResult(GLuint target, EShaderStage type) const;

    private:
        KIWI_NODISCARD GlID CompileShaderStage(EShaderStage stage, StringView src) const;

        KIWI_NODISCARD GlID CreateFromSpirVByteCode(EShaderStage stage, StringView entryPoint, const Vector<u32>& byteCode) const;
    };
}