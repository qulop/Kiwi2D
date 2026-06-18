#pragma once

#include <common/meta/TypeTraits.hpp>

#include <core/Object.hpp>

#include <math/Rect.hpp>
#include <math/vec/Vec2.hpp>
#include <math/vec/Vec3.hpp>
#include <math/vec/Vec4.hpp>

#include <glm/glm.hpp>

#include <array>


namespace Kiwi {
    KIWI_INTERFACE IGraphicObjectsFactory;
    KIWI_INTERFACE IRenderContext;
    KIWI_INTERFACE IVertexArray;
    KIWI_INTERFACE IVertexBuffer;
    KIWI_INTERFACE IIndexBuffer;
    class ARenderPipeline;
    class AShaderCompiler;
    class AShader;
    class AGraphicDevice;
    class ATexture2D;
    class Camera2D;


    struct QuadVertex {
        Vec3 position;
        Vec4 color;
        Vec2 texCoord;
        f32 texIndex = 0.0f;
    };


    class Renderer : public AObject {
        KIWI_CREATE_OBJECT(Renderer, AObject);

    public:
        static constexpr u32 MaxQuads = 10000;
        static constexpr u32 MaxVertices = MaxQuads * 4;
        static constexpr u32 MaxIndices = MaxQuads * 6;
        static constexpr u32 MaxTextureSlots = 32;

    public:
        KIWI_NODISCARD bool Init();

        void SetViewport(const I32Rect& viewport) const;

        // Clears the active framebuffer. Useful as a per-frame hook when no scene
        // is being submitted yet (BeginScene() also clears before a batch).
        void ClearScreen(const Vec4& color = Vec4{ 0.1f, 0.1f, 0.12f, 1.0f }) const;

        // Per-frame batch lifecycle. A frame consists of BeginScene() -> any number
        // of SubmitDraw() calls -> EndScene() (which flushes the final batch).
        void BeginScene(const Camera2D& camera);
        void EndScene();

        void SubmitDraw(const glm::mat4& transform, const Vec4& color);
        void SubmitDraw(const glm::mat4& transform, const SharedPtr<ATexture2D>& texture, const Vec4& tint = Vec4{ 1.0f });

        ~Renderer() override;

    private:
        KIWI_NODISCARD bool InitBatchResources();

        void StartBatch();
        void NextBatch();
        void Flush();

        KIWI_NODISCARD f32 ResolveTextureSlot(const SharedPtr<ATexture2D>& texture);

    private:
        SharedPtr<IGraphicObjectsFactory> m_factory;

        SharedPtr<IRenderContext> m_renderContext;

        SharedPtr<IVertexArray> m_quadVertexArray;
        SharedPtr<IVertexBuffer> m_quadVertexBuffer;
        SharedPtr<IIndexBuffer> m_quadIndexBuffer;

        SharedPtr<AShader> m_batchShader;
        SharedPtr<ATexture2D> m_whiteTexture;

        u32 m_quadIndexCount = 0;
        QuadVertex* m_quadVertexBufferBase = nullptr;
        QuadVertex* m_quadVertexBufferPtr = nullptr;

        std::array<SharedPtr<ATexture2D>, MaxTextureSlots> m_textureSlots;
        u32 m_textureSlotIndex = 1; // slot 0 is reserved for the white texture

        glm::vec4 m_quadVertexPositions[4];
    };
}
