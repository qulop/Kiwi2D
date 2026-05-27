#pragma once

#include <stb_image.h>
#include <glad/glad.h>

#include <core/Object.hpp>
#include <core/resources/Asset.hpp>

#include <common/meta/TypeTraits.hpp>
#include <common/Definitions.hpp>
#include <common/types/String.hpp>


namespace Kiwi {
    namespace EImageFormat {
        enum Type {
            NONE = 0,
            GRAYSCALE,
            RGB_8,
            RGBA_8,
        };

        constexpr Type FromChannels(const i32 channels) {
            switch (channels) {
            case 1:
                return GRAYSCALE;
            case 3:
                return RGB_8;
            case 4:
                return RGBA_8;
            default:
                return NONE;
            }
        }
    }


    struct ImageDesc {
        EImageFormat::Type format = EImageFormat::NONE;
        i32 channels = 0;
        i32 width = 0;
        i32 height = 0;
        i32 size = 0;
        unsigned char* data = nullptr;
    };



    class ATexture : public AAsset {
        KIWI_CREATE_OBJECT(ATexture, AAsset);

    public:
        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;

        ~ATexture() override = default;
    };



    class ATexture2D : public ATexture {
        KIWI_CREATE_OBJECT(ATexture2D, ATexture)

    public:
        KIWI_NODISCARD static std::shared_ptr<ATexture2D> Create(const ImageDesc& desc);


        ~ATexture2D() override = default;
    };
}