#include "Renderer.hpp"

#include <renderer/IRenderContext.hpp>
#include <renderer/pipeline/RenderPipeline.hpp>
#include <renderer/GraphicDevice.hpp>

#include <misc/WindowSubsystem.hpp>



namespace Kiwi {
    bool Renderer::Init(ERenderAPI::Type renderAPI) {
        m_renderContext = IRenderContext::Create(renderAPI);
        if (!m_renderContext->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize a render context");
            return false;
        }

        return true;
    }

    void Renderer::SetViewport(const I32Rect& viewport) const {
        
    }

    void Renderer::OnFramebufferResized(U32Rect newSize) {

    }

    bool Renderer::BeginScene() {
        m_renderContext->SetClearColor({ 0.1f, 0.1f, 0.1f });
    }

    void Renderer::Render() {

    }

    void Renderer::EndScene() {

    }
}
