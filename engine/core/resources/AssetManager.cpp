#include "AssetManager.hpp"

#include <core/resources/Asset.hpp>
#include <core/resources/AssetImporter.hpp>

#include <renderer/Texture.hpp>
#include <renderer/shaders/Shader.hpp>


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

        const AssetMetaData& assetMetaData = m_registry.at(assetUUID);
        std::shared_ptr<AAsset> imported = AssetImporter::ImportAsset(assetMetaData);
        if (imported) {
            m_loadedAssets[assetUUID] = imported;
        }

        return imported;
    }

    std::shared_ptr<ATexture2D> AssetManager::GetTexture(const std::filesystem::path& path) {
        const UUID uuid = ResolveUUIDByPath(path);
        if (uuid.IsEmpty()) {
            KIWI_CTX_LOG(ERROR, "No texture asset registered for path: {}", path.string());
            return nullptr;
        }

        return GetAsset<ATexture2D>(uuid);
    }

    std::shared_ptr<AShader> AssetManager::GetShader(const std::filesystem::path& path) {
        const UUID uuid = ResolveUUIDByPath(path);
        if (uuid.IsEmpty()) {
            KIWI_CTX_LOG(ERROR, "No shader asset registered for path: {}", path.string());
            return nullptr;
        }

        return GetAsset<AShader>(uuid);
    }

    UUID AssetManager::ResolveUUIDByPath(const std::filesystem::path& path) const {
        std::error_code ec;
        const std::filesystem::path target = std::filesystem::weakly_canonical(path, ec);

        for (const auto& [uuid, metaData] : m_registry) {
            std::error_code entryEc;
            const std::filesystem::path candidate = std::filesystem::weakly_canonical(metaData.assetPath, entryEc);

            if (!ec && !entryEc) {
                if (candidate == target) {
                    return uuid;
                }
            }
            else if (metaData.assetPath == path) {
                return uuid;
            }
        }

        return UUID{};
    }
}

