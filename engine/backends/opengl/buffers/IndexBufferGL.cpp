#include "IndexBufferGL.hpp"

#include <glad/glad.h>


namespace Kiwi::OpenGL {
    IndexBufferGL::IndexBufferGL(const u32* indices, u32 count) :
        m_count(count)
    {
        glCreateBuffers(1, &m_id);
        glNamedBufferData(
            m_id,
            static_cast<GLsizeiptr>(count) * static_cast<GLsizeiptr>(sizeof(u32)),
            indices,
            GL_STATIC_DRAW
        );
    }

    void IndexBufferGL::Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    }

    void IndexBufferGL::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    u32 IndexBufferGL::GetCount() const {
        return m_count;
    }

    IndexBufferGL::~IndexBufferGL() {
        glDeleteBuffers(1, &m_id);
    }
}
