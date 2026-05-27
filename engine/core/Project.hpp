#pragma once

#include <common/UUID.hpp>

#include <core/Object.hpp>

#include <nlohmann/json.hpp>


namespace Kiwi {
    class AssetManager;


    struct ProjectConfig {
        String projectName;
        UUID projectUUID;
        std::filesystem::path assetRegistryPath;
        std::filesystem::path cacheDirectoryPath;

        struct Keys {
            static constexpr StringView PROJECT_NAME = "ProjectName";
            static constexpr StringView PROJECT_UUID = "UUID";
            static constexpr StringView PROJECT_CACHE = "CacheDirectory";
            static constexpr StringView PROJECT_ASSET_REGISTRY = "AssetRegistryDirectory";
        };

        KIWI_NODISCARD nlohmann::json ToJSON() const;

        KIWI_NODISCARD static Opt<ProjectConfig> ReadConfig(const std::filesystem::path& configPath);
        KIWI_NODISCARD static ProjectConfig CreateNew(StringView projectName, const std::filesystem::path& projectPath);
    };

    class Project final : public AObject {
        KIWI_CREATE_OBJECT(Project, AObject)

    public:

        static constexpr StringView KIWI_PROJECT_EXTENSION = ".kiwiprj";
        static constexpr StringView KIWI_ASSETS_REGISTRY_ROOT_DIRECTORY_NAME = "Assets";

        static constexpr StringView CONFIG_DIRECTORY_NAME = ".kiwi";
        static constexpr StringView CACHE_DIRECTORY_NAME = "cache";
        static constexpr StringView SHADER_CACHE_DIRECTORY_NAME = "shaders";

    public:
        KIWI_NODISCARD static std::shared_ptr<Project> FromConfig(const std::filesystem::path& projectPath, const ProjectConfig& config);

        KIWI_NODISCARD static std::shared_ptr<Project> Open(const std::filesystem::path& projectPath);
        KIWI_NODISCARD static std::shared_ptr<Project> CreateNew(StringView projectName, const std::filesystem::path& projectPath);


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
