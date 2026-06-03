#include "AssetManager.hpp"

#include <core/resources/Asset.hpp>
#include <core/resources/AssetImporter.hpp>


namespace Kiwi {
   bool AssetManager::BindToRegistry(const std::filesystem::path& registryPath) {
        if (registryPath.empty() || !std::filesystem::is_directory(registryPath)) {
            if (registryPath.empty()) {
                KIWI_LOG(ERROR, "Failed to bind asset manager to a registry, because provided path is empty");
            }
            else {
                KIWI_LOG(ERROR, "Failed to bind asset manager to a registry, because provided path not a directory: {}",
                    registryPath.string()
                );
            }
            return false;
        }

        if (!m_registry.empty()) {
            m_registry.clear();
        }

        constexpr auto options = std::filesystem::directory_options::follow_directory_symlink;
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(registryPath, options)) {
            if (entry.is_regular_file() && entry.path().extension() == AssetMetaData::METADATA_EXTENSION) {
                if (const Result<AssetMetaData> assetMetaData = AssetImporter::OpenMetaData(entry.path())) {
                    m_registry[assetMetaData->assetUUID] = *assetMetaData;
                }
                else {
                    KIWI_LOG(ERROR, "Failed to open a metadata file: {}. The reason: {}",
                        entry.path().string(),
                        assetMetaData.GetError().GetDescription()
                    );
                }
            }
        }

        return true;
    }

    bool AssetManager::IsValidUUID(const UUID assetUUID) const {
        return m_registry.contains(assetUUID);
    }

    bool AssetManager::IsAssetLoaded(UUID assetUUID) const {
        return m_loadedAssets.contains(assetUUID);
    }

    std::shared_ptr<AAsset> AssetManager::GetAsset(UUID assetUUID) {
        if (!IsValidUUID(assetUUID)) {
            return nullptr;
        }

        if (IsAssetLoaded(assetUUID)) {
            return m_loadedAssets.at(assetUUID);   
        }
        else {
            const AssetMetaData& assetMetaData = m_registry.at(assetUUID);
            return AssetImporter::ImportAsset(assetMetaData);
        }
    }
}

