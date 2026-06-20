#pragma once

#include <common/Definitions.hpp>
#include <common/types/Result.hpp>

#include <nlohmann/json.hpp>


namespace Kiwi {
    namespace EDatabaseOpenFlags {
        enum Type : u8 {
            READONLY,
            READ_WRITE,
            CREATE,
            DELETE_ON_CLOSE,
            URI
        };
    }

    namespace ESqlDatatype {
        enum Type : u8 {
            NULL_TYPE,
            INTEGER,
            FLOAT,
            TEXT,
            BLOB
        };
    }



    KIWI_INTERFACE ISqlDatabase {
        KIWI_NODISCARD virtual Result<void> Open(std::string_view path, EDatabaseOpenFlags::Type flags) = 0;
        KIWI_NODISCARD virtual bool IsOpen() const = 0;


        virtual Result<std::vector<nlohmann::json>> Execute(std::string_view query) = 0;

        virtual ~ISqlDatabase() = default;
    };
}
