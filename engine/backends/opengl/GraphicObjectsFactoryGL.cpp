#include "GraphicObjectsFactoryGL.hpp"

#include <backends/opengl/RenderContextGL.hpp>
#include <backends/opengl/shaders/ShaderCompilerGL.hpp>

#include <backends/opengl/buffers/VertexArrayGL.hpp>
#include <backends/opengl/buffers/VertexBufferGL.hpp>
#include <backends/opengl/buffers/IndexBufferGL.hpp>



namespace Kiwi::OpenGL {
    SharedPtr<AGraphicDevice> GraphicObjectsFactoryGL::CreateGraphicDevice() const {
        return nullptr;
    }

    SharedPtr<ARenderPipeline> GraphicObjectsFactoryGL::CreateRenderPipeline() const {
        return nullptr;
    }

    SharedPtr<AShaderCompiler> GraphicObjectsFactoryGL::CreateShaderCompiler() const {
        return MakeShared<ShaderCompilerGL>();
    }

    SharedPtr<IVertexArray> GraphicObjectsFactoryGL::CreateVertexArray() const {
        return MakeShared<VertexArrayGL>();
    }

    SharedPtr<IVertexBuffer> GraphicObjectsFactoryGL::CreateVertexBuffer(u32 size) const {
        return MakeShared<VertexBufferGL>(size);
    }

    SharedPtr<IIndexBuffer> GraphicObjectsFactoryGL::CreateIndexBuffer(u32* indices, u32 count) const {
        return MakeShared<IndexBufferGL>(indices, count);
    }
}
