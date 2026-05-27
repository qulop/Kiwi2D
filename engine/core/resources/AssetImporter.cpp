#include "AssetImporter.hpp"

#include <core/resources/Asset.hpp>

#include <renderer/Texture.hpp>
#include <renderer/shaders/ShaderBundle.hpp>

#include <common/types/INI.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace Kiwi {
    Opt<AssetMetaData> AssetImporter::OpenMetaData(const std::filesystem::path& assetPath) {
        INI metaData = INI::FromPath(assetPath);
        if (metaData.IsEmpty()) {
            return nullopt;
        }

        AssetMetaData res;
        const auto& mainIniFileSection = metaData[AssetMetaData::IniFile::MAIN_SECTION_NAME];

        Opt<UUID> uuidParseResult = UUID::FromString(
            mainIniFileSection[AssetMetaData::IniFile::UUID_KEY_NAME].As<String>()
        );
        if (uuidParseResult) {
            res.assetUUID = *uuidParseResult;
        }
        else {
            return nullopt;
        }

        res.type = EAssetType::FromString(
            mainIniFileSection[AssetMetaData::IniFile::ASSET_TYPE_KEY_NAME]
                .As<String>()
                .ToStringView()
        );

        res.assetPath = mainIniFileSection[AssetMetaData::IniFile::ASSET_TYPE_KEY_NAME].As<std::filesystem::path>();

        return res;
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
        KIWI_IGNORE_RETURN(assetMetaData);
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
