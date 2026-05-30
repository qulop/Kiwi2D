#pragma once

#include <core/Object.hpp>

#include <common/UUID.hpp>


namespace Kiwi {
    namespace EAssetType {
        enum Type : u8 {
            NONE = 0,
            TEXTURE_2D,
            SHADER,
        };

        KIWI_FORCEINLINE Type FromString(StringView s) {
            if (s == "texture2D") {
                return TEXTURE_2D;
            }
            if (s == "shader") {
                return SHADER;
            }
            return NONE;
        }

        KIWI_FORCEINLINE String ToString(Type t) {
            switch (t) {
            case TEXTURE_2D:
                return "texture2D";
            case SHADER:
                return "shader";
            default:
                return "none";
            }
        }

    }


    struct AssetMetaData {
        struct IniFile {
            constexpr static StringView MAIN_SECTION_NAME = "MainMeta";
            constexpr static StringView UUID_KEY_NAME = "UUID";
            constexpr static StringView ASSET_TYPE_KEY_NAME = "AssetType";
            constexpr static StringView ASSET_PATH_KEY_NAME = "AssetPath";
        };

        constexpr static StringView METADATA_EXTENSION = ".assetmeta";


        UUID assetUUID;
        EAssetType::Type type = EAssetType::NONE;
        std::filesystem::path assetPath;
    };


    class AAsset : public AObject {
        KIWI_CREATE_OBJECT(AAsset, AObject)

    public:
        KIWI_NODISCARD virtual EAssetType::Type GetAssetType() const {
            return m_assetMetaData.type;
        }

        KIWI_NODISCARD virtual const UUID& GetUUID() const {
            return m_assetMetaData.assetUUID;
        }

        KIWI_NODISCARD virtual const std::filesystem::path& GetAssetPath() const {
            return m_assetMetaData.assetPath;
        }

        ~AAsset() override = default;

    protected:
        AssetMetaData m_assetMetaData;
    };

}