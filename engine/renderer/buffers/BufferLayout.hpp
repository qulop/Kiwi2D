#pragma once

#include <common/meta/TypeTraits.hpp>
#include <common/types/String.hpp>


namespace Kiwi {
    enum class EShaderDataType : u8 {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };


    KIWI_NODISCARD constexpr u32 ShaderDataTypeSize(EShaderDataType type) {
        switch (type) {
            case EShaderDataType::Float:    return 4;
            case EShaderDataType::Float2:   return 4 * 2;
            case EShaderDataType::Float3:   return 4 * 3;
            case EShaderDataType::Float4:   return 4 * 4;
            case EShaderDataType::Mat3:     return 4 * 3 * 3;
            case EShaderDataType::Mat4:     return 4 * 4 * 4;
            case EShaderDataType::Int:      return 4;
            case EShaderDataType::Int2:     return 4 * 2;
            case EShaderDataType::Int3:     return 4 * 3;
            case EShaderDataType::Int4:     return 4 * 4;
            case EShaderDataType::Bool:     return 1;
            case EShaderDataType::None:     return 0;
        }
        return 0;
    }


    KIWI_NODISCARD constexpr u32 ComponentCountOf(EShaderDataType type) {
        switch (type) {
            case EShaderDataType::Float:    return 1;
            case EShaderDataType::Float2:   return 2;
            case EShaderDataType::Float3:   return 3;
            case EShaderDataType::Float4:   return 4;
            case EShaderDataType::Mat3:     return 3 * 3;
            case EShaderDataType::Mat4:     return 4 * 4;
            case EShaderDataType::Int:      return 1;
            case EShaderDataType::Int2:     return 2;
            case EShaderDataType::Int3:     return 3;
            case EShaderDataType::Int4:     return 4;
            case EShaderDataType::Bool:     return 1;
            case EShaderDataType::None:     return 0;
        }
        return 0;
    }


    struct BufferElement {
        String name;
        EShaderDataType type = EShaderDataType::None;
        u32 size = 0;
        u32 offset = 0;
        bool normalized = false;

        BufferElement() = default;

        BufferElement(EShaderDataType elementType, const String& elementName, bool isNormalized = false) :
            name(elementName),
            type(elementType),
            size(ShaderDataTypeSize(elementType)),
            offset(0),
            normalized(isNormalized)
        {}

        KIWI_NODISCARD u32 GetComponentCount() const {
            return ComponentCountOf(type);
        }
    };


    class BufferLayout final {
    public:
        BufferLayout() = default;

        BufferLayout(std::initializer_list<BufferElement> elements) :
            m_elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        KIWI_NODISCARD u32 GetStride() const { return m_stride; }
        KIWI_NODISCARD const Vector<BufferElement>& GetElements() const { return m_elements; }

        KIWI_NODISCARD Vector<BufferElement>::iterator begin() { return m_elements.begin(); }
        KIWI_NODISCARD Vector<BufferElement>::iterator end() { return m_elements.end(); }
        KIWI_NODISCARD Vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
        KIWI_NODISCARD Vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

    private:
        void CalculateOffsetsAndStride() {
            u32 offset = 0;
            m_stride = 0;

            for (auto& element : m_elements) {
                element.offset = offset;
                offset += element.size;
                m_stride += element.size;
            }
        }

    private:
        Vector<BufferElement> m_elements;
        u32 m_stride = 0;
    };
}
