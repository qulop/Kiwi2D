/*
    Bundle format(header):
    ```
        KIWI SBUNDL V0 <number-of-stages-in-bundle> \n
        <original-sourc e-UUID> \n
        <stage-letter>: <offset-from-file-begin> <bytecode-size> \n
        ...
    ```
*/


#include "ShaderBundle.hpp"

#include <common/filesystem/File.hpp>


namespace {
    constexpr char BUNDLE_HEADER_BEGIN[] = "KIWI SBUNDL";
}



namespace Kiwi {
    bool ShaderBundle::SerializeShader(const std::shared_ptr<AShader>& shader) {
        if (!shader) {
            return false;
        }

        // Serialization needs backend bytecode extraction support.
        // Path ownership is now delegated to Project/ShaderCacheManager.
        return false;
    }

    std::shared_ptr<AShader> ShaderBundle::DeserializeShader(const std::filesystem::path& bundlePath) {
        if (!std::filesystem::exists(bundlePath)) {
            return nullptr;
        }

        Result<FileContent> result = File::LoadFromFile(bundlePath, EFileOpenMode::READ | EFileOpenMode::BINARY);

        const FileContent* fileContent = result.GetValuePtr();
        if (!fileContent) {
            return nullptr;
        }

        const String sourceData = fileContent->GetAsString();
        if (!sourceData.StartsWith(BUNDLE_HEADER_BEGIN)) {
            return nullptr;
        }

        return nullptr;
    }
}
