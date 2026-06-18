#pragma once

#include <renderer/buffers/IVertexArray.hpp>

#include <backends/opengl/basic/TypesGL.hpp>


namespace Kiwi::OpenGL {
    class VertexArrayGL final : public IVertexArray {
        KIWI_CREATE_OBJECT(VertexArrayGL, IVertexArray)

    public:
        VertexArrayGL();

        void Bind() const override;
        void Unbind() const override;

        void AddVertexBuffer(const SharedPtr<IVertexBuffer>& vertexBuffer) override;
        void SetIndexBuffer(const SharedPtr<IIndexBuffer>& indexBuffer) override;

        KIWI_NODISCARD const Vector<SharedPtr<IVertexBuffer>>& GetVertexBuffers() const override;
        KIWI_NODISCARD const SharedPtr<IIndexBuffer>& GetIndexBuffer() const override;

        ~VertexArrayGL() override;

    private:
        GlID m_id = KIWI_GL_UNDEFINED_ID;

        u32 m_attribIndex = 0;
        u32 m_bindingIndex = 0;

        Vector<SharedPtr<IVertexBuffer>> m_vertexBuffers;
        SharedPtr<IIndexBuffer> m_indexBuffer;
    };
}
