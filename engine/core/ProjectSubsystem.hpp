#pragma once

#include <core/Object.hpp>


namespace Kiwi {
    class Project;


    class ProjectSubsystem final : public ASubsystem {
        KIWI_CREATE_OBJECT(ProjectSubsystem, ASubsystem)

    public:
        explicit ProjectSubsystem(const std::filesystem::path& activeProjectPath, bool newProject);

    public:
        KIWI_NODISCARD bool Init() override;

        KIWI_NODISCARD std::shared_ptr<Project> GetActiveProject() const;

    private:
        std::shared_ptr<Project> m_activeProject;

        std::filesystem::path m_activeProjectPath;
        bool m_newProject = false;
    };
}
