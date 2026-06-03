#pragma once

#include <common/UUID.hpp>

#include <core/Object.hpp>

#include <renderer/pipeline/RenderAPI.hpp>

#include <nlohmann/json.hpp>


namespace Kiwi {
    class AssetManager;


    struct ProjectCreateInfo {
        StringView projectName;
        std::filesystem::path projectPath;
        ERenderAPI::Type renderAPI = ERenderAPI::NONE;
    };



    struct ProjectConfig {
        String projectName;
        UUID projectUUID;
        ERenderAPI::Type renderAPI = ERenderAPI::NONE;

        // Folders inside the `.kiwi`
        std::filesystem::path engineDataDirectoryPath;
        std::filesystem::path assetRegistryPath;
        std::filesystem::path cacheDirectoryPath;

        std::filesystem::path assetsPath;


        struct Keys {
            static constexpr StringView PROJECT_NAME = "ProjectName";
            static constexpr StringView PROJECT_UUID = "UUID";
            static constexpr StringView ENGINE_DATA_DIRECTORY = "EngineDataDirectory";
            static constexpr StringView PROJECT_ASSET_REGISTRY = "AssetRegistryDirectory";
            static constexpr StringView PROJECT_CACHE = "DataCacheDirectory";
            static constexpr StringView ASSETS_DIRECTORY = "AssetsDirectory";
            static constexpr StringView RENDERER = "Renderer";
        };

        KIWI_NODISCARD nlohmann::ordered_json ToJSON() const;

        KIWI_NODISCARD std::array<std::filesystem::path, 4> GetDirectoryPaths() const;

        KIWI_NODISCARD static Result<ProjectConfig> ReadConfig(const std::filesystem::path& configPath);
        KIWI_NODISCARD static ProjectConfig CreateNew(const ProjectCreateInfo& createInfo);
    };



    class Project final : public AObject {
        KIWI_CREATE_OBJECT(Project, AObject)

    public:

        static constexpr StringView KIWI_PROJECT_EXTENSION = ".kiwiprj";
        static constexpr StringView ASSETS_DIRECTORY_NAME = "Assets";
        static constexpr StringView ASSETS_REGISTRY_DIRECTORY_NAME = "AssetsRegistry";
        static constexpr StringView ENGINE_DATA_DIRECTORY_NAME = ".kiwi";
        static constexpr StringView CACHE_DIRECTORY_NAME = "DataCache";

    public:
        KIWI_NODISCARD static Result<std::shared_ptr<Project>> FromConfig(const std::filesystem::path& projectPath, const ProjectConfig& config);

        KIWI_NODISCARD static Result<std::shared_ptr<Project>> Open(const std::filesystem::path& projectPath);
        KIWI_NODISCARD static Result<std::shared_ptr<Project>> CreateNew(const ProjectCreateInfo& createInfo);


    public:
        Project() = default;

        KIWI_NODISCARD const ProjectConfig& GetConfig() const;

        KIWI_NODISCARD const std::filesystem::path& GetProjectDirectory() const;

        KIWI_NODISCARD std::shared_ptr<AssetManager> GetAssetManager() const;

    private:
        std::filesystem::path m_projectDirectory;
        ProjectConfig m_config;

        std::shared_ptr<AssetManager> m_assetManager;
    };
}
