#include "ShaderCompiler.hpp"

#include <renderer/shaders/SpirV.hpp>

#include <common/types/Result.hpp>



namespace Kiwi {
    Opt<PreprocessorGLSL::SourcesMap> AShaderCompiler::PreprocessSource(const String& src) {
        PreprocessorGLSL preprocessor;

        Result res = preprocessor.Preprocess(src);
        if (res) {
            return *res;
        }

        Opt<EShaderPreprocessError::Type> errKind = res.GetError().GetAsEnum<EShaderPreprocessError::Type>();
        KIWI_ASSERT_BASIC(errKind && *errKind != EShaderPreprocessError::NONE);

        // TODO: Idk where am i should output error messages, so for now just skip it
        switch (*errKind) {
        case EShaderPreprocessError::SHADER_VERSION_MISSING:
            break;
        case EShaderPreprocessError::INCORRECT_STAGE_NAME:
            break;
        case EShaderPreprocessError::END_OF_STAGE_MISSED:
            break;
        case EShaderPreprocessError::TOKEN_ALREADY_DECLARED:
            break;
        case EShaderPreprocessError::INCORRECT_PREPROCESSOR_PROPERTIES_COUNT:
            break;
        default:
            break;
        }

        return nullopt;
    }

    Result<Map<EShaderStage, Vector<u32>>> AShaderCompiler::CompileToSpirV(const CompilationDetails& compilationDetails) {
        Map<EShaderStage, Vector<u32>> result;

        for (const auto& [stage, src] : compilationDetails.preprocessedSrc) {
            SpirV::CompilationDetails details;
            details.stage = stage;
            details.src = src;
            details.environment = compilationDetails.environment;
            details.optimizationLevel = compilationDetails.optimizationLvl;


            if (auto r = SpirV::CompileGLSL(details)) {
                result[stage] = *r;
            }
            else {
                return r.GetError();
            }
        }

        return Success(result);
    }

    Result<Map<EShaderStage, Vector<u32>>> AShaderCompiler::PreprocessAndCompileToSpirV(const File& sourceFile, ESpirVEnvironment env, ESpirVOptimizationLevel optimizationLvl) {
        Result src = sourceFile.ReadAll();
        if (!src) {
            KIWI_LOG(ERROR, "Failed to read source file: {}",  src.GetError().GetDescription());

            return Error::Create(EGeneralError::COMPILE_ERROR);
        }

        if (const Opt preprocessedSrc = PreprocessSource(src->GetAsString())) {
            CompilationDetails d;
            d.environment = env;
            d.optimizationLvl = optimizationLvl;
            d.preprocessedSrc = *preprocessedSrc;

            if (Result compiledSpirV = CompileToSpirV(d)) {
                return compiledSpirV;
            }
            else {
                return compiledSpirV.GetError();
            }
        }
        else {

        }
    }
}
