#include "ProjectSubsystem.hpp"

#include <core/Project.hpp>

#include <platform/Platform.hpp>


namespace Kiwi {
    ProjectSubsystem::ProjectSubsystem(std::filesystem::path projectPath) :
        m_activeProjectPath(std::move(projectPath))
    {}

    bool ProjectSubsystem::Init() {
        if (!Super::Init()) {
            return false;
        }

        if (m_activeProjectPath.empty()) {
            m_activeProjectPath = Platform::GetApplicationPath();
        }

        if (Result<std::shared_ptr<Project>> project = Project::Open(m_activeProjectPath)) {
            m_activeProject = *project;
            return true;
        }
        else {
            KIWI_LOG(CRITICAL, "Failed to open project by the path: {}. Reason: {}",
                m_activeProjectPath.string(),
                project.GetError().GetDescription()
            );
            return false;
        }
    }

    std::shared_ptr<Project> ProjectSubsystem::GetActiveProject() const {
        KIWI_ASSERT_BASIC(m_activeProject);

        return m_activeProject;
    }
}
