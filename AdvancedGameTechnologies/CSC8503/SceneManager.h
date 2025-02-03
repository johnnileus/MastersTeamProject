#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include "GameObject.h"   // Base class for game objects (Enemy, Player, SimpleSphere, etc.)
#include "GameWorld.h"
#include "Vector.h"
#include "Quaternion.h"

namespace NCL {
    namespace CSC8503 {

        class SceneManager {
        public:
            // Singleton access
            static SceneManager& GetInstance() {
                static SceneManager instance;
                return instance;
            }

            /**
             * Instantiate: generate a prefab GameObject and set its position/rotation.
             * - Each GameObject script (e.g., Enemy, Player, SimpleSphere) configures its own
             *   mass, render object, physics, etc.
             *
             * @param prefab    Pre-configured GameObject template.
             * @param position  New object's position.
             * @param rotation  New object's rotation.
             * @return GameObject* Newly instantiated object.
             */
            GameObject* Instantiate(GameWorld* world, const GameObject* prefab, const Vector3& position, const Quaternion& rotation) {
                if (!prefab)
                    return nullptr;

                // Create a new instance using the copy constructor.
                GameObject* newObj = new GameObject(*prefab);

                // Set the transform properties.
                newObj->GetTransform().SetPosition(position);
                newObj->GetTransform().SetOrientation(rotation);

                // Add the new object to the scene.
                m_sceneObjects.push_back(newObj);

                world->AddGameObject(newObj);
                return newObj;
            }

            // Return a const reference to the scene objects.
            const std::vector<GameObject*>& GetSceneObjects() const {
                return m_sceneObjects;
            }

            // Update all game objects.
            void Update(float deltaTime) {
                for (auto obj : m_sceneObjects);
                    //obj->Update(deltaTime);
            }

            // Destructor: Clean up all dynamically allocated GameObjects.
            ~SceneManager() {
                for (auto obj : m_sceneObjects)
                    delete obj;
                m_sceneObjects.clear();
            }

        private:
            SceneManager() {}
            SceneManager(const SceneManager&) = delete;
            SceneManager& operator=(const SceneManager&) = delete;

            std::vector<GameObject*> m_sceneObjects;
        };

    } // namespace csc8503
} // namespace NCL

#endif // SCENEMANAGER_H
