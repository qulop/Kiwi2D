#pragma once

#include <common/types/String.hpp>
#include <common/types/Errors.hpp>
#include <common/types/Result.hpp>


namespace Kiwi::OpenGL {
    namespace EOpenGLExtensions {
        enum Type : u8 {
            DEBUG_OUTPUT      = KIWI_BIT(0),
            CLIP_CONTROL      = KIWI_BIT(1),
            ES2_COMPATIBILITY = KIWI_BIT(2),
            SPIRV_EXTENSIONS  = KIWI_BIT(3),
            GL_SPIRV          = KIWI_BIT(4),
         };

        String ToString(Type t);
        std::vector<Type> Enumerate();
    }


    namespace ELoadContextError {
        enum Type : u16 {
            LOADER_ERROR,
            UNSUPPORTED_LOADER,
            ALREADY_LOADED
         };

        String Describe(Type t);
    }


    struct ExtensionSupportInfo {
        String extensionName;
        bool isRequired = true;
        bool isSupported = false;
    };

    enum class EOpenGLLoaderVendor {
        GLAD, GLEW, GL3W
    };
}


namespace Kiwi::OpenGL::Context {
    KIWI_NODISCARD bool IsLoaded();
    KIWI_NODISCARD Result<void> LoadContext();

    KIWI_NODISCARD EOpenGLLoaderVendor GetLoaderVendor();

    KIWI_NODISCARD bool IsExtensionRequired(const char* ext);
    KIWI_NODISCARD bool IsExtensionSupported(const char* ext);
}


namespace Kiwi {
    template<>
    struct ErrorDescription<OpenGL::ELoadContextError::Type> {
        static constexpr StringView ERROR_CATEGORY_NAME = "RENDERER";

        KIWI_NODISCARD static constexpr String Describe(u16 t) {
            return OpenGL::ELoadContextError::Describe(static_cast<OpenGL::ELoadContextError::Type>(t));
        }
    };
}