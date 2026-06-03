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

   Result<ProjectConfig> ProjectConfig::ReadConfig(const std::filesystem::path& configPath) {
        if (!std::filesystem::exists(configPath)) {
            return Error::Create(
                EErrorIO::DOES_NOT_EXIST,
                String::Format("Specified path does not exist: {}", configPath.string())
            );
        }
        if (!std::filesystem::is_regular_file(configPath)) {
            return Error::Create(
                EErrorIO::NOT_A_FILE,
                String::Format("Specified path is not a regular file: {}", configPath.string())
            );
        }

        if (const Result content = File::LoadFromFile(configPath, EFileOpenMode::READ | EFileOpenMode::BINARY)) {
            nlohmann::json jsonConfig = nlohmann::json::parse(content.GetValue().GetAsString(), nullptr, false);
            if (jsonConfig.is_discarded() || jsonConfig.empty()) {
                return Error::Create(
                    EGeneralError::PARSE_ERROR,
                    String::Format("Failed to parse project config file: {}", configPath.string())
                );
            }

            ProjectConfig result;
            if (const Opt<UUID> uuid = UUID::FromString(jsonConfig.value(Keys::PROJECT_UUID, std::string()))) {
                result.projectUUID = *uuid;
            }
            else {
                return Error::Create(
                    EGeneralError::PARSE_ERROR,
                    String::Format("Failed to parse project config file: {}", configPath.string())
                );
            }

            result.projectName = jsonConfig.value(Keys::PROJECT_NAME, std::string());

            result.engineDataDirectoryPath = jsonConfig.value(Keys::ENGINE_DATA_DIRECTORY, std::filesystem::path());
            result.assetRegistryPath = jsonConfig.value(Keys::PROJECT_ASSET_REGISTRY, std::filesystem::path());
            result.cacheDirectoryPath = jsonConfig.value(Keys::PROJECT_CACHE, std::filesystem::path());
            result.assetsPath = jsonConfig.value(Keys::ASSETS_DIRECTORY, std::filesystem::path());

            return Success(result);
        }
        else {
            return content.GetError();
        }
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



    Result<std::shared_ptr<Project>> Project::FromConfig(const std::filesystem::path& projectPath, const ProjectConfig& config) {
        for (const auto& directory : config.GetDirectoryPaths()) {
            if (!std::filesystem::exists(directory)) {
                std::error_code ec;
                std::filesystem::create_directories(directory, ec);

                if (ec) {
                    return Error::Create(EGeneralError::CREATION_FAILED, ec.message());
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

            return Success(project);
        }

        return Error::Create(EGeneralError::CREATION_FAILED, "Failed to bind asset manager to the provided asset registry");
    }

    Result<std::shared_ptr<Project>> Project::Open(const std::filesystem::path& projectPath) {
        const bool isCorrectPath = std::filesystem::exists(projectPath) && std::filesystem::is_directory(projectPath);
        if (projectPath.empty() || !isCorrectPath) {
            return Error::Create(EErrorIO::INVALID_PATH, "Provided path either empty or is not correct(not exists or is directory)");
        }

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(projectPath)) {
            if (entry.is_regular_file() && entry.path().extension() == KIWI_PROJECT_EXTENSION) {
                const Result<ProjectConfig> config = ProjectConfig::ReadConfig(entry.path());
                if (!config) {
                    return config.GetError();
                }

                return FromConfig(projectPath, *config);
            }
        }

        return Error::Create(EErrorIO::DOES_NOT_EXIST, "Failed to find a project file by the provided path");
    }

    Result<std::shared_ptr<Project>> Project::CreateNew(StringView projectName, const std::filesystem::path& projectPath) {
        if (projectPath.empty()) {
            return Error::Create(EErrorIO::INVALID_PATH, "Provided path is empty");
        }

        std::error_code ec;
        std::filesystem::create_directories(projectPath, ec);
        if (ec) {
            return Error::Create(EErrorIO::IO_ERROR, ec.message());
        }

        const ProjectConfig config = ProjectConfig::CreateNew(projectName, projectPath);
        const String projectConfigFileName = String::Format("{}{}", projectName, KIWI_PROJECT_EXTENSION);

        const Result<void> saveResult = File::SaveInFile(
            projectPath / projectConfigFileName.ToStdString(),
            config.ToJSON().dump(4),
            true
        );
        if (!saveResult) {
            return saveResult.GetError();
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
