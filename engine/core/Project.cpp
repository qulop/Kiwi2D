#include "Project.hpp"

#include <core/resources/AssetManager.hpp>
#include <core/resources/Asset.hpp>

#include <common/types/String.hpp>
#include <common/filesystem/File.hpp>


namespace Kiwi {
    nlohmann::ordered_json ProjectConfig::ToJSON() const {
        return nlohmann::ordered_json {
            { Keys::PROJECT_NAME, projectName.ToStdString() },
            { Keys::PROJECT_UUID, projectUUID.ToString().ToStdString() },
            { Keys::ENGINE_DATA_DIRECTORY, engineDataDirectoryPath },
            { Keys::PROJECT_ASSET_REGISTRY, assetRegistryPath },
            { Keys::PROJECT_CACHE, cacheDirectoryPath },
            { Keys::ASSETS_DIRECTORY, assetsPath },
        };
    }

    std::array<std::filesystem::path, 4> ProjectConfig::GetDirectoryPaths() const {
        return std::array {
            engineDataDirectoryPath,
            assetRegistryPath,
            cacheDirectoryPath,
            assetsPath,
        };
    }

    Opt<ProjectConfig> ProjectConfig::ReadConfig(const std::filesystem::path& configPath) {
        if (!std::filesystem::exists(configPath) || !std::filesystem::is_regular_file(configPath)) {
            return nullopt;
        }

        const String content = File::LoadFromFile(configPath, EFileOpenMode::READ | EFileOpenMode::BINARY)
            .ValueOr(FileContent())
            .GetAsString();
        if (content.IsEmpty()) {
            return nullopt;
        }

        nlohmann::json jsonConfig = nlohmann::json::parse(content, nullptr, false);
        if (jsonConfig.is_discarded() || jsonConfig.empty()) {
            return nullopt;
        }

        ProjectConfig result;
        if (const Opt<UUID> uuid = UUID::FromString(jsonConfig[Keys::PROJECT_UUID].get<std::string>())) {
            result.projectUUID = *uuid;
        }
        else {
            return nullopt;
        }

        result.projectName = jsonConfig[Keys::PROJECT_NAME].get<std::string>();
        result.engineDataDirectoryPath = jsonConfig[Keys::ENGINE_DATA_DIRECTORY].get<std::filesystem::path>();
        result.assetRegistryPath = jsonConfig[Keys::PROJECT_ASSET_REGISTRY].get<std::filesystem::path>();
        result.cacheDirectoryPath = jsonConfig[Keys::PROJECT_CACHE].get<std::filesystem::path>();
        result.assetsPath = jsonConfig[Keys::ASSETS_DIRECTORY].get<std::filesystem::path>();

        return result;
    }

    ProjectConfig ProjectConfig::CreateNew(StringView projectName, const std::filesystem::path& projectPath) {
        ProjectConfig result;
        result.projectName = projectName;
        result.projectUUID = UUID::Generate();

        result.engineDataDirectoryPath = projectPath / Project::ENGINE_DATA_DIRECTORY_NAME;
        result.assetRegistryPath = result.engineDataDirectoryPath / Project::ASSETS_REGISTRY_DIRECTORY_NAME;
        result.cacheDirectoryPath = result.engineDataDirectoryPath / Project::CACHE_DIRECTORY_NAME;

        result.assetsPath = projectPath / Project::ASSETS_DIRECTORY_NAME;

        return result;
    }



    std::shared_ptr<Project> Project::FromConfig(const std::filesystem::path& projectPath, const ProjectConfig& config) {
        for (const auto& directory : config.GetDirectoryPaths()) {
            if (!std::filesystem::exists(directory)) {
                std::error_code ec;
                std::filesystem::create_directories(directory, ec);

                if (ec) {
                    return nullptr;
                }

                ec.clear();
            }
        }

        auto&& assetManager = std::make_shared<AssetManager>();
        if (assetManager->BindToRegistry(config.assetRegistryPath)) {
            auto&& project = std::make_shared<Project>();
            project->m_projectDirectory = projectPath;
            project->m_config = config;
            project->m_assetManager = std::move(assetManager);

            return project;
        }

        return nullptr;
    }

    std::shared_ptr<Project> Project::Open(const std::filesystem::path& projectPath) {
        const bool isCorrectPath = std::filesystem::exists(projectPath) && std::filesystem::is_directory(projectPath);
        if (projectPath.empty() || !isCorrectPath) {
            return nullptr;
        }

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(projectPath)) {
            if (entry.is_regular_file() && entry.path().extension() == KIWI_PROJECT_EXTENSION) {
                const Opt<ProjectConfig> config = ProjectConfig::ReadConfig(entry.path());
                if (!config) {
                    return nullptr;
                }

                return FromConfig(projectPath, *config);
            }
        }

        return nullptr;
    }

    std::shared_ptr<Project> Project::CreateNew(StringView projectName, const std::filesystem::path& projectPath) {
        if (projectPath.empty()) {
            return nullptr;
        }

        std::error_code ec;
        std::filesystem::create_directories(projectPath, ec);
        if (ec) {
            return nullptr;
        }

        const ProjectConfig config = ProjectConfig::CreateNew(projectName, projectPath);
        const String projectConfigFileName = String::Format("{}{}", projectName, KIWI_PROJECT_EXTENSION);

        const Result<void, EErrorIO> saveResult = File::SaveInFile(
            projectPath / projectConfigFileName.ToStdString(),
            config.ToJSON().dump(4),
            true
        );
        if (!saveResult) {
            return nullptr;
        }

        return FromConfig(projectPath, config);
    }

    const ProjectConfig& Project::GetConfig() const {
        return m_config;
    }

    const std::filesystem::path& Project::GetProjectDirectory() const {
        return m_projectDirectory;
    }

    std::shared_ptr<AssetManager> Project::GetAssetManager() const {
        return m_assetManager;
    }
}
