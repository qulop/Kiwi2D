#pragma once

#include <renderer/buffers/IIndexBuffer.hpp>

#include <backends/opengl/basic/TypesGL.hpp>


namespace Kiwi::OpenGL {
    class IndexBufferGL final : public IIndexBuffer {
        KIWI_CREATE_OBJECT(IndexBufferGL, IIndexBuffer)

    public:
        IndexBufferGL(const u32* indices, u32 count);

        void Bind() const override;
        void Unbind() const override;

        KIWI_NODISCARD u32 GetCount() const override;

        KIWI_NODISCARD GlBuffer GetRendererID() const { return m_id; }

        ~IndexBufferGL() override;

    private:
        GlBuffer m_id = KIWI_GL_UNDEFINED_BUFFER;
        u32 m_count = 0;
    };
}
