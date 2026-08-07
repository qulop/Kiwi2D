#pragma once

#include <core/Object.hpp>

#include <common/meta/TypeTraits.hpp>
#include <common/filesystem/File.hpp>

#include <common/Definitions.hpp>

#include <renderer/shaders/ShaderStage.hpp>
#include <renderer/shaders/PreprocessorGLSL.hpp>
#include <renderer/shaders/Shader.hpp>
#include <renderer/shaders/SpirV.hpp>
#include <renderer/pipeline/RenderAPI.hpp>


namespace Kiwi {
    struct ShaderCompilationRequest {
        std::filesystem::path path;
        StringView entryPoint = "main";
    };


    class AShaderCompiler : public AObject {
        KIWI_CREATE_OBJECT(AShaderCompiler, AObject)

    public:
        struct CompilationDetails {
            ESpirVEnvironment environment = ESpirVEnvironment::Vulkan;
            ESpirVOptimizationLevel optimizationLvl = ESpirVOptimizationLevel::PERFORMANCE;
            PreprocessorGLSL::SourcesMap preprocessedSrc;
        };

    public:
        KIWI_NODISCARD static std::unique_ptr<AShaderCompiler> Create(ERenderAPI::Type api);

        KIWI_NODISCARD static Result<std::map<EShaderStage, std::vector<u32>>> CompileToSpirV(const CompilationDetails& compilationDetails);

        KIWI_NODISCARD static Result<std::map<EShaderStage, std::vector<u32>>> PreprocessAndCompileToSpirV(const File& sourceFile, ESpirVEnvironment env, ESpirVOptimizationLevel optimizationLvl);

    public:
        KIWI_NODISCARD virtual Result<std::shared_ptr<AShader>> CompileFile(const ShaderCompilationRequest& request) = 0;

        ~AShaderCompiler() override = default;
    };
}