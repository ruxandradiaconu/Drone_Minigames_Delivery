#pragma once

#include <vector>
#include <utility>
#include "components/simple_scene.h"
#include "./transform3D.h"
#include "./labCamera.h"
#include "components/text_renderer.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

        void Init() override;

        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, bool isTeren = false);
        void Tema2::RenderSimpleMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void Tema2::CreateTree(glm::vec3 position);
        bool Tema2::VerifyCilindruCollision(glm::vec3 dronePosition, glm::vec3 treePosition, float radius, float height);
        bool Tema2::VerifyConCollision(glm::vec3 dronePosition, glm::vec3 conePosition, float baseRadius, float coneHeight);
        bool Tema2::VerifyTreeCollision(glm::vec3 dronePosition, glm::vec3 treePosition);
        void Tema2::RenderMinimap(float deltaTimeSeconds);
        void Tema2::RenderMainScene(float deltaTimeSeconds);
        glm::vec3 Tema2::GenerateRandomPosition();

    protected:
        float eliceRotation = 0;
        glm::vec3 dronePosition = glm::vec3(0, 2, 0);
        float droneRotation = 0;
        glm::vec3 cameraOffset = glm::vec3(0.0f, 2.0f, 5.0f); 
        glm::vec3 cameraPosition; 
        glm::vec3 cameraTarget;
        int treeNumber = 15;
        std::vector<std::pair<float, float>> treePositions;
        glm::vec3 boxPosition;
        glm::vec3 dropPosition;
        bool hasBox = false;
        bool isDropped = false;
        int scoreCount = 0;
        float timer = 123;
        float timeSinceBeninging = 0;
        int time;

        implemented::Camera2* camera;
        implemented::Camera2* minimapCamera;
        ViewportArea miniViewportArea;
        glm::mat4 projectionMatrix;

        gfxc::TextRenderer* textRenderer;

    };


}   // namespace m1
