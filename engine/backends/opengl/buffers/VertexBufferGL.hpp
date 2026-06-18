#pragma once

#include <renderer/buffers/IVertexBuffer.hpp>

#include <backends/opengl/basic/TypesGL.hpp>


namespace Kiwi::OpenGL {
    class VertexBufferGL final : public IVertexBuffer {
        KIWI_CREATE_OBJECT(VertexBufferGL, IVertexBuffer)

    public:
        explicit VertexBufferGL(u32 size);
        VertexBufferGL(const void* data, u32 size);

        void Bind() const override;
        void Unbind() const override;

        void SetData(const void* data, u32 size) override;

        KIWI_NODISCARD const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout) override;

        KIWI_NODISCARD GlBuffer GetRendererID() const { return m_id; }

        ~VertexBufferGL() override;

    private:
        GlBuffer m_id = KIWI_GL_UNDEFINED_BUFFER;
        BufferLayout m_layout;
    };
}
