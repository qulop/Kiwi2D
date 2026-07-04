#pragma once

#include <core/Object.hpp>

#include <nlohmann/json_fwd.hpp>


namespace Kiwi {
    class Project;
    struct ProjectCreateInfo;
    KIWI_INTERFACE ISqlDatabase;


    class ProjectSubsystem final : public ASubsystem {
        KIWI_CREATE_OBJECT(ProjectSubsystem, ASubsystem)

        static constexpr StringView PROJECTS_DB_FILE_NAME = "Projects.db";

    public:
        struct ProjectDescription {
            u64 projectID = 0;
            String projectName;
            std::filesystem::path path;
            // TODO: add preview image

            bool isCorrupted = false;
        };

        KIWI_NODISCARD static std::filesystem::path GetProjectsDatabasePath();

    public:
        explicit ProjectSubsystem(Opt<i32> activeProjectID);

        KIWI_NODISCARD bool Init() override;

        KIWI_NODISCARD Result<std::shared_ptr<Project>> CreateNewProject(const ProjectCreateInfo& createInfo) const;

        KIWI_NODISCARD std::shared_ptr<Project> GetActiveProject() const;
        KIWI_NODISCARD const std::vector<ProjectDescription>& GetAllProjects() const;

    private:
        KIWI_NODISCARD bool LoadProjectsDatabase();
        KIWI_NODISCARD bool LoadActiveProject();

    private:
        Opt<i32> m_activeProjectID;

        std::vector<ProjectDescription> m_allProjectsList;

        std::unique_ptr<ISqlDatabase> m_projectsDB;
        std::shared_ptr<Project> m_activeProject;
    };
}
