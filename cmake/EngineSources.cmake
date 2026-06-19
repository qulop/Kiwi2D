set(PCH_PATH ${CMAKE_SOURCE_DIR}/engine/common/PCH.hpp)


set(ENGINE_LIBRARY_SRC
    engine/memory/Memory.cpp

    engine/core/Application.cpp
    engine/core/EngineConfig.cpp
    engine/core/Engine.cpp
    engine/core/Object.cpp
    engine/core/LogSubsystem.cpp
    engine/core/Project.cpp
    engine/core/ProjectSubsystem.cpp
    engine/core/resources/AssetManager.cpp
    engine/core/resources/AssetImporter.cpp

    engine/imgui/ImGuiSubsystem.cpp
    engine/imgui/ImGuiBackend.cpp

    engine/misc/Window.cpp
    engine/misc/ParserBase.cpp
    engine/misc/WindowSubsystem.cpp

    engine/platform/window/GLFWWindow.cpp
    engine/platform/PlatformVars.cpp

    # --- BEGIN: OpenGL driver ---
    engine/driver/opengl/RenderContextGL.cpp
    engine/driver/opengl/GraphicObjectsFactoryGL.cpp
    engine/driver/opengl/TextureGL.cpp
    engine/driver/opengl/shaders/ShaderGL.cpp
    engine/driver/opengl/shaders/ShaderCompilerGL.cpp
    # --- END ---

    # --- BEGIN: Vulkan driver ---
    engine/driver/vulkan/RenderContextVK.cpp
    engine/driver/vulkan/core/VulkanSubsystem.cpp
    engine/driver/vulkan/core/VulkanTypes.cpp
    engine/driver/vulkan/core/QueueFamilies.cpp
    engine/driver/vulkan/core/Device.cpp
    engine/driver/vulkan/core/SwapChain.cpp
    engine/driver/vulkan/core/Pipeline.cpp
    engine/driver/vulkan/core/RenderPass.cpp
    engine/driver/vulkan/pipeline/RenderPipelineVK.cpp
    engine/driver/vulkan/memory/Allocator.cpp
    engine/driver/vulkan/GraphicObjectsFactoryVK.cpp
    engine/driver/vulkan/GraphicDeviceVK.cpp
    engine/driver/vulkan/shaders/ShaderCompilerVK.cpp
    engine/driver/vulkan/shaders/ShaderVK.cpp
    # --- END ---

    # --- BEGIN: Renderer ---
    engine/renderer/Renderer.cpp
    engine/renderer/IRenderContext.cpp
    engine/renderer/IGraphicObjectsFactory.cpp
    engine/renderer/pipeline/RenderPipeline.cpp
    engine/renderer/shaders/Shader.cpp
    engine/renderer/shaders/ShaderMetaInfo.cpp
    engine/renderer/shaders/PreprocessorGLSL.cpp
    engine/renderer/shaders/SpirV.cpp
    engine/renderer/shaders/ShaderCompiler.cpp
    engine/renderer/shaders/ShaderBundle.cpp
    engine/renderer/Texture.cpp
    # --- END ---

    engine/sync/Mutex.cpp
    engine/sync/Thread.cpp

    engine/common/filesystem/File.cpp
    engine/common/filesystem/FileContent.cpp
    engine/common/meta/TypeMetaInfo.cpp
    engine/common/Time.cpp
    engine/common/ProgramOptions.cpp
    engine/common/Debug.cpp
    engine/common/UUID.cpp
    engine/common/Numeric.cpp
    engine/common/Errors.cpp
    engine/common/types/String.cpp
)

set(ENGINE_APPLICATION_SRC
    editor/Main.cpp
    editor/app/Editor.cpp
)


if(WIN32)
    list(APPEND ENGINE_LIBRARY_SRC
        engine/driver/vulkan/core/win32/ImplWin32.cpp

        engine/win32/PlatformWin32.cpp
        engine/win32/io/SystemConsoleWin32.cpp
    )
elseif(UNIX)
    list(APPEND ENGINE_LIBRARY_SRC
        engine/linux/LinuxScreen.cpp
    )
endif()

