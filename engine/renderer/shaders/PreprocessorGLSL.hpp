#pragma once

#include "Shader.hpp"

#include <misc/ParserBase.hpp>

#include <common/types/Errors.hpp>


namespace Kiwi {
    namespace EShaderPreprocessError {
        enum Type : u16 {
            NONE,
            INCORRECT_STAGE_NAME,
            END_OF_STAGE_MISSED,
            TOKEN_ALREADY_DECLARED,
            INCORRECT_PREPROCESSOR_PROPERTIES_COUNT,
            SHADER_VERSION_MISSING,
        };

        KIWI_NODISCARD String ToString(Type type);
    }


    template<>
    struct ErrorDescription<EShaderPreprocessError::Type> {
        static constexpr StringView ERROR_CATEGORY_NAME = "Shaders";

        KIWI_NODISCARD static String Describe(u16 t) {
            return EShaderPreprocessError::ToString(static_cast<EShaderPreprocessError::Type>(t));
        }
    };



    class PreprocessorGLSL : protected Misc::ParserBase {
    public:
        using SourcesMap = std::unordered_map<EShaderStage, String>;
        using PreprocessResult = Result<SourcesMap>;
        using PreprocessorProperties= std::pair<String, std::vector<String>>;

    public:
        PreprocessorGLSL() = default;

    public:
        KIWI_NODISCARD Result<SourcesMap> Preprocess(const std::filesystem::path& path);

    private:
        KIWI_NODISCARD Opt<size_t> FindPreprocessorPosition(StringView token);

        KIWI_NODISCARD Opt<PreprocessorProperties> ExtractPreprocessor(StringView token);
    };
}