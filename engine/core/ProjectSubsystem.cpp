#include "ProjectSubsystem.hpp"

#include <complex>
#include <core/Project.hpp>

#include <common/database/SQLiteDatabase.hpp>
#include <common/filesystem/File.hpp>

#include <platform/Platform.hpp>

#include <nlohmann/json.hpp>


namespace Kiwi {
    std::filesystem::path ProjectSubsystem::GetProjectsDatabasePath() {
        return Platform::GetExecutableDirectoryPath() / PROJECTS_DB_FILE_NAME;
    }


    ProjectSubsystem::ProjectSubsystem(Opt<i32> activeProjectID) :
        m_activeProjectID(std::move(activeProjectID))
    {}

    bool ProjectSubsystem::Init() {
        if (!Super::Init()) {
            return false;
        }

        if (!LoadProjectsDatabase()) {
            return false;
        }


        if (m_activeProjectID.HasValue()) {
            if (!LoadActiveProject()) {
                return false;
            }
        }


        Result<std::vector<nlohmann::json>> allProjectsRetrieveRes = m_projectsDB->Execute("SELECT * FROM Projects");
        if (!allProjectsRetrieveRes) {
            KIWI_LOG(ERROR, "Failed to retrieve all projects from the database: {}",
                allProjectsRetrieveRes.GetError().GetDescription()
            );
            return false;
        }

        for (const nlohmann::json& project : *allProjectsRetrieveRes) {
            ProjectDescription projectDesc;
            projectDesc.projectID = project.value("Id", std::numeric_limits<u64>::max());
            projectDesc.projectName = project.value("ProjectName", std::string());
            projectDesc.path = project.value("Path", std::filesystem::path());

            if (!Project::Validate(projectDesc.path)) {
                projectDesc.isCorrupted = true;
            }

            m_allProjectsList.push_back(projectDesc);
        }

        return true;
    }

    std::shared_ptr<Project> ProjectSubsystem::GetActiveProject() const {
        KIWI_ASSERT_BASIC(m_activeProject);

        return m_activeProject;
    }

    const std::vector<ProjectSubsystem::ProjectDescription>& ProjectSubsystem::GetAllProjects() const {
        return m_allProjectsList;
    }


    bool ProjectSubsystem::LoadProjectsDatabase() {
        constexpr StringView databaseCreateQuery = KIWI_MAKE_STRING(
            CREATE TABLE IF NOT EXISTS Projects(
                Id INTEGER PRIMARY KEY NOT NULL,
                ProjectName TEXT NOT NULL,
                Path TEXT NOT NULL,
                PreviewImage BLOB
            );
        );


        m_projectsDB = std::make_unique<SQLiteDatabase>();
        if (Result<void> res = m_projectsDB->Open(GetProjectsDatabasePath())) {
            const Result<std::vector<nlohmann::json>> createRes = m_projectsDB->Execute(databaseCreateQuery);
            if (createRes) {
                return true;
            }

            KIWI_CTX_LOG(ERROR, "Failed to create projects database file: {}",
                createRes.GetError().GetDescription()
            );
            return false;
        }
        else {
            KIWI_CTX_LOG(ERROR, "Failed to open projects database file: {}",
                res.GetError().GetDescription()
            );
            return false;
        }
    }

    bool ProjectSubsystem::LoadActiveProject() {
        KIWI_ASSERT_BASIC(m_projectsDB->IsOpen());
        KIWI_ASSERT_BASIC(m_activeProjectID.HasValue() && *m_activeProjectID >= 0)

        const String pathRetrieveQuery = String::Format(
            "SELECT Path FROM Projects WHERE Id={};",
            *m_activeProjectID
        );

        if (Result<std::vector<nlohmann::json>> res = m_projectsDB->Execute(pathRetrieveQuery.ToStringView())) {
            const std::filesystem::path path = res->at(0)["Path"];

            if (path.empty()) {
                KIWI_LOG(ERROR, "An active project path successfully retrieved, but the path itself is empty");
                return false;
            }

            if (Result<std::shared_ptr<Project>> proj = Project::Open(path)) {
                m_activeProject = *proj;
                return true;
            }
            else {
                KIWI_LOG(ERROR, "Failed to open exising project: {}", proj.GetError().GetDescription());
                return false;
            }
        }
        else {
            KIWI_LOG(ERROR, "Failed to retrieve an active project path: {}",
                res.GetError().GetDescription()
            );

            return false;
        }
    }
}
