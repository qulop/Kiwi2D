#include "Renderer.hpp"

#include <renderer/IRenderContext.hpp>
#include <renderer/pipeline/RenderPipeline.hpp>
#include <renderer/IGraphicObjectsFactory.hpp>
#include <renderer/GraphicDevice.hpp>
#include <renderer/Camera2D.hpp>
#include <renderer/Texture.hpp>

#include <renderer/shaders/Shader.hpp>
#include <renderer/shaders/ShaderCompiler.hpp>

#include <renderer/buffers/IVertexArray.hpp>
#include <renderer/buffers/IVertexBuffer.hpp>
#include <renderer/buffers/IIndexBuffer.hpp>
#include <renderer/buffers/BufferLayout.hpp>

#include <misc/WindowSubsystem.hpp>

#include <string>



namespace {
    // The built-in batch shader. It is authored in the engine's GLSL dialect
    // (#version / #stage / #endstage) so the standard preprocessor can split stages.
    // A switch over a constant index keeps sampler-array access dynamically uniform,
    // which is required for portable behaviour across GL vendors.
    KIWI_NODISCARD Kiwi::String BuildBatchShaderSource() {
        std::string source;
        source += "#version 460 core\n";

        source += "#stage vertex\n";
        source += "layout(location = 0) in vec3 a_Position;\n";
        source += "layout(location = 1) in vec4 a_Color;\n";
        source += "layout(location = 2) in vec2 a_TexCoord;\n";
        source += "layout(location = 3) in float a_TexIndex;\n";
        source += "uniform mat4 u_ViewProjection;\n";
        source += "out vec4 v_Color;\n";
        source += "out vec2 v_TexCoord;\n";
        source += "flat out float v_TexIndex;\n";
        source += "void main() {\n";
        source += "    v_Color = a_Color;\n";
        source += "    v_TexCoord = a_TexCoord;\n";
        source += "    v_TexIndex = a_TexIndex;\n";
        source += "    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);\n";
        source += "}\n";
        source += "#endstage\n";

        source += "#stage fragment\n";
        source += "layout(location = 0) out vec4 FragColor;\n";
        source += "in vec4 v_Color;\n";
        source += "in vec2 v_TexCoord;\n";
        source += "flat in float v_TexIndex;\n";
        source += "uniform sampler2D u_Textures[32];\n";
        source += "void main() {\n";
        source += "    int index = int(v_TexIndex);\n";
        source += "    vec4 sampled = vec4(1.0);\n";
        source += "    switch (index) {\n";
        for (int slot = 0; slot < 32; ++slot) {
            const std::string idx = std::to_string(slot);
            source += "        case " + idx + ": sampled = texture(u_Textures[" + idx + "], v_TexCoord); break;\n";
        }
        source += "    }\n";
        source += "    FragColor = sampled * v_Color;\n";
        source += "}\n";
        source += "#endstage\n";

        return Kiwi::String(source);
    }
}


namespace Kiwi {
    bool Renderer::Init() {
        m_factory = IGraphicObjectsFactory::Create();

        m_renderContext = IRenderContext::Create();
        if (!m_renderContext->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize a render context");
            return false;
        }

        auto windowSubsystem = GetSubsystem<WindowSubsystem>();
        KIWI_ENSURE(windowSubsystem);

        RenderPipelineInitInfo pipelineInitInfo;
        pipelineInitInfo.viewport = windowSubsystem->GetMainWindow()->GetFramebufferSizes();
        pipelineInitInfo.scissor = windowSubsystem->GetMainWindow()->GetFramebufferSizes();
        pipelineInitInfo.msaaSamplesCount = 4;

        if (!m_renderContext->GetPipeline()->Init(pipelineInitInfo)) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize a pipeline");
            return false;
        }


        // m_graphicDevice = m_factory->CreateGraphicDevice();
        // if (!m_graphicDevice->Init()) {
        //     return false;
        // }

        // m_renderPipeline = m_factory->CreateRenderPipeline();
        // if (!m_renderPipeline->Init()) {
        //     KIWI_CTX_LOG(ERROR, "Failed to initialize a render pipeline");
        //     return false;
        // }

        // m_shaderCompiler = m_factory->CreateShaderCompiler();

        if (!InitBatchResources()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize batch rendering resources");
            return false;
        }

