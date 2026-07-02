#include "SQLiteDatabase.hpp"

#include <common/types/Errors.hpp>
#include <common/types/String.hpp>


namespace Kiwi {
    i32 EDatabaseOpenFlags::ToSQLiteOpenFlags(Type flags) {
        i32 outFlags = 0;

        if (flags & READONLY) {
            outFlags |= SQLITE_OPEN_READONLY;
        }
        if (flags & READ_WRITE) {
            outFlags |= SQLITE_OPEN_READWRITE;
        }
        if (flags & CREATE) {
            outFlags |= SQLITE_OPEN_CREATE;
        }
        if (flags & DELETE_ON_CLOSE) {
            outFlags |= SQLITE_OPEN_DELETEONCLOSE;
        }
        if (flags & URI) {
            outFlags |= SQLITE_OPEN_URI;
        }

        return outFlags;
    }


    bool SQLiteDatabase::IsDatabaseExist(std::filesystem::path path) {
        return std::filesystem::exists(path);
    }

    Result<void> SQLiteDatabase::Open(std::filesystem::path path, EDatabaseOpenFlags::Type flags) {
        if (flags == EDatabaseOpenFlags::AUTO) {
            flags = EDatabaseOpenFlags::READ_WRITE;
            if (!IsDatabaseExist(path)) {
                flags |= EDatabaseOpenFlags::CREATE;
            }
        }

        sqlite3* connection = nullptr;
        i32 sqliteOpenFlags = EDatabaseOpenFlags::ToSQLiteOpenFlags(flags);
        if (sqlite3_open_v2(path.string().data(), &connection, sqliteOpenFlags, nullptr) != SQLITE_OK) {
            const char* errmsg = sqlite3_errmsg(connection);
            sqlite3_close(connection);

            return Error::Create(EErrorIO::IO_ERROR, errmsg);
        }

        m_connection.reset(connection, sqlite3_close);

        return Success<void>();
    }

    bool SQLiteDatabase::IsOpen() const {
        return m_connection != nullptr;
    }

    Result<std::vector<nlohmann::json>> SQLiteDatabase::Execute(std::string_view query) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_connection.get(), query.data(), -1, &stmt, nullptr) != SQLITE_OK) {
            return Error::Create(EErrorIO::IO_ERROR, sqlite3_errmsg(m_connection.get()));
        }

        const i32 columnCount = sqlite3_column_count(stmt);

        std::vector<nlohmann::json> resultRowsSet;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            nlohmann::json row;

            for (i32 i = 0; i < columnCount; i++) {
                std::string columnName = sqlite3_column_name(stmt, i);

                switch (sqlite3_column_type(stmt, i)) {
                    case SQLITE_INTEGER:
                        row[columnName] = sqlite3_column_int64(stmt, i);
                        break;
                    case SQLITE_FLOAT:
                        row[columnName] = sqlite3_column_double(stmt, i);
                        break;
                    case SQLITE_TEXT: {
                        const char* t = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                        row[columnName] = std::move(std::string(t));
                        break;
                    }
                    case SQLITE_BLOB: {
                        const u8* ptr = static_cast<const u8*>(sqlite3_column_blob(stmt, i));
                        const i32 blobLen = sqlite3_column_bytes(stmt, i);

                        row[columnName] = nlohmann::json::binary_t(
                            std::vector<u8>(ptr, ptr + blobLen)
                        );
                        break;
                    }
                    default: // null
                        row[columnName] = nullptr;
                        break;
                }
            }

            resultRowsSet.push_back(std::move(row));
        }

        sqlite3_finalize(stmt);

        return Success(resultRowsSet);
    }

    bool SQLiteDatabase::TableExists(std::string_view tableName) {
        String query = String::Format(
            "SELECT name FROM sqlite_schema WHERE type='table' AND name='{}';",
            tableName
        );
        if (Result r = Execute(query.ToStringView())) {
            return !r->empty();
        }
        else {
            return false;
        }
    }
}
