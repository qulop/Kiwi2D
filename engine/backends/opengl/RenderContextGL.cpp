#include "RenderContextGL.hpp"

#include <common/types/CString.hpp>
#include <common/meta/TypeTraits.hpp>
#include <common/cast/Cast.hpp>

#include <renderer/buffers/IVertexArray.hpp>

#include <glad/glad.h>
#include <glfw/glfw3.h>


namespace {
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
    bool RenderContextGL::Init() {
        auto loadResult = LoadContext();
        if (!loadResult.has_value()) {
            KIWI_CTX_LOG(ERROR, "Failed to load OpenGL context. The reason: {}",
                GetLoadErrorMessage(loadResult.error())
            );
            return false;
        }

        CreateExtensionsInfo();
        for (auto& [extensionName, isRequired, isSupported] : std::views::values(m_extensions)) {
            bool isExtensionSupported = CheckExtensionForSupport(extensionName.ToCString());

            if (!isExtensionSupported && isRequired) {
                KIWI_CTX_LOG(ERROR, "The required extension \"{}\" doesn't supported - check your OpenGL driver",
                    extensionName
                );
                return false;
            }

            isSupported = isExtensionSupported;
        }

        // 2D rendering relies on straight-alpha blending; depth testing is not needed.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_pipeline = MakeShared<ARenderPipeline>();

        return true;
    }

    ERenderAPI::Type RenderContextGL::GetUsedAPI() const {
        return ERenderAPI::OpenGL;
    }

    bool RenderContextGL::SetupDebugLayerCallback(const PFN_DebugCallback &debugCallback) {
        return true;
    }

    ARenderPipeline* RenderContextGL::GetPipeline() {
        return m_pipeline.get();
    }

    void RenderContextGL::SetViewport(i32 x, i32 y, u32 width, u32 height) {
        glViewport(x, y, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderContextGL::SetClearColor(const Vec4& color) {
        glClearColor(color.x, color.y, color.z, color.w);
    }

    void RenderContextGL::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderContextGL::DrawIndexed(const SharedPtr<IVertexArray>& vertexArray, u32 indexCount) {
        if (!vertexArray || indexCount == 0) {
            return;
        }

        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
    }

    bool RenderContextGL::CheckExtensionForSupport(const char *ext) const {
        if (!m_contextLoaded) {
            return false;
        }

        GLint numExtension = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);

        for (GLint i = 0; i < numExtension; i++) {
            auto* currExt = BasicCast::UnsafeCast<const char*>(glGetStringi(GL_EXTENSIONS, i));
            if (CString::StrCmpBool(ext, currExt)) {
                return true;
            }
        }

        return false;
    }

    std::expected<void, RenderContextGL::ELoadContextError> RenderContextGL::LoadContext() {
        if (GetLoaderVendor() != EOpenGLLoaderVendor::GLAD) KIWI_UNLIKELY {
            return std::unexpected(ELoadContextError::UNSUPPORTED_LOADER);
        }

        if (m_contextLoaded) {
            return std::unexpected(ELoadContextError::ALREADY_LOADED);
        }

        // SUPER-TODO: This is hardcoded at this moment, because we need to focus on the rendering system
        // MAKE IT CONFIGURABLE LATER!
        GLADloadproc procAddress = (GLADloadproc)&glfwGetProcAddress;
        if (!gladLoadGLLoader(procAddress)) {
            return std::unexpected(ELoadContextError::LOADER_ERROR);
        }

        m_contextLoaded = true;
        return {};
    }

    String RenderContextGL::GetLoadErrorMessage(ELoadContextError error) {
        switch (error) {
            case ELoadContextError::ALREADY_LOADED:
                return "Context already loaded";
            case ELoadContextError::UNSUPPORTED_LOADER:
                return "Unsupported loader was selected from the ELoaderVendor enum";
            case ELoadContextError::LOADER_ERROR:
                return "OpenGL load failed. Perhaps, you trying to load context before make it current?";
            default:
                return "I have no fucking idea what to write here. The fucking IDE force me to add the 'default' block...";
        }
    }

    void RenderContextGL::CreateExtensionsInfo() {
        for (auto extension : Cast<EOpenGLExtensions>::Enumerate()) {
            m_extensions[extension] = {
                .extensionName = Cast<EOpenGLExtensions>::ToString(extension).value_or("")
            };
        }

        m_extensions[EOpenGLExtensions::DEBUG_OUTPUT].isRequired = false;
    }

    constexpr RenderContextGL::EOpenGLLoaderVendor RenderContextGL::GetLoaderVendor() const {
        return EOpenGLLoaderVendor::GLAD;
    }
}
