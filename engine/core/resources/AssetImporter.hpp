#pragma once

#include <core/Object.hpp>

#include <common/Definitions.hpp>



namespace Kiwi {
    class AAsset;
    struct AssetMetaData;


    class AssetImporter : public AObject {
        KIWI_CREATE_OBJECT(AssetImporter, AObject)

    public:
        KIWI_NODISCARD static Result<AssetMetaData> OpenMetaData(const std::filesystem::path& assetPath);

        KIWI_NODISCARD static std::shared_ptr<AAsset> ImportAsset(const AssetMetaData& assetMetaData);

        KIWI_NODISCARD static std::shared_ptr<AAsset> ImportShaderAsset(const AssetMetaData& assetMetaData);
        KIWI_NODISCARD static std::shared_ptr<AAsset> ImportTextureAsset(const AssetMetaData& assetMetaData);
    };
}