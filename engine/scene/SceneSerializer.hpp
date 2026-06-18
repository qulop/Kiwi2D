#pragma once

#include <core/Object.hpp>

#include <memory>
#include <string>


namespace Kiwi {
    class Scene;


    class SceneSerializer : public AObject {
        KIWI_CREATE_OBJECT(SceneSerializer, AObject);

    public:
        explicit SceneSerializer(const std::shared_ptr<Scene>& scene);
        ~SceneSerializer() override = default;

        void Serialize(const std::string& filepath);
        KIWI_NODISCARD bool Deserialize(const std::string& filepath);

    private:
        std::shared_ptr<Scene> m_scene;
    };
}