        return true;
    }

    bool Renderer::InitBatchResources() {
        KIWI_ENSURE(m_factory);

        m_quadVertexArray = m_factory->CreateVertexArray();
        m_quadVertexBuffer = m_factory->CreateVertexBuffer(MaxVertices * sizeof(QuadVertex));

        if (m_quadVertexBuffer) {
            m_quadVertexBuffer->SetLayout({
                { EShaderDataType::Float3, "a_Position" },
                { EShaderDataType::Float4, "a_Color" },
                { EShaderDataType::Float2, "a_TexCoord" },
                { EShaderDataType::Float,  "a_TexIndex" },
            });
        }

        if (m_quadVertexArray && m_quadVertexBuffer) {
            m_quadVertexArray->AddVertexBuffer(m_quadVertexBuffer);
        }

        Vector<u32> quadIndices(MaxIndices);
        u32 offset = 0;
        for (u32 i = 0; i < MaxIndices; i += 6) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        m_quadIndexBuffer = m_factory->CreateIndexBuffer(quadIndices.data(), MaxIndices);
        if (m_quadVertexArray && m_quadIndexBuffer) {
            m_quadVertexArray->SetIndexBuffer(m_quadIndexBuffer);
        }

        // CPU-side staging buffer that batches accumulate quads into before upload.
        m_quadVertexBufferBase = new QuadVertex[MaxVertices];

        // 1x1 opaque white texture, used so solid-colour quads can share the texture path.
        u32 whitePixel = 0xffffffff;
        ImageDesc whiteDesc;
        whiteDesc.format = EImageFormat::RGBA_8;
        whiteDesc.channels = 4;
        whiteDesc.width = 1;
        whiteDesc.height = 1;
        whiteDesc.size = sizeof(whitePixel);
        whiteDesc.data = reinterpret_cast<unsigned char*>(&whitePixel);

        m_whiteTexture = ATexture2D::Create(whiteDesc);
        m_textureSlots[0] = m_whiteTexture;

        SharedPtr<AShaderCompiler> shaderCompiler = m_factory->CreateShaderCompiler();
        if (!shaderCompiler) {
            KIWI_CTX_LOG(ERROR, "Failed to create a shader compiler for the batch renderer");
            return false;
        }

        m_batchShader = shaderCompiler->CompileSource(BuildBatchShaderSource());
        if (!m_batchShader) {
            KIWI_CTX_LOG(ERROR, "Failed to compile the built-in batch shader");
            return false;
        }

        // Map each sampler in the array to its matching texture unit (u_Textures[i] -> unit i).
        std::array<i32, MaxTextureSlots> samplers{};
        for (u32 i = 0; i < MaxTextureSlots; ++i) {
            samplers[i] = static_cast<i32>(i);
        }

        m_batchShader->Bind();
        m_batchShader->SetIntVector("u_Textures", samplers.data(), static_cast<i32>(MaxTextureSlots));

        // Unit quad centred on the origin; SubmitDraw transforms these into world space.
        m_quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        m_quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        m_quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        m_quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        return true;
    }

    void Renderer::SetViewport(const I32Rect& viewport) const {
        if (!m_renderContext) {
            return;
        }

        m_renderContext->SetViewport(0, 0,
            static_cast<u32>(viewport.bottomRight.x),
            static_cast<u32>(viewport.bottomRight.y));
    }

    void Renderer::ClearScreen(const Vec4& color) const {
        if (!m_renderContext) {
            return;
        }

        m_renderContext->SetClearColor(color);
        m_renderContext->Clear();
    }

    void Renderer::BeginScene(const Camera2D& camera) {
        m_renderContext->SetClearColor(Vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
        m_renderContext->Clear();

        m_batchShader->Bind();
        m_batchShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        StartBatch();
    }

    void Renderer::EndScene() {
        Flush();
    }

    void Renderer::SubmitDraw(const glm::mat4& transform, const Vec4& color) {
        SubmitDraw(transform, nullptr, color);
    }

    void Renderer::SubmitDraw(const glm::mat4& transform, const SharedPtr<ATexture2D>& texture, const Vec4& tint) {
        if (m_quadIndexCount >= MaxIndices) {
            NextBatch();
        }

        const f32 texIndex = ResolveTextureSlot(texture);

        constexpr Vec2 texCoords[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f },
        };

        for (u32 i = 0; i < 4; ++i) {
            const glm::vec4 worldPos = transform * m_quadVertexPositions[i];

            m_quadVertexBufferPtr->position = Vec3{ worldPos.x, worldPos.y, worldPos.z };
            m_quadVertexBufferPtr->color = tint;
            m_quadVertexBufferPtr->texCoord = texCoords[i];
            m_quadVertexBufferPtr->texIndex = texIndex;
            ++m_quadVertexBufferPtr;
        }

        m_quadIndexCount += 6;
    }

    f32 Renderer::ResolveTextureSlot(const SharedPtr<ATexture2D>& texture) {
        if (!texture) {
            return 0.0f; // white texture
        }

        for (u32 i = 1; i < m_textureSlotIndex; ++i) {
            if (m_textureSlots[i] == texture) {
                return static_cast<f32>(i);
            }
        }

        if (m_textureSlotIndex >= MaxTextureSlots) {
            NextBatch();
        }

        const f32 slot = static_cast<f32>(m_textureSlotIndex);
        m_textureSlots[m_textureSlotIndex] = texture;
        ++m_textureSlotIndex;

        return slot;
    }

    void Renderer::StartBatch() {
        m_quadIndexCount = 0;
        m_quadVertexBufferPtr = m_quadVertexBufferBase;
        m_textureSlotIndex = 1;
    }

    void Renderer::NextBatch() {
        Flush();
        StartBatch();
    }

    void Renderer::Flush() {
        if (m_quadIndexCount == 0) {
            return;
        }

        const auto vertexCount = static_cast<u32>(m_quadVertexBufferPtr - m_quadVertexBufferBase);
        const u32 dataSize = vertexCount * static_cast<u32>(sizeof(QuadVertex));
        m_quadVertexBuffer->SetData(m_quadVertexBufferBase, dataSize);

        for (u32 i = 0; i < m_textureSlotIndex; ++i) {
            if (m_textureSlots[i]) {
                m_textureSlots[i]->Bind(i);
            }
        }

        m_batchShader->Bind();
        m_renderContext->DrawIndexed(m_quadVertexArray, m_quadIndexCount);
    }

    Renderer::~Renderer() {
        delete[] m_quadVertexBufferBase;
    }
}
