#pragma once

#include <common/types/String.hpp>


namespace Kiwi {
    template<Concepts::Enumeration EErrorEnum>
    struct ErrorDescription;

}


namespace Kiwi::Concepts {
    template<typename T>
    concept ErrorEnumeration =
        Enumeration<T> &&
        std::same_as<std::underlying_type_t<T>, u16> &&
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

        KIWI_NODISCARD static String Describe(u16 t) {
            return EGeneralError::ToString(static_cast<EGeneralError::Type>(t));
        }
    };

    template<>
    struct ErrorDescription<EErrorIO::Type> {
        static constexpr StringView ERROR_CATEGORY_NAME = "IO";

        KIWI_NODISCARD static String Describe(u16 t) {
            return EErrorIO::ToString(static_cast<EErrorIO::Type>(t));
        }
    };



    class Error {
        using ErrorCategoryDescriptionFN = String(*)(u16);

    public:
        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD static Error Create(EErrorEnum err, Opt<String> desc = nullopt) {
            return {
                &ErrorDescription<EErrorEnum>::Describe,
                static_cast<u16>(err),
                std::move(desc)
            };
        }

    public:
        Error() = default;


        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD bool Is(EErrorEnum err) const {
            return (categoryDescriptionFn == &ErrorDescription<EErrorEnum>::Describe) && (code == static_cast<u16>(err));
        }

        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD bool InCategory() const {
            return categoryDescriptionFn == &ErrorDescription<EErrorEnum>::Describe;
        }

        template<Concepts::ErrorEnumeration EErrorEnum>
        KIWI_NODISCARD Opt<EErrorEnum> GetAsEnum() const {
            if (InCategory<EErrorEnum>()) {
                return static_cast<EErrorEnum>(code);
            }

            return nullopt;
        }

        KIWI_NODISCARD String GetDescription() const {
            if (desc) {
                return *desc;
            }

            return categoryDescriptionFn(code);
        }

        KIWI_NODISCARD bool operator==(const Error& other) const noexcept = default;

    private:
        Error(ErrorCategoryDescriptionFN categoryDescFn, u16 code, Opt<String> desc) :
            categoryDescriptionFn(categoryDescFn),
            code(code),
            desc(std::move(desc))
        {}

        ErrorCategoryDescriptionFN categoryDescriptionFn = nullptr;
        u16 code = 0;
        Opt<String> desc;
    };
}