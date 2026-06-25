#include "SceneSerializer.hpp"

#include <scene/Scene.hpp>
#include <scene/Entity.hpp>
#include <scene/components/Transform2D.hpp>
#include <scene/components/SpriteRenderer.hpp>
#include <scene/components/RigidBody2D.hpp>

#include <common/types/String.hpp>

#include <nlohmann/json.hpp>

#include <fstream>


namespace Kiwi {
    namespace {
        nlohmann::json SerializeEntity(const Entity& entity) {
            nlohmann::json entityJson;
            entityJson["uuid"] = entity.GetUUID().ToString().ToStdString();
            entityJson["name"] = entity.GetName();

            if (const Transform2D* transform = entity.GetComponent<Transform2D>()) {
                entityJson["transform2D"] = {
                    { "position", { transform->position.x, transform->position.y } },
                    { "rotation", transform->rotation },
                    { "scale",    { transform->scale.x, transform->scale.y } }
                };
            }

            if (const SpriteRenderer* sprite = entity.GetComponent<SpriteRenderer>()) {
                nlohmann::json spriteJson;
                spriteJson["color"] = {
                    sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a
                };

                if (sprite->texture) {
                    spriteJson["texture"] = sprite->texture->GetUUID().ToString().ToStdString();
                }

                entityJson["spriteRenderer"] = std::move(spriteJson);
            }

            if (const RigidBody2D* body = entity.GetComponent<RigidBody2D>()) {
                entityJson["rigidBody2D"] = {
                    { "type",            static_cast<u8>(body->type) },
                    { "velocity",        { body->velocity.x, body->velocity.y } },
                    { "angularVelocity", body->angularVelocity },
                    { "gravityScale",    body->gravityScale },
                    { "linearDamping",   body->linearDamping },
                    { "angularDamping",  body->angularDamping },
                    { "fixedRotation",   body->fixedRotation },
                    { "mass",            body->GetMass() },
                    { "inertia",         body->GetInertia() }
                };
            }

            return entityJson;
        }

        void DeserializeEntity(const nlohmann::json& entityJson, Scene& scene) {
            const std::string name = entityJson.value("name", std::string{ "Entity" });

            UUID uuid = UUID::Generate();
            if (entityJson.contains("uuid")) {
                const std::string uuidStr = entityJson.at("uuid").get<std::string>();
                if (Opt<UUID> parsed = UUID::FromString(String{ uuidStr })) {
                    uuid = *parsed;
                }
            }

            Entity* entity = scene.CreateEntityWithUUID(uuid, name);

            if (entityJson.contains("transform2D")) {
                const nlohmann::json& transformJson = entityJson.at("transform2D");
                Transform2D* transform = entity->AddComponent<Transform2D>();

                const auto& position = transformJson.at("position");
                const auto& scale = transformJson.at("scale");

                transform->position = Vec2{ position.at(0).get<f32>(), position.at(1).get<f32>() };
                transform->rotation = transformJson.at("rotation").get<f32>();
                transform->scale = Vec2{ scale.at(0).get<f32>(), scale.at(1).get<f32>() };
            }

            if (entityJson.contains("spriteRenderer")) {
                const nlohmann::json& spriteJson = entityJson.at("spriteRenderer");
                SpriteRenderer* sprite = entity->AddComponent<SpriteRenderer>();

                const auto& color = spriteJson.at("color");
                sprite->color = Vec4{
                    color.at(0).get<f32>(), color.at(1).get<f32>(),
                    color.at(2).get<f32>(), color.at(3).get<f32>()
                };
                // Texture resolution by UUID requires an AssetManager lookup at load time;
                // the serialized texture UUID is preserved for the loader to bind later.
            }

            if (entityJson.contains("rigidBody2D")) {
                const nlohmann::json& bodyJson = entityJson.at("rigidBody2D");
                RigidBody2D* body = entity->AddComponent<RigidBody2D>();

                const auto& velocity = bodyJson.at("velocity");
                body->type = static_cast<EBodyType>(bodyJson.at("type").get<u8>());
                body->velocity = Vec2{ velocity.at(0).get<f32>(), velocity.at(1).get<f32>() };
                body->angularVelocity = bodyJson.at("angularVelocity").get<f32>();
                body->gravityScale = bodyJson.at("gravityScale").get<f32>();
                body->linearDamping = bodyJson.at("linearDamping").get<f32>();
                body->angularDamping = bodyJson.at("angularDamping").get<f32>();
                body->fixedRotation = bodyJson.at("fixedRotation").get<bool>();
                body->SetMass(bodyJson.at("mass").get<f32>());
                body->SetInertia(bodyJson.at("inertia").get<f32>());
            }
        }
    }


    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) :
        m_scene(scene)
    {}

    void SceneSerializer::Serialize(const std::string& filepath) {
        if (!m_scene) {
            KIWI_CTX_LOG(ERROR, "Cannot serialize a null scene");
            return;
        }

        nlohmann::json sceneJson;
        sceneJson["entities"] = nlohmann::json::array();

        for (const std::shared_ptr<Entity>& entity : m_scene->GetEntities()) {
            sceneJson["entities"].push_back(SerializeEntity(*entity));
        }

        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            KIWI_CTX_LOG(ERROR, "Failed to open file for scene serialization: {}", filepath);
            return;
        }

        outFile << sceneJson.dump(4);
    }

    bool SceneSerializer::Deserialize(const std::string& filepath) {
        if (!m_scene) {
            KIWI_CTX_LOG(ERROR, "Cannot deserialize into a null scene");
            return false;
        }

        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            KIWI_CTX_LOG(ERROR, "Failed to open file for scene deserialization: {}", filepath);
            return false;
        }

        nlohmann::json sceneJson;
        try {
            inFile >> sceneJson;
        }
        catch (const nlohmann::json::parse_error& e) {
            KIWI_CTX_LOG(ERROR, "Failed to parse scene file {}. The reason: {}", filepath, e.what());
            return false;
        }

        if (!sceneJson.contains("entities") || !sceneJson.at("entities").is_array()) {
            KIWI_CTX_LOG(ERROR, "Scene file {} has no valid 'entities' array", filepath);
            return false;
        }

        for (const nlohmann::json& entityJson : sceneJson.at("entities")) {
            DeserializeEntity(entityJson, *m_scene);
        }

        return true;
    }
}
