#pragma once

#include <common/types/String.hpp>
#include <common/types/Opt.hpp>


namespace Kiwi {
    template<Concepts::Enumeration EErrorEnum>
    struct ErrorDescription;
}


namespace Kiwi::Concepts {
    template<typename T>
    concept ErrorEnumeration =
        Enumeration<T> &&
        requires(T e) {
            { ErrorDescription<T>::Describe(e) } -> SameAs<String>;
        };
}


namespace Kiwi {
    namespace EGeneralError {
        enum Type : u16 {
            UNKNOWN = 0,

            // Initialization & State
            INITIALIZE_FAILED,
            CREATION_FAILED,
            INCOMPLETE,
            UNSUPPORTED,

            // Memory & Data
            OUT_OF_MEMORY,
            OUT_OF_RANGE,
            BUFF_OVERFLOW,
            ALLOC_FAILED,

            // Arguments & Input
            INVALID_ARGUMENT,
            INVALID_FORMAT,

            // Resources & IO
            NOT_FOUND,
            ALREADY_EXISTS,
            TIMEOUT,
            COMPILE_ERROR,
            PARSE_ERROR,
        };


        KIWI_NODISCARD String ToString(Type t);
    }


    namespace EErrorIO {
        enum Type : u16 {
            // Core states
            UNKNOWN = 0,
            IO_ERROR,

            // Path
            DOES_NOT_EXIST,
            INVALID_PATH,
            IS_DIRECTORY,
            NOT_A_FILE,

            // Access & Permission
            PERMISSION_DENIED,
            ACCESS_DENIED,

            // Resource limits
            TOO_MANY_OPEN_FILES,
            NO_SPACE_LEFT,

            // Data
            INVALID_ARGUMENT,
            UNEXPECTED_EOF,
        };


        KIWI_NODISCARD String ToString(Type t);
        KIWI_NODISCARD Type FromPosixCode(errno_t v);
    }


    template<>
    struct ErrorDescription<EGeneralError::Type> {
        static constexpr StringView ERROR_CATEGORY_NAME = "General";

        KIWI_NODISCARD static constexpr String Describe(u16 t) {
            return EGeneralError::ToString(static_cast<EGeneralError::Type>(t));
        }
    };

    template<>
    struct ErrorDescription<EErrorIO::Type> {
        static constexpr StringView ERROR_CATEGORY_NAME = "IO";

        KIWI_NODISCARD static constexpr String Describe(u16 t) {
            return EErrorIO::ToString(static_cast<EErrorIO::Type>(t));
        }
    };



    class Error {
        using PFN_ErrorCategoryDescription = String(*)(u16);

    public:
        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD static constexpr Error Create(EErrorEnum err, Opt<String> desc = ZERO_OPT) {
            return {
                &ErrorDescription<EErrorEnum>::Describe,
                static_cast<u16>(err),
                std::move(desc)
            };
        }

    public:
        constexpr Error() = default;


        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD constexpr bool Is(EErrorEnum err) const {
            return (m_categoryDescriptionFn == &ErrorDescription<EErrorEnum>::Describe) && (m_code == static_cast<u16>(err));
        }

        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD constexpr bool InCategory() const {
            return m_categoryDescriptionFn == &ErrorDescription<EErrorEnum>::Describe;
        }

        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD constexpr Opt<EErrorEnum> GetAsEnum() const {
            if (InCategory<EErrorEnum>()) {
                return static_cast<EErrorEnum>(m_code);
            }

            return ZERO_OPT;
        }

        KIWI_NODISCARD constexpr String GetDescription() const {
            if (m_desc) {
                return *m_desc;
            }

            return m_categoryDescriptionFn(m_code);
        }

        KIWI_NODISCARD constexpr bool operator==(const Error& other) const noexcept {
            return (m_code == other.m_code)
                && (m_categoryDescriptionFn == other.m_categoryDescriptionFn)
                && (m_desc == other.m_desc);
        }

    private:
        constexpr Error(PFN_ErrorCategoryDescription categoryDescFn, u16 code, Opt<String> desc) :
            m_categoryDescriptionFn(categoryDescFn),
            m_code(code),
            m_desc(std::move(desc))
        {}

        PFN_ErrorCategoryDescription m_categoryDescriptionFn = nullptr;
        u16 m_code = 0;
        Opt<String> m_desc;
    };
}