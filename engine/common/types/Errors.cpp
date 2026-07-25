#include "Errors.hpp"


namespace Kiwi {
    namespace EGeneralError {
        KIWI_NODISCARD String ToString(Type t) {
            switch (t) {
            case INITIALIZE_FAILED:
                return "INITIALIZE_FAILED";
            case CREATION_FAILED:
                return "CREATION_FAILED";
            case INCOMPLETE:
                return "INCOMPLETE";
            case UNSUPPORTED:
                return "UNSUPPORTED";
            case OUT_OF_MEMORY:
                return "OUT_OF_MEMORY";
            case OUT_OF_RANGE:
                return "OUT_OF_RANGE";
            case BUFF_OVERFLOW:
                return "BUFF_OVERFLOW";
            case ALLOC_FAILED:
                return "ALLOC_FAILED";
            case INVALID_ARGUMENT:
                return "INVALID_ARGUMENT";
            case INVALID_FORMAT:
                return "INVALID_FORMAT";
            case NOT_FOUND:
                return "NOT_FOUND";
            case ALREADY_EXISTS:
                return "ALREADY_EXISTS";
            case TIMEOUT:
                return "TIMEOUT";
            case COMPILE_ERROR:
                return "COMPILE_ERROR";
            case PARSE_ERROR:
                return "PARSE_ERROR";
            default:
                return "UNKNOWN";
            }
        }
    }


    namespace EErrorIO {
        String ToString(Type t) {
            switch (t) {
            case IO_ERROR:
                return "IO_ERROR";
            case DOES_NOT_EXIST:
                return "DOES_NOT_EXIST";
            case INVALID_PATH:
                return "INVALID_PATH";
            case IS_DIRECTORY:
                return "IS_DIRECTORY";
            case NOT_A_FILE:
                return "NOT_A_FILE";
            case PERMISSION_DENIED:
                return "PERMISSION_DENIED";
            case ACCESS_DENIED:
                return "ACCESS_DENIED";
            case TOO_MANY_OPEN_FILES:
                return "TOO_MANY_OPEN_FILES";
            case NO_SPACE_LEFT:
                return "NO_SPACE_LEFT";
            case INVALID_ARGUMENT:
                return "INVALID_ARGUMENT";
            case UNEXPECTED_EOF:
                return "UNEXPECTED_EOF";
            default:
                return "UNKNOWN";
            }
        }

        KIWI_NODISCARD Type FromPosixCode(errno_t v) {
            switch (v) {
            case ENOENT:
                return DOES_NOT_EXIST;
            case EACCES:
                return PERMISSION_DENIED;
            case EINVAL:
                return INVALID_ARGUMENT;
            case EMFILE:
                return TOO_MANY_OPEN_FILES;
            case EISDIR:
                return IS_DIRECTORY;
            case ENOSPC:
                return NO_SPACE_LEFT;
            case EIO:
                return IO_ERROR;
            default:
                return UNKNOWN;
            }
        }
    }
}
