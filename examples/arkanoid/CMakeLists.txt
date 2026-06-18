# Arkanoid example game built on the Kiwi2D engine.
#
# This target is configured from the root CMakeLists via add_subdirectory when
# KIWI_BUILD_EXAMPLES is ON. It links against the engine static library (KiwiLib)
# and the third-party libraries the engine keeps PRIVATE (ImGui/glfw/glad), since
# this example uses ImGui directly for its HUD and the GL context utilities.

set(ARKANOID_TARGET_NAME "Arkanoid")

set(ARKANOID_SRC
    src/Main.cpp
    src/ArkanoidApp.cpp

    # The engine's ImGui subsystem lives in the editor sources; compile it into the
    # example so the game can drive the ImGui (GL3 + GLFW) overlay itself.
    ${CMAKE_SOURCE_DIR}/editor/gui/ImGuiSubsystem.cpp
)

add_executable(${ARKANOID_TARGET_NAME} ${ARKANOID_SRC})

target_precompile_headers(${ARKANOID_TARGET_NAME}
    PUBLIC ${PCH_PATH}
)

target_include_directories(${ARKANOID_TARGET_NAME} PRIVATE
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

target_link_libraries(${ARKANOID_TARGET_NAME} PRIVATE
    ${ENGINE_LIBRARY_NAME}
    ImGui
    glfw
    glad
)

if (WIN32)
    set_target_properties(${ARKANOID_TARGET_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

set_target_properties(${ARKANOID_TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Ship the engine resources (default shaders, etc.) next to the executable.
add_custom_command(
    TARGET ${ARKANOID_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:${ARKANOID_TARGET_NAME}>/resources
    COMMENT "[Arkanoid] Copying engine resources to the output directory..."
)
