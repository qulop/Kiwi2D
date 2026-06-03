#include "AssetImporter.hpp"

#include <core/resources/Asset.hpp>
#include <core/ProjectSubsystem.hpp>
#include <core/Project.hpp>

#include <renderer/Texture.hpp>
#include <renderer/shaders/ShaderCompiler.hpp>
#include <renderer/shaders/ShaderBundle.hpp>

#include <common/types/INI.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace Kiwi {
    Result<AssetMetaData> AssetImporter::OpenMetaData(const std::filesystem::path& assetPath) {
        INI metaData = INI::FromPath(assetPath);
        if (metaData.IsEmpty()) {
            return Error::Create(EGeneralError::PARSE_ERROR, "Failed to parse asset metadata file");
        }

        AssetMetaData res;
        const auto& mainIniFileSection = metaData[AssetMetaData::IniKeys::MAIN_SECTION_NAME];

        Opt<UUID> uuidParseResult = UUID::FromString(
            mainIniFileSection[AssetMetaData::IniKeys::UUID].As<String>()
        );
        if (uuidParseResult) {
            res.assetUUID = *uuidParseResult;
        }
        else {
            return Error::Create(EGeneralError::PARSE_ERROR, "Failed to parse asset UUID");
        }

        res.type = EAssetType::FromString(
            mainIniFileSection[AssetMetaData::IniKeys::ASSET_TYPE]
                .As<String>()
                .ToStringView()
        );

        res.assetPath = mainIniFileSection[AssetMetaData::IniKeys::ASSET_TYPE].As<std::filesystem::path>();

        return Success(res);
    }

    std::shared_ptr<AAsset> AssetImporter::ImportAsset(const AssetMetaData& assetMetaData) {
        switch (assetMetaData.type) {
        case EAssetType::SHADER:
            return ImportShaderAsset(assetMetaData);

        case EAssetType::TEXTURE_2D:
            return ImportTextureAsset(assetMetaData);

        default:
            return nullptr;
        }
    }

    std::shared_ptr<AAsset> AssetImporter::ImportShaderAsset(const AssetMetaData& assetMetaData) {
        Result<File> shaderFile = File::OpenFileStatic(assetMetaData.assetPath, EFileOpenMode::WRITE);
        if (!shaderFile) {
            KIWI_LOG(ERROR, "Failed to import a shader asset, because provided path is invalid: {}",
                assetMetaData.assetPath.string()
            );

            return nullptr;
        }

        std::unique_ptr<AShaderCompiler> shaderCompiler = AShaderCompiler::Create();
        KIWI_ENSURE(shaderCompiler);

        if (std::shared_ptr<AShader> result = shaderCompiler->CompileFile(*shaderFile)) {
            return std::static_pointer_cast<AAsset>(result);
        }

        KIWI_LOG(ERROR, "Failed to compile a shader");
        return nullptr;
    }

    std::shared_ptr<AAsset> AssetImporter::ImportTextureAsset(const AssetMetaData& assetMetaData) {
        stbi_set_flip_vertically_on_load(true);

        const String assetPath = assetMetaData.assetPath.string();

        i32 channels = 0, width = 0, height = 0;
        stbi_uc* data = stbi_load(assetPath.ToCString(), &width, &height, &channels, 0);

        if (!data) {
            // KIWI_LOG(ERROR, "Failed to load a texture by the path: {}", assetMetaData.assetPath.string());
            return nullptr;
        }


        ImageDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = EImageFormat::FromChannels(channels);
        desc.channels = channels;
        desc.size = width * height * channels;
        desc.data = data;

        return ATexture2D::Create(desc);
    }
}
