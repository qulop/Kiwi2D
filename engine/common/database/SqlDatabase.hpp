#pragma once

#include <common/Definitions.hpp>
#include <common/types/Result.hpp>

#include <nlohmann/json.hpp>


namespace Kiwi {
    namespace EDatabaseOpenFlags {
        enum Type : u8 {
            AUTO = 0,
            READONLY = KIWI_BIT(0),
            READ_WRITE = KIWI_BIT(1),
            CREATE = KIWI_BIT(2),
            DELETE_ON_CLOSE = KIWI_BIT(3),
            URI = KIWI_BIT(4)
        };
    }

    constexpr EDatabaseOpenFlags::Type operator|(EDatabaseOpenFlags::Type lhs, EDatabaseOpenFlags::Type rhs) {
        using T = std::underlying_type_t<EDatabaseOpenFlags::Type>;

        return static_cast<EDatabaseOpenFlags::Type>(
            static_cast<T>(lhs) | static_cast<T>(rhs)
        );
    }

    constexpr EDatabaseOpenFlags::Type& operator|=(EDatabaseOpenFlags::Type& lhs, EDatabaseOpenFlags::Type rhs) {
        lhs = lhs | rhs;

        return lhs;
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
        KIWI_NODISCARD virtual Result<void> Open(std::string_view path, EDatabaseOpenFlags::Type flags = EDatabaseOpenFlags::AUTO) = 0;
        KIWI_NODISCARD virtual bool IsOpen() const = 0;
        KIWI_NODISCARD virtual bool TableExists(std::string_view tableName) = 0;

        KIWI_NODISCARD virtual Result<std::vector<nlohmann::json>> Execute(std::string_view query) = 0;

        virtual ~ISqlDatabase() = default;
    };
}
