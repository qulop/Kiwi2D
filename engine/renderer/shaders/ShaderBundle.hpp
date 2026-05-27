#pragma once

#include <common/UUID.hpp>

#include <renderer/shaders/ShaderStage.hpp>


namespace Kiwi {
    class AShader;



    enum class EShaderBundleFormatVersion {
        V0 = 0
    };

    struct ShaderBundleHeader {
        EShaderBundleFormatVersion version = EShaderBundleFormatVersion::V0;
        UUID uuid;
        std::vector<EShaderStage> stages;
    };


    class ShaderBundle {
    public:
        KIWI_NODISCARD static bool SerializeShader(const std::shared_ptr<AShader>& shader);
        KIWI_NODISCARD static std::shared_ptr<AShader> DeserializeShader(const std::filesystem::path& bundlePath);
    };
}