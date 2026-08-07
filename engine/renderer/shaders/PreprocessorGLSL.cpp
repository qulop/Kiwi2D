#include "PreprocessorGLSL.hpp"

#include <common/types/String.hpp>
#include <common/filesystem/File.hpp>


namespace {
    constexpr Kiwi::StringView VERSION_TOKEN_NAME = "version";
    constexpr Kiwi::StringView STAGE_BEGIN_TOKEN_NAME = "stage";
    constexpr Kiwi::StringView STAGE_END_TOKEN_NAME = "endstage";
}


namespace Kiwi {
    String EShaderPreprocessError::ToString(Type type) {
        switch (type) {
        case INCORRECT_STAGE_NAME:
            return "INCORRECT_STAGE_NAME";
        case END_OF_STAGE_MISSED:
            return "END_OF_STAGE_MISSED";
        case TOKEN_ALREADY_DECLARED:
            return "TOKEN_ALREADY_DECLARED";
        case INCORRECT_PREPROCESSOR_PROPERTIES_COUNT:
            return "INCORRECT_PREPROCESSOR_PROPERTIES_COUNT";
        case SHADER_VERSION_MISSING:
            return "SHADER_VERSION_MISSING";
        default:
            return "NONE";
        }
    }
    
    

    Result<PreprocessorGLSL::SourcesMap> PreprocessorGLSL::Preprocess(const std::filesystem::path& path) {
        Result<FileContent> loadResult = File::LoadFromFile(path, EFileOpenMode::READ);
        if (!loadResult) {
            return loadResult.GetError();
        }
        const String src = loadResult.GetValue().GetAsString();

        Reset(src.ToStringView());

        SourcesMap result;
        auto&& [_, versionProps] = ExtractPreprocessor(VERSION_TOKEN_NAME).GetOrDefault();
        if (versionProps.empty()) {
            return Error::Create(
                EShaderPreprocessError::SHADER_VERSION_MISSING
            );
        }

        while (true) {
            auto&& [preprocName, preprocProps] = ExtractPreprocessor(STAGE_BEGIN_TOKEN_NAME)
                .ValueOr(std::make_pair(String{}, std::vector<String>{}));

            if (preprocName.IsEmpty() || preprocName != STAGE_BEGIN_TOKEN_NAME) {
                if (preprocName.IsEmpty()) {
                    break;
                }
                continue;
            }

            if (preprocProps.size() != 1) {
                return Error::Create(
                    EShaderPreprocessError::INCORRECT_PREPROCESSOR_PROPERTIES_COUNT
                );
            }

            Opt<EShaderStage> shaderStageName = String::ParseShaderStage(preprocProps.at(0).ToStringView());
            if (!shaderStageName) {
                return Error::Create(
                    EShaderPreprocessError::INCORRECT_STAGE_NAME
                );
            }

            const size_t stageCodeBeginPos = JumpToNextLine();
            const size_t stageCodeEndPos = FindPreprocessorPosition(STAGE_END_TOKEN_NAME).ValueOr(StringView::npos);

            if (stageCodeEndPos == StringView::npos) {
                return Error::Create(
                    EShaderPreprocessError::END_OF_STAGE_MISSED
                );
            }

            String& shaderStageCode = result[*shaderStageName];
            if (!versionProps.empty()) {
                shaderStageCode = String::Format("#version {}\n", String::Join(std::span<String>{ versionProps }));
            }

            result[*shaderStageName] += src.CreateSlice(stageCodeBeginPos, stageCodeEndPos);
            m_currPos = stageCodeEndPos + 1;
        }

        return Success(result);
    }

    Opt<size_t> PreprocessorGLSL::FindPreprocessorPosition(StringView token) {
        size_t searchPos = m_currPos;
        while ((searchPos = m_src.find_first_of('#', m_currPos)) != StringView::npos) {
            size_t tokenBegin = m_src.find_first_not_of(Globals::Misc::WHITESPACE, searchPos + 1);

            auto optCurrToken = GetCurrentToken(tokenBegin);
            if (!optCurrToken) {
                return ZERO_OPT;
            }

            if (*optCurrToken == token) {
                return searchPos;
            }

            searchPos = m_currPos = (tokenBegin + token.length());
        }

        return ZERO_OPT;
    }

    Opt<PreprocessorGLSL::PreprocessorProperties> PreprocessorGLSL::ExtractPreprocessor(StringView token) {
        size_t pos = FindPreprocessorPosition(token).ValueOr(StringView::npos);
        if (pos == StringView::npos) {
            return ZERO_OPT;
        }

        pos = m_src.find_first_not_of(Globals::Misc::WHITESPACE, pos + 1);
        String foundPreprocessor = GetCurrentToken(pos).ValueOr("");
        if (foundPreprocessor.IsEmpty()) {
            return ZERO_OPT;
        }

        pos += foundPreprocessor.Size() + 1;

        size_t endOfLinePos = m_src.find_first_of(Globals::Misc::END_OF_LINE, pos);
        if (endOfLinePos == StringView::npos) {
            return ZERO_OPT;
        }

        Opt<std::vector<String>> properties = Tokenize(pos, endOfLinePos);

        m_currPos = pos + 1;
        return std::make_pair(
            std::move(foundPreprocessor),
            std::move(*properties)
        );
    }
}