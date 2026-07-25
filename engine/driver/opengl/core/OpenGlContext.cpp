#include "OpenGlContext.hpp"

#include <driver/opengl/core/TypesGL.hpp>

#include <glfw/glfw3.h>
#include <algorithm>


namespace {
    bool g_isContextLoaded = false;

    std::array<Kiwi::StringView, 4> REQUIRED_OPENGL_EXTENSION = {
        "GL_ARB_ES2_compatibility",
        "GL_ARB_gl_spirv",
        "GL_ARB_spirv_extensions",
        "GL_ARB_clip_control"
    };

    std::array<Kiwi::StringView, 1> OPTIONAL_OPENGL_EXTENSION = {
        "GL_ARB_debug_output"
    };
}


namespace Kiwi::OpenGL {
    namespace EOpenGLExtensions {
        String ToString(Type t) {
            switch (t) {
            case DEBUG_OUTPUT:
                return "GL_ARB_debug_output";
            case CLIP_CONTROL:
                return "GL_ARB_clip_control";
            case ES2_COMPATIBILITY:
                return "GL_ARB_ES2_compatibility";
            case SPIRV_EXTENSIONS:
                return "GL_ARB_spirv_extensions";
            case GL_SPIRV:
                return "GL_ARB_gl_spirv";
            }
        }

        std::vector<Type> Enumerate() {
            return {
                DEBUG_OUTPUT,
                CLIP_CONTROL,
                ES2_COMPATIBILITY,
                SPIRV_EXTENSIONS,
                GL_SPIRV,
            };
        }
    }

    namespace ELoadContextError {
        String Describe(Type t) {
            switch (t) {
            case ALREADY_LOADED:
                return "Context already loaded";
            case UNSUPPORTED_LOADER:
                return "Unsupported loader was selected from the ELoaderVendor enum";
            default:    // LOADER_ERROR
                return "OpenGL load failed. Perhaps, you trying to load context before make it current?";
            }
        }
    }
}


namespace Kiwi::OpenGL::Context {
    bool IsLoaded() {
        return g_isContextLoaded;
    }

    Result<void> LoadContext() {
        if (g_isContextLoaded) {
            return Error::Create(ELoadContextError::ALREADY_LOADED);
        }

        if (GetLoaderVendor() != EOpenGLLoaderVendor::GLAD) KIWI_UNLIKELY {
            return Error::Create(ELoadContextError::UNSUPPORTED_LOADER);
        }

        // SUPER-TODO: This is hardcoded at this moment, because we need to focus on the rendering system
        // MAKE IT CONFIGURABLE LATER!
        GLADloadproc procAddress = (GLADloadproc)&glfwGetProcAddress;
        if (!gladLoadGLLoader(procAddress)) {
            return Error::Create(ELoadContextError::LOADER_ERROR);
        }

        g_isContextLoaded = true;
        return Success<void>();
    }

    EOpenGLLoaderVendor GetLoaderVendor() {
        return EOpenGLLoaderVendor::GLAD;
    }

    bool IsExtensionRequired(const char* ext) {
        return std::ranges::any_of(REQUIRED_OPENGL_EXTENSION, [&ext](StringView requiredExt)
        {
            return CString::StrCmpBool(requiredExt.data(), ext);
        });
    }

    bool IsExtensionSupported(const char *ext) {
        if (!g_isContextLoaded) {
            return false;
        }

        GLint numExtension = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);

        for (GLint i = 0; i < numExtension; i++) {
            auto* currExt = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
            if (CString::StrCmpBool(ext, currExt)) {
                return true;
            }
        }

        return false;
    }
}