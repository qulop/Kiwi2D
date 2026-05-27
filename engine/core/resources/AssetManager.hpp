#pragma once

#include <core/Object.hpp>

#include <common/UUID.hpp>


namespace Kiwi {
    class AAsset;
    struct AssetMetaData;


    class AssetManager : public AObject {
        KIWI_CREATE_OBJECT(AssetManager, AObject);

    public:
        KIWI_NODISCARD bool BindToRegistry(const std::filesystem::path& registryPath);

        KIWI_NODISCARD bool IsValidUUID(UUID assetUUID) const;
        KIWI_NODISCARD bool IsAssetLoaded(UUID assetUUID) const;

        template<Concepts::DerivedFrom<AAsset> TAsset>
        KIWI_NODISCARD std::shared_ptr<TAsset> GetAsset(const UUID assetUUID) {
            // TODO: Change dynamic cast with static cast by applying AObject derive check
            if (std::shared_ptr<AAsset> r = GetAsset(assetUUID)) {
                return std::dynamic_pointer_cast<TAsset>(r);
            }

            return nullptr;
        }

        KIWI_NODISCARD std::shared_ptr<AAsset> GetAsset(UUID assetUUID);

    private:
        std::map<UUID, AssetMetaData> m_registry;
        std::map<UUID, std::shared_ptr<AAsset>> m_loadedAssets;
    };

}
