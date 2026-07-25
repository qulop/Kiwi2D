#pragma once

#include <renderer/IRenderContext.hpp>

#include <common/meta/TypeTraits.hpp>

#include <core/Handle.hpp>

#include <driver/opengl/core/OpenGlContext.hpp>
#include <driver/opengl/shaders/ShaderCompilerGL.hpp>


namespace Kiwi::OpenGL {
    class RenderPipelineGL;


    class RenderContextGL : public IRenderContext {
        KIWI_CREATE_OBJECT(RenderContextGL, IRenderContext)

    public:
        static constexpr u32 OPENGL_MAJOR_VERSION = 4;
        static constexpr u32 OPENGL_MINOR_VERSION = 6;

    public:
        enum class EOpenGLLoaderVendor {
            GLAD, GLEW, GL3W
        };

    public:
        RenderContextGL() = default;

    public:
        KIWI_NODISCARD bool Init() override;

        KIWI_NODISCARD bool SetupDebugCallback(const PFN_DebugCallback& debugCallback) override;

        void SetClearColor(const Color& color) override;

        KIWI_NODISCARD ERenderAPI::Type GetUsedAPI() const override;
        KIWI_NODISCARD ARenderPipeline* GetPipeline() override;

        KIWI_NODISCARD TextureHandle CreateTexture(const ImageDesc& imageDesc) override;
        KIWI_NODISCARD bool DestroyTexture(TextureHandle handle) override;

        KIWI_NODISCARD ShaderHandle CreateShader() override;
        KIWI_NODISCARD bool DestroyShader(ShaderHandle handle) override;

        ~RenderContextGL() override = default;

    private:
        std::unordered_map<EOpenGLExtensions::Type, ExtensionSupportInfo> m_extensions;

        std::unique_ptr<ShaderCompilerGL> m_shaderCompiler;

        HandlePool<TextureHandle> m_textureHandlePool;
    };
}
