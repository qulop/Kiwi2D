#pragma once

#include <core/Object.hpp>


namespace Kiwi {
    class Project;


    class ProjectSubsystem final : public ASubsystem {
        KIWI_CREATE_OBJECT(ProjectSubsystem, ASubsystem)

    public:
        explicit ProjectSubsystem(std::filesystem::path projectPath);

        KIWI_NODISCARD bool Init() override;
        KIWI_NODISCARD std::shared_ptr<Project> GetActiveProject() const;

    private:
        std::filesystem::path m_activeProjectPath;

        std::shared_ptr<Project> m_activeProject;
    };
}
