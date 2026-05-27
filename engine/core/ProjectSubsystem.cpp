#include "ProjectSubsystem.hpp"

#include <core/Project.hpp>


namespace Kiwi {
    ProjectSubsystem::ProjectSubsystem(const std::filesystem::path& activeProjectPath, const bool newProject) :
        m_activeProjectPath(activeProjectPath),
        m_newProject(newProject)
    {}

    bool ProjectSubsystem::Init() {
        if (!Super::Init()) {
            return false;
        }

        if (!m_newProject) {
            m_activeProject = Project::Open(m_activeProjectPath);
        }
        else {
            m_activeProject = Project::CreateNew("Hui", m_activeProjectPath);
        }

        if (!m_activeProject) {
            if (m_newProject) {
                KIWI_LOG(CRITICAL, "Failed to create a new project by the path: {}", m_activeProjectPath.string());
            }
            else {
                KIWI_LOG(CRITICAL, "Failed to open a project by the path: {}", m_activeProjectPath.string());
            }

            return false;
        }

        return true;
    }

    std::shared_ptr<Project> ProjectSubsystem::GetActiveProject() const {
        KIWI_ASSERT_BASIC(m_activeProject);

        return m_activeProject;
    }
}
