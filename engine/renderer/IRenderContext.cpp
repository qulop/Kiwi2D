#include "IRenderContext.hpp"

#include <backends/opengl/RenderContextGL.hpp>


namespace Kiwi {
    SharedPtr<IRenderContext> IRenderContext::Create() {
        return MakeShared<OpenGL::RenderContextGL>();
    }
}
