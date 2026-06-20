#include "FileOpenMode.hpp"

#include <common/types/Opt.hpp>


namespace Kiwi {
    namespace EFileOpenMode {
        KIWI_NODISCARD Opt<String> ToCStyleOpenMode(const Type mode) {
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
                return ZERO_OPT;
            }


            if (hasBinary) {
                res += 'b';
            }

            return res;
        }

        KIWI_NODISCARD std::ios_base::openmode ToStdOpenMode(const Type mode) {
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


    EFileOpenMode::Type operator|(EFileOpenMode::Type lhs, EFileOpenMode::Type rhs) {
        using T = std::underlying_type_t<EFileOpenMode::Type>;

        return static_cast<EFileOpenMode::Type>(
            static_cast<T>(lhs) | static_cast<T>(rhs)
        );
    }

    EFileOpenMode::Type& operator|=(EFileOpenMode::Type& lhs, EFileOpenMode::Type rhs) {
        lhs = lhs | rhs;
        return lhs;
    }


    EFileOpenMode::Type operator&(EFileOpenMode::Type lhs, EFileOpenMode::Type rhs) {
        using T = std::underlying_type_t<EFileOpenMode::Type>;

        return static_cast<EFileOpenMode::Type>(
            static_cast<T>(lhs) & static_cast<T>(rhs)
        );
    }

    EFileOpenMode::Type& operator&=(EFileOpenMode::Type& lhs, EFileOpenMode::Type rhs) {
        lhs = lhs & rhs;
        return lhs;
    }
}