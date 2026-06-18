#pragma once

#include <common/Definitions.hpp>


namespace Kiwi {
    class AWindow;


    KIWI_INTERFACE IImGuiBackend {
        virtual void Init(std::shared_ptr<AWindow> mainWindow) = 0;
        virtual void DeInit() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual ~IImGuiBackend() = default;
    };



    class ImGuiBackendOpenGL : public IImGuiBackend {
    public:
        void Init(std::shared_ptr<AWindow> mainWindow) override;
        void DeInit() override;

        void BeginFrame() override;
        void EndFrame() override;

        ~ImGuiBackendOpenGL() override = default;
    };


    class ImGuiBackendVulkan : public IImGuiBackend {
    public:
        void Init(std::shared_ptr<AWindow> mainWindow) override;
        void DeInit() override;

        void BeginFrame() override;
        void EndFrame() override;

        ~ImGuiBackendVulkan() override = default;
    };
}