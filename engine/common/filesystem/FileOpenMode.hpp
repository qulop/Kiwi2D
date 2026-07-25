#pragma once

#include <common/types/String.hpp>



namespace Kiwi {
    template<typename T>
    class Opt;


    namespace EFileOpenMode {
        enum Type {
            READ = (1 << 0),
            WRITE = (1 << 1),
            BINARY = (1 << 2),
            APPEND = (1 << 3),
            TRUNCATE = (1 << 4)
        };

        KIWI_NODISCARD Opt<String> ToCStyleOpenMode(Type mode);
        KIWI_NODISCARD std::ios_base::openmode ToStdOpenMode(const Type mode);
    }
    
    
    EFileOpenMode::Type operator|(EFileOpenMode::Type lhs, EFileOpenMode::Type rhs);
    EFileOpenMode::Type& operator|=(EFileOpenMode::Type& lhs, EFileOpenMode::Type rhs);


    EFileOpenMode::Type operator&(EFileOpenMode::Type lhs, EFileOpenMode::Type rhs);
    EFileOpenMode::Type& operator&=(EFileOpenMode::Type& lhs, EFileOpenMode::Type rhs);
}