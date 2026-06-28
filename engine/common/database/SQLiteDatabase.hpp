#pragma once

#include <common/database/SqlDatabase.hpp>
#include <common/Definitions.hpp>

#include <sqlite/sqlite3.h>


namespace Kiwi {
    namespace EDatabaseOpenFlags {
        i32 ToSQLiteOpenFlags(Type flags);
    }



    class SQLiteDatabase : public ISqlDatabase {
    public:
        KIWI_NODISCARD static bool IsDatabaseExist(std::string_view path);

    public:
        SQLiteDatabase() = default;
        SQLiteDatabase(SQLiteDatabase&& other) = default;

        KIWI_NODISCARD Result<void> Open(std::string_view path, EDatabaseOpenFlags::Type flags) override;
        KIWI_NODISCARD bool IsOpen() const override;

        Result<std::vector<nlohmann::json>> Execute(std::string_view query) override;

        bool TableExists(std::string_view tableName) override;


        ~SQLiteDatabase() override = default;

    private:
        std::shared_ptr<sqlite3> m_connection;
    };

}
