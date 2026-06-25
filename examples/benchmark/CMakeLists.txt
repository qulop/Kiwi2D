# Renderer load-test (benchmark) built on the Kiwi2D engine.
#
# Configured from the root CMakeLists via add_subdirectory when KIWI_BUILD_EXAMPLES
# is ON. Links the engine static library (KiwiLib) and the third-party libraries the
# engine keeps PRIVATE (ImGui/glfw/glad), since this tool drives the ImGui overlay
# itself and queries the GL device string directly.

set(BENCHMARK_TARGET_NAME "Benchmark")

set(BENCHMARK_SRC
    src/Main.cpp
    src/BenchmarkApp.cpp

    # The engine's ImGui subsystem lives in the editor sources; compile it into the
    # example so the tool can drive the ImGui (GL3 + GLFW) overlay itself.
    ${CMAKE_SOURCE_DIR}/editor/gui/ImGuiSubsystem.cpp
)

add_executable(${BENCHMARK_TARGET_NAME} ${BENCHMARK_SRC})

target_precompile_headers(${BENCHMARK_TARGET_NAME}
    PUBLIC ${PCH_PATH}
)

target_include_directories(${BENCHMARK_TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_SOURCE_DIR}/editor
    ${CMAKE_SOURCE_DIR}/third_party/glm
    ${CMAKE_SOURCE_DIR}/third_party/ImGui
    ${CMAKE_SOURCE_DIR}/third_party/ImGui/backends
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party/xxHash
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/json/single_include
    ${CMAKE_SOURCE_DIR}/third_party/tracy/public
    ${CMAKE_SOURCE_DIR}/third_party/freetype2/include
    ${CMAKE_SOURCE_DIR}/third_party/stb
    ${CMAKE_SOURCE_DIR}/third_party/shaderc/libshaderc/include
)

target_link_libraries(${BENCHMARK_TARGET_NAME} PRIVATE
    ${ENGINE_LIBRARY_NAME}
    ImGui
    glfw
    glad
)

if (WIN32)
    set_target_properties(${BENCHMARK_TARGET_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

set_target_properties(${BENCHMARK_TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Ship the engine resources (default shaders, etc.) next to the executable.
add_custom_command(
    TARGET ${BENCHMARK_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:${BENCHMARK_TARGET_NAME}>/resources
    COMMENT "[Benchmark] Copying engine resources to the output directory..."
)

# Ship the sprite images (cat, dog, capybara) next to the executable so they resolve
# relative to the application path at runtime.
add_custom_command(
    TARGET ${BENCHMARK_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/sprites
            $<TARGET_FILE_DIR:${BENCHMARK_TARGET_NAME}>/sprites
    COMMENT "[Benchmark] Copying sprite images to the output directory..."
)
