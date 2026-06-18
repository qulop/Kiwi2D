#pragma once

#include <core/Object.hpp>

#include <common/meta/TypeTraits.hpp>
#include <common/filesystem/File.hpp>

#include <common/Definitions.hpp>

#include <renderer/shaders/ShaderStage.hpp>
#include <renderer/shaders/PreprocessorGLSL.hpp>
#include <renderer/shaders/Shader.hpp>
#include <renderer/shaders/SpirV.hpp>



namespace Kiwi {
    class AShaderCompiler : public AObject {
        KIWI_CREATE_OBJECT(AShaderCompiler, AObject)

    public:
        struct CompilationDetails {
            ESpirVEnvironment environment = ESpirVEnvironment::Vulkan;
            ESpirVOptimizationLevel optimizationLvl = ESpirVOptimizationLevel::PERFORMANCE;
            PreprocessorGLSL::SourcesMap preprocessedSrc;
        };

    public:
        KIWI_NODISCARD static std::unique_ptr<AShaderCompiler> Create();

        KIWI_NODISCARD static Opt<PreprocessorGLSL::SourcesMap> PreprocessSource(const String& src);
        KIWI_NODISCARD static Result<Map<EShaderStage, Vector<u32>>> CompileToSpirV(const CompilationDetails& compilationDetails);

        KIWI_NODISCARD static Result<Map<EShaderStage, Vector<u32>>> PreprocessAndCompileToSpirV(const File& sourceFile, ESpirVEnvironment env, ESpirVOptimizationLevel optimizationLvl);

    public:
        KIWI_NODISCARD virtual std::shared_ptr<AShader> CompileFile(const File& sourceFile) = 0;
        KIWI_NODISCARD virtual std::shared_ptr<AShader> CompileSource(const String& src) = 0;

        ~AShaderCompiler() override = default;
    };
}