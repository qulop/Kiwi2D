#include "RenderContextGL.hpp"

#include <driver/opengl/core/OpenGlContext.hpp>
#include <driver/opengl/TextureGL.hpp>
#include <driver/opengl/pipeline/RenderPipelineGL.hpp>


namespace Kiwi::OpenGL {
    bool RenderContextGL::Init() {
        Result<void> loadResult = Context::LoadContext();
        if (!loadResult) {
            KIWI_CTX_LOG(CRITICAL, "Failed to load OpenGL context. The reason: {}",
                loadResult.GetError().GetDescription()
            );
            return false;
        }

        bool allRequiredSupported = true;
        for (EOpenGLExtensions::Type extension : EOpenGLExtensions::Enumerate()) {
            ExtensionSupportInfo info = ExtensionSupportInfo();
            info.extensionName = EOpenGLExtensions::ToString(extension);
            info.isRequired = Context::IsExtensionRequired(info.extensionName.ToCString());
            info.isSupported = Context::IsExtensionSupported(info.extensionName.ToCString());

            if (!info.isSupported && info.isRequired) {
                KIWI_CTX_LOG(ERROR, "The extension \"{}\" required, but not supported - check your OpenGL driver",
                    info.extensionName
                );
                allRequiredSupported = false;
            }

            m_extensions[extension] = info;
        }

        if (!allRequiredSupported) {
            return false;
        }

        m_shaderCompiler = std::make_unique<ShaderCompilerGL>();

        return true;
    }

    ERenderAPI::Type RenderContextGL::GetUsedAPI() const {
        return ERenderAPI::OpenGL;
    }

    bool RenderContextGL::SetupDebugCallback(const PFN_DebugCallback& debugCallback) {
        return true;
    }

    void RenderContextGL::SetClearColor(const Color& color) {
        glClearColor(color.Red(), color.Green(), color.Blue(), color.Alpha());
    }

    ARenderPipeline* RenderContextGL::GetPipeline() {
        return nullptr;
    }

    TextureHandle RenderContextGL::CreateTexture(const ImageDesc& imageDesc) {
        KIWI_ENSURE(Context::IsLoaded());

        Texture2DGL* texture = Texture2DGL::Create(imageDesc);
        return m_textureHandlePool.Add(texture);
    }

    bool RenderContextGL::DestroyTexture(TextureHandle handle) {
        KIWI_ENSURE(Context::IsLoaded());

        return m_textureHandlePool.Remove(handle);
    }

    ShaderHandle RenderContextGL::CreateShader() {
        KIWI_ENSURE(Context::IsLoaded());

        return {};
    }

    bool RenderContextGL::DestroyShader(ShaderHandle handle) {
        return false;
    }
}
