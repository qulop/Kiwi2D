#include "VertexBufferGL.hpp"

#include <glad/glad.h>


namespace Kiwi::OpenGL {
    VertexBufferGL::VertexBufferGL(u32 size) {
        glCreateBuffers(1, &m_id);
        glNamedBufferData(m_id, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBufferGL::VertexBufferGL(const void* data, u32 size) {
        glCreateBuffers(1, &m_id);
        glNamedBufferData(m_id, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
    }

    void VertexBufferGL::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }

    void VertexBufferGL::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferGL::SetData(const void* data, u32 size) {
        glNamedBufferSubData(m_id, 0, static_cast<GLsizeiptr>(size), data);
    }

    const BufferLayout& VertexBufferGL::GetLayout() const {
        return m_layout;
    }

    void VertexBufferGL::SetLayout(const BufferLayout& layout) {
        m_layout = layout;
    }

    VertexBufferGL::~VertexBufferGL() {
        glDeleteBuffers(1, &m_id);
    }
}
