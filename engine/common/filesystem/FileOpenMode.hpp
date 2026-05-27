#pragma once

#include <common/types/String.hpp>
#include <common/cast/Cast.hpp>



namespace Kiwi {
    namespace EFileOpenMode {
        enum Type {
            READ = (1 << 0),
            WRITE = (1 << 1),
            BINARY = (1 << 2),
            APPEND = (1 << 3),
            TRUNCATE = (1 << 4)
        };


        KIWI_NODISCARD KIWI_FORCEINLINE Opt<String> ToCStyleOpenMode(const Type mode) {
            String res;

            const bool hasWrite = mode & WRITE;
            const bool hasRead = mode & READ;
            const bool hasBinary = mode & BINARY;
            const bool hasAppend = mode & APPEND;
            const bool hasTrunc = mode & TRUNCATE;

            if ((hasWrite && !hasRead) || (hasRead && !hasWrite)) {
                if ((hasRead && !hasWrite)) {
                    res += 'r';
                }
                else {
                    if (hasAppend) {
                        res += 'a';
                    }
                    else {
                        res += 'w';
                    }
                }

            }
            else if (hasWrite && hasRead) {
                if (hasAppend) {
                    res += "a+";
                }
                else if (hasTrunc) {
                    res += "w+";
                }
                else {
                    res += "r+";
                }
            }
            else {
                return nullopt;
            }


            if (hasBinary) {
                res += 'b';
            }

            return res;
        }

        KIWI_NODISCARD KIWI_FORCEINLINE std::ios_base::openmode ToStdOpenMode(const Type mode) {
            std::ios_base::openmode stdOpenMode = static_cast<std::ios_base::openmode>(0);
            if (mode & BINARY) {
                stdOpenMode |= std::ios_base::binary;
            }
            if (mode & READ) {
                stdOpenMode |= std::ios_base::in;
            }
            if (mode & WRITE) {
                stdOpenMode |= std::ios_base::out;
            }
            if (mode & APPEND) {
                stdOpenMode |= std::ios_base::ate;
            }
            if (mode & TRUNCATE) {
                stdOpenMode |= std::ios_base::trunc;
            }

            return stdOpenMode;
        }
    }
    
    
    KIWI_FORCEINLINE EFileOpenMode::Type operator|(EFileOpenMode::Type lhs, EFileOpenMode::Type rhs) {
        using T = std::underlying_type_t<EFileOpenMode::Type>;

        return static_cast<EFileOpenMode::Type>(
            static_cast<T>(lhs) | static_cast<T>(rhs)
        );
    }

    KIWI_FORCEINLINE EFileOpenMode::Type& operator|=(EFileOpenMode::Type& lhs, EFileOpenMode::Type rhs) {
        lhs = lhs | rhs;
        return lhs;
    }


    KIWI_FORCEINLINE EFileOpenMode::Type operator&(EFileOpenMode::Type lhs, EFileOpenMode::Type rhs) {
        using T = std::underlying_type_t<EFileOpenMode::Type>;

        return static_cast<EFileOpenMode::Type>(
            static_cast<T>(lhs) & static_cast<T>(rhs)
        );
    }

    KIWI_FORCEINLINE EFileOpenMode::Type& operator&=(EFileOpenMode::Type& lhs, EFileOpenMode::Type rhs) {
        lhs = lhs & rhs;
        return lhs;
    }
}