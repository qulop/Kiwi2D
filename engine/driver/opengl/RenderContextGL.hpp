#pragma once

#include <renderer/IRenderContext.hpp>

#include <common/meta/TypeTraits.hpp>
#include <common/cast/CastTraits.hpp>


namespace Kiwi::OpenGL {
    class RenderPipelineGL;


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
    };

    struct ExtensionSupportInfo {
        String extensionName;
        bool isRequired = true;
        bool isSupported = false;
    };


    class RenderContextGL : public IRenderContext {
        KIWI_CREATE_OBJECT(RenderContextGL, IRenderContext)

    public:
        static constexpr u32 OPENGL_MAJOR_VERSION = 4;
        static constexpr u32 OPENGL_MINOR_VERSION = 6;

    public:
        enum class EOpenGLLoaderVendor {
            GLAD, GLEW, GL3W
        };

        enum class ELoadContextError {
            LOADER_ERROR,
            UNSUPPORTED_LOADER,
            ALREADY_LOADED
        };

    public:
        RenderContextGL() = default;

    public:
        KIWI_NODISCARD bool Init() override;

        KIWI_NODISCARD bool SetupDebugLayerCallback(const PFN_DebugCallback& debugCallback) override;

        KIWI_NODISCARD ERenderAPI::Type GetUsedAPI() const override;
        KIWI_NODISCARD ARenderPipeline* GetPipeline() override;

        KIWI_NODISCARD constexpr EOpenGLLoaderVendor GetLoaderVendor() const;

        KIWI_NODISCARD bool CheckExtensionForSupport(const char* ext) const;

        ~RenderContextGL() override = default;

    private:
        KIWI_NODISCARD std::expected<void, ELoadContextError> LoadContext();
        KIWI_NODISCARD String GetLoadErrorMessage(ELoadContextError error);
        void CreateExtensionsInfo();

    private:
        RenderPipelineGL* m_pipeline = nullptr;

        bool m_contextLoaded = false;
        std::unordered_map<EOpenGLExtensions::Type, ExtensionSupportInfo> m_extensions;
    };
}
