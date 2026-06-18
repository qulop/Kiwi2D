#include "VertexArrayGL.hpp"

#include <backends/opengl/buffers/VertexBufferGL.hpp>
#include <backends/opengl/buffers/IndexBufferGL.hpp>

#include <glad/glad.h>


namespace {
    using namespace Kiwi;

    KIWI_NODISCARD GLenum ShaderDataTypeToGLBaseType(EShaderDataType type) {
        switch (type) {
            case EShaderDataType::Float:
            case EShaderDataType::Float2:
            case EShaderDataType::Float3:
            case EShaderDataType::Float4:
            case EShaderDataType::Mat3:
            case EShaderDataType::Mat4:
                return GL_FLOAT;
            case EShaderDataType::Int:
            case EShaderDataType::Int2:
            case EShaderDataType::Int3:
            case EShaderDataType::Int4:
                return GL_INT;
            case EShaderDataType::Bool:
                return GL_BOOL;
            case EShaderDataType::None:
                return 0;
        }
        return 0;
    }

    KIWI_NODISCARD bool IsIntegerType(EShaderDataType type) {
        switch (type) {
            case EShaderDataType::Int:
            case EShaderDataType::Int2:
            case EShaderDataType::Int3:
            case EShaderDataType::Int4:
            case EShaderDataType::Bool:
                return true;
            default:
                return false;
        }
    }
}


namespace Kiwi::OpenGL {
    VertexArrayGL::VertexArrayGL() {
        glCreateVertexArrays(1, &m_id);
    }

    void VertexArrayGL::Bind() const {
        glBindVertexArray(m_id);
    }

    void VertexArrayGL::Unbind() const {
        glBindVertexArray(0);
    }

    void VertexArrayGL::AddVertexBuffer(const SharedPtr<IVertexBuffer>& vertexBuffer) {
        KIWI_ASSERT(vertexBuffer != nullptr, "VertexArrayGL::AddVertexBuffer received a null vertex buffer");
        KIWI_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(),
                    "Vertex buffer has no layout. Call SetLayout() before adding it to a vertex array");

        const auto* glVertexBuffer = static_cast<const VertexBufferGL*>(vertexBuffer.get());
        const BufferLayout& layout = vertexBuffer->GetLayout();
        const u32 bindingIndex = m_bindingIndex;

        glVertexArrayVertexBuffer(
            m_id,
            bindingIndex,
            glVertexBuffer->GetRendererID(),
            0,
            static_cast<GLsizei>(layout.GetStride())
        );

        for (const BufferElement& element : layout) {
            const GLenum baseType = ShaderDataTypeToGLBaseType(element.type);

            if (element.type == EShaderDataType::Mat3 || element.type == EShaderDataType::Mat4) {
                const u32 columnCount = (element.type == EShaderDataType::Mat3) ? 3 : 4;
                const u32 columnSize = columnCount * static_cast<u32>(sizeof(f32));

                for (u32 column = 0; column < columnCount; ++column) {
                    glEnableVertexArrayAttrib(m_id, m_attribIndex);
                    glVertexArrayAttribFormat(
                        m_id,
                        m_attribIndex,
                        static_cast<GLint>(columnCount),
                        baseType,
                        element.normalized ? GL_TRUE : GL_FALSE,
                        element.offset + column * columnSize
                    );
                    glVertexArrayAttribBinding(m_id, m_attribIndex, bindingIndex);
                    ++m_attribIndex;
                }
            }
            else {
                glEnableVertexArrayAttrib(m_id, m_attribIndex);

                if (IsIntegerType(element.type)) {
                    glVertexArrayAttribIFormat(
                        m_id,
                        m_attribIndex,
                        static_cast<GLint>(element.GetComponentCount()),
                        baseType,
                        element.offset
                    );
                }
                else {
                    glVertexArrayAttribFormat(
                        m_id,
                        m_attribIndex,
                        static_cast<GLint>(element.GetComponentCount()),
                        baseType,
                        element.normalized ? GL_TRUE : GL_FALSE,
                        element.offset
                    );
                }

                glVertexArrayAttribBinding(m_id, m_attribIndex, bindingIndex);
                ++m_attribIndex;
            }
        }

        ++m_bindingIndex;
        m_vertexBuffers.push_back(vertexBuffer);
    }

    void VertexArrayGL::SetIndexBuffer(const SharedPtr<IIndexBuffer>& indexBuffer) {
        KIWI_ASSERT(indexBuffer != nullptr, "VertexArrayGL::SetIndexBuffer received a null index buffer");

        const auto* glIndexBuffer = static_cast<const IndexBufferGL*>(indexBuffer.get());
        glVertexArrayElementBuffer(m_id, glIndexBuffer->GetRendererID());

        m_indexBuffer = indexBuffer;
    }

    const Vector<SharedPtr<IVertexBuffer>>& VertexArrayGL::GetVertexBuffers() const {
        return m_vertexBuffers;
    }

    const SharedPtr<IIndexBuffer>& VertexArrayGL::GetIndexBuffer() const {
        return m_indexBuffer;
    }

    VertexArrayGL::~VertexArrayGL() {
        glDeleteVertexArrays(1, &m_id);
    }
}
