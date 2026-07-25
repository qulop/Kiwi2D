#include "RenderContextVK.hpp"

#include <core/EngineConfig.hpp>

#include <driver/vulkan/core/VulkanSubsystem.hpp>
#include <driver/vulkan/core/Device.hpp>

#include <driver/vulkan/pipeline/RenderPipelineVK.hpp>


namespace Kiwi::Vulkan {
    bool RenderContextVK::Init() {
        RegisterSubsystem<VulkanSubsystem>();

        auto vulkanSubsystem = GetSubsystem<VulkanSubsystem>();
        KIWI_ENSURE(vulkanSubsystem);

        if (!vulkanSubsystem->Init()) {
            return false;
        }

        m_pipeline = KIWI_NOTHROW_NEW RenderPipelineVK();
        return m_pipeline != nullptr;
    }

    bool RenderContextVK::SetupDebugCallback(const PFN_DebugCallback& debugCallback) {
        return true;
    }

    void RenderContextVK::SetClearColor(const Color& color) {

    }

    ERenderAPI::Type RenderContextVK::GetUsedAPI() const {
        return ERenderAPI::Vulkan;
    }

    TextureHandle RenderContextVK::CreateTexture(const ImageDesc& imageDesc) {
        return {};
    }

    bool RenderContextVK::DestroyTexture(TextureHandle handle) {
        return false;
    }

    ShaderHandle RenderContextVK::CreateShader() {
        return {};
    }

    bool RenderContextVK::DestroyShader(ShaderHandle handle) {
        return false;
    }

    ARenderPipeline* RenderContextVK::GetPipeline() {
        return m_pipeline;
    }
}
