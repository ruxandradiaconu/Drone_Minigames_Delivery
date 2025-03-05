#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/transform3D.h"
#include "lab_m1/Tema2/labCamera.h"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.5, 0.5, 0.5)),
            VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.5, 0.5, 0.5)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh("cube", vertices, indices);
    }

    {
        Shader* shader = new Shader("TemaShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    const int m = 50; 
    const int n = 50; 
    const float cellSize = 1.0f; 

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = -m/2; i <= m/2; ++i) {
        for (int j = -n/2; j <= n/2; ++j) {
            float x = (j * cellSize) ;
            float z = (i * cellSize) ;
            float y = 0.0f; 

            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 normal = glm::vec3(0, 1, 0); 
            glm::vec3 color = glm::vec3(0.2f, 0.8f, 0.2f); 

            vertices.push_back(VertexFormat(position, normal, color));
        }
    }

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            int startIndex = i * (n + 1) + j;

            indices.push_back(startIndex);
            indices.push_back(startIndex + n + 1);
            indices.push_back(startIndex + 1);

            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + n + 1);
            indices.push_back(startIndex + n + 2);
        }
    }

    CreateMesh("terrain", vertices, indices);

    Shader* terrainShader = new Shader("TerenShader");
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    terrainShader->CreateAndLink();
    shaders[terrainShader->GetName()] = terrainShader;
    std::srand(std::time(0));

    for (int i = 0; i < treeNumber; i++) {
        float x = rand() % m - (m / 2);
        float z = rand() % n - (n / 2);
        if( ! ((x >= -1 && x <= 1) || (z >= -1 && z <= 1)) )
            treePositions.push_back(make_pair(x, z));
    }

    Mesh* mesh = new Mesh("cilindru");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "cilindru.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("con");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "con.obj");
    meshes[mesh->GetMeshID()] = mesh;

    boxPosition = GenerateRandomPosition();
    dropPosition = GenerateRandomPosition();

    camera = new implemented::Camera2();
    camera->Set(glm::vec3(0, 3, 4.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(glm::radians(60.0f), window->props.aspectRatio, 0.01f, 200.0f);
    minimapCamera = new implemented::Camera2();
    cout << endl << endl << endl;

    textRenderer = new gfxc::TextRenderer(window->props.selfDir, window->GetResolution().x, window->GetResolution().y);
    textRenderer->Load(PATH_JOIN(RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);
}


Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    glBindVertexArray(0);

    CheckOpenGLError();

    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}


void Tema2::FrameStart()
{
    glClearColor(0.6, 0.9, 0.9, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

}


void Tema2::Update(float deltaTimeSeconds)
{
    glm::mat4 droneMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 1, -1));
    droneMatrix = glm::translate(droneMatrix, dronePosition);
    droneMatrix = glm::rotate(droneMatrix, droneRotation, glm::vec3(0, 1, 0));
    droneMatrix = glm::scale(droneMatrix, glm::vec3(0.7, 0.5, 0.7));

    RenderMainScene(deltaTimeSeconds);

    timeSinceBeninging += deltaTimeSeconds;
    int time = int(timer - timeSinceBeninging);
    if (time < 0)
        time = 0;
    textRenderer->RenderText("Score: ", 1100, 50, 1, glm::vec3(0, 0, 0));
    textRenderer->RenderText(std::to_string(scoreCount), 1200, 50, 1, glm::vec3(0, 0, 0));
    textRenderer->RenderText("Time: ", 1100, 80, 1, glm::vec3(0, 0, 0));
    textRenderer->RenderText(std::to_string(time), 1200, 80, 1, glm::vec3(0, 0, 0));
    textRenderer->RenderText(" s", 1230, 80, 1, glm::vec3(0, 0, 0));

    if (time == 0) {
        textRenderer->RenderText("GAME OVER!", 450, 110, 3, glm::vec3(1, 0, 0));
    }

    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    RenderMinimap(deltaTimeSeconds);

    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
    
}


void Tema2::RenderMinimap(float deltaTimeSeconds)
{
    glViewport(-50, window->GetResolution().y - 170, 300, 200);

    glm::mat4 viewMatrix = glm::lookAt( glm::vec3(0.f, 30.0f, 0.f), glm::vec3(0.f, 0.0f, 0.f), glm::vec3(0, 0, -1));
    glm::mat4 minimapProjectionMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);

    glm::mat4 terrainMatrix = glm::mat4(1);
    RenderSimpleMesh2(meshes["terrain"], shaders["TemaShader"], terrainMatrix, glm::vec3(0.2f, 0.8f, 0.2f), minimapProjectionMatrix, viewMatrix);

    glm::mat4 droneMatrix = glm::translate(glm::mat4(1), glm::vec3(dronePosition.x, 0.1f, dronePosition.z));
    droneMatrix = glm::scale(droneMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    RenderSimpleMesh2(meshes["cube"], shaders["TemaShader"], droneMatrix, glm::vec3(1.0f, 0.0f, 0.0f), minimapProjectionMatrix, viewMatrix);

    for (auto& tree : treePositions) {
        glm::mat4 treeMatrix = glm::translate(glm::mat4(1), glm::vec3(tree.first, 0.1f, tree.second));
        treeMatrix = glm::scale(treeMatrix, glm::vec3(1.5f, 0.1f, 1.5f));
        RenderSimpleMesh2(meshes["cilindru"], shaders["TemaShader"], treeMatrix, glm::vec3(0.0f, 0.5f, 0.0f), minimapProjectionMatrix, viewMatrix);
    }

    if (timer >= timeSinceBeninging) {
        glm::mat4 boxMatrix = glm::translate(glm::mat4(1), glm::vec3(boxPosition.x, 0.1f, boxPosition.z));
        boxMatrix = glm::scale(boxMatrix, glm::vec3(1.5, 0.1f, 1.5));
        RenderSimpleMesh2(meshes["cube"], shaders["TemaShader"], boxMatrix, glm::vec3(0.75, 0.25, 0.7), minimapProjectionMatrix, viewMatrix);

        glm::mat4 dropMatrix = glm::translate(glm::mat4(1), glm::vec3(dropPosition.x, 0.1f, dropPosition.z));
        dropMatrix = glm::scale(dropMatrix, glm::vec3(3.5f, 0.02f, 3.5f));
        if (hasBox == true)
            RenderSimpleMesh2(meshes["cilindru"], shaders["TemaShader"], dropMatrix, glm::vec3(0.3, 0.05, 0.25), minimapProjectionMatrix, viewMatrix);
    }

    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
}


void Tema2::RenderMainScene(float deltaTimeSeconds) {

    glm::mat4 droneMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 1, -1));
    droneMatrix = glm::translate(droneMatrix, dronePosition);
    droneMatrix = glm::rotate(droneMatrix, droneRotation, glm::vec3(0, 1, 0));
    droneMatrix = glm::scale(droneMatrix, glm::vec3(0.7, 0.5, 0.7));
    camera->Set(droneMatrix * glm::vec4(0, 3, 4.5, 1), droneMatrix[3] + glm::vec4(0, 1, 0, 1), glm::vec3(0, 1, 0));

    for (auto currPos : treePositions) {
        float x = currPos.first;
        float z = currPos.second;
        CreateTree(glm::vec3(x, 0.1, z));
    }

    glm::mat4 paralelipiped1Matrix = droneMatrix;
    paralelipiped1Matrix = glm::translate(paralelipiped1Matrix, glm::vec3(0, 0, 0));
    paralelipiped1Matrix = glm::rotate(paralelipiped1Matrix, RADIANS(40.0f), glm::vec3(0, 1, 0));
    paralelipiped1Matrix = glm::scale(paralelipiped1Matrix, glm::vec3(1.5f, 0.15f, 0.15f));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], paralelipiped1Matrix, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 paralelipiped2Matrix = droneMatrix;
    paralelipiped2Matrix = glm::translate(paralelipiped2Matrix, glm::vec3(0, 0, 0));
    paralelipiped2Matrix = glm::rotate(paralelipiped2Matrix, RADIANS(-40.f), glm::vec3(0, 1, 0));
    paralelipiped2Matrix = glm::scale(paralelipiped2Matrix, glm::vec3(1.5f, 0.15f, 0.15f));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], paralelipiped2Matrix, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 cubLeft1 = paralelipiped1Matrix;
    cubLeft1 = glm::translate(cubLeft1, glm::vec3(0.9, 2, 0));
    cubLeft1 = glm::scale(cubLeft1, glm::vec3(0.1, 1, 1));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], cubLeft1, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 cubRight1 = paralelipiped1Matrix;
    cubRight1 = glm::translate(cubRight1, glm::vec3(-0.9, 2, 0));
    cubRight1 = glm::scale(cubRight1, glm::vec3(0.1, 1, 1));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], cubRight1, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 cubLeft2 = paralelipiped2Matrix;
    cubLeft2 = glm::translate(cubLeft2, glm::vec3(-0.9, 2, 0));
    cubLeft2 = glm::scale(cubLeft2, glm::vec3(0.1, 1, 1));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], cubLeft2, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 cubRight2 = paralelipiped2Matrix;
    cubRight2 = glm::translate(cubRight2, glm::vec3(0.9, 2, 0));
    cubRight2 = glm::scale(cubRight2, glm::vec3(0.1, 1, 1));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], cubRight2, glm::vec3(0.5, 0.5, 0.5));

    eliceRotation += 10 * deltaTimeSeconds;

    glm::mat4 elice1 = cubRight1;
    elice1 = glm::translate(elice1, glm::vec3(0, 1.2, 0));
    elice1 = glm::rotate(elice1, eliceRotation, glm::vec3(0, 1, 0));
    elice1 = glm::scale(elice1, glm::vec3(2.5, 0.2, 0.2));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], elice1, glm::vec3(0, 0, 0));

    glm::mat4 elice2 = cubLeft1;
    elice2 = glm::translate(elice2, glm::vec3(0, 1.2, 0));
    elice2 = glm::rotate(elice2, eliceRotation, glm::vec3(0, 1, 0));
    elice2 = glm::scale(elice2, glm::vec3(2.5, 0.2, 0.2));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], elice2, glm::vec3(0, 0, 0));

    glm::mat4 elice3 = cubRight2;
    elice3 = glm::translate(elice3, glm::vec3(0, 1.2, 0));
    elice3 = glm::rotate(elice3, eliceRotation, glm::vec3(0, 1, 0));
    elice3 = glm::scale(elice3, glm::vec3(2.5, 0.2, 0.2));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], elice3, glm::vec3(0, 0, 0));

    glm::mat4 elice4 = cubLeft2;
    elice4 = glm::translate(elice4, glm::vec3(0, 1.2, 0));
    elice4 = glm::rotate(elice4, eliceRotation, glm::vec3(0, 1, 0));
    elice4 = glm::scale(elice4, glm::vec3(2.5, 0.2, 0.2));
    RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], elice4, glm::vec3(0, 0, 0));

    glm::mat4 terrainMatrix = glm::mat4(1);
    RenderSimpleMesh(meshes["terrain"], shaders["TerenShader"], terrainMatrix, glm::vec3(0.2f, 0.8f, 0.2f), true);

    if (timer >= timeSinceBeninging) {
        glm::mat4 boxMatrix = glm::mat4(1);
        boxMatrix = glm::translate(boxMatrix, boxPosition);
        boxMatrix = glm::scale(boxMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        if (hasBox == false) {
            if (abs(dronePosition.x - boxPosition.x) <= 1 && abs(dronePosition.z - boxPosition.z) <= 1 && (dronePosition.y <= 3))
                hasBox = true;
        }
        else {
            boxPosition = droneMatrix[3] + glm::vec4(0, -0.8f, 0, 1);
        }
        RenderSimpleMesh(meshes["cube"], shaders["TemaShader"], boxMatrix, glm::vec3(0.75, 0.25, 0.7));

        glm::mat4 dropMatrix = glm::mat4(1);
        dropMatrix = glm::translate(dropMatrix, dropPosition + glm::vec3(0, -0.3f, 0));
        dropMatrix = glm::scale(dropMatrix, glm::vec3(2, 0.02f, 2));
        if (isDropped == false) {
            if (hasBox == true) {
                if (abs(dronePosition.x - dropPosition.x) <= 2.0f && abs(dronePosition.z - dropPosition.z) <= 2.0f && (dronePosition.y <= 6)) {
                    isDropped = true;
                }
            }
        }
        else {
            boxPosition = dropPosition;
        }
        if (hasBox == true)
            RenderSimpleMesh(meshes["cilindru"], shaders["TemaShader"], dropMatrix, glm::vec3(0.3, 0.05, 0.25));

        if (hasBox == true && isDropped == true) {
            hasBox = false;
            isDropped = false;
            scoreCount++;
            //cout << "Congrats! You dropped a packet" << endl;
            //cout << "Score: " << scoreCount << endl << endl;
            boxPosition = GenerateRandomPosition();
            dropPosition = GenerateRandomPosition();
        }

    }
}


void Tema2::CreateTree(glm::vec3 position) {
    glm::mat4 treeMatrix = glm::mat4(1);
    treeMatrix = glm::translate(treeMatrix, position);
    treeMatrix = glm::scale(treeMatrix, glm::vec3(4, 4, 4));

    glm::mat4 trunchiMatrix = treeMatrix;
    trunchiMatrix = glm::translate(trunchiMatrix, glm::vec3(0, 0, 0));
    trunchiMatrix = glm::scale(trunchiMatrix, glm::vec3(0.1, 0.3, 0.1));
    RenderSimpleMesh(meshes["cilindru"], shaders["TemaShader"], trunchiMatrix, glm::vec3(0.35, 0.15, 0.05));

    glm::mat4 con1Matrix = treeMatrix;
    con1Matrix = glm::translate(con1Matrix, glm::vec3(0, 0.3, 0));
    con1Matrix = glm::scale(con1Matrix, glm::vec3(0.4, 0.4, 0.4));
    RenderSimpleMesh(meshes["con"], shaders["TemaShader"], con1Matrix, glm::vec3(0.05, 0.2, 0.01));

    glm::mat4 con2Matrix = treeMatrix;
    con2Matrix = glm::translate(con2Matrix, glm::vec3(0, 0.7, 0));
    con2Matrix = glm::scale(con2Matrix, glm::vec3(0.3, 0.4, 0.3));
    RenderSimpleMesh(meshes["con"], shaders["TemaShader"], con2Matrix, glm::vec3(0.05, 0.2, 0.01));
}


void Tema2::FrameEnd()
{
    //DrawCoordinateSystem();
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);

}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, bool isTeren)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;
    glUseProgram(shader->program);
    GLint modelLocation = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    GLint viewLocation = glGetUniformLocation(shader->GetProgramID(), "View");
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    GLint projectionLocation = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glm::mat4 projectionMatrix = this->projectionMatrix;
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    GLint loc_color = glGetUniformLocation(shader->GetProgramID(), "Object_Color");
    glUniform3fv(loc_color, 1, glm::value_ptr(color));
    GLint terrainLocation = glGetUniformLocation(shader->GetProgramID(), "isTeren");
    glUniform1i(terrainLocation, isTeren ? 1 : 0);
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::RenderSimpleMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;
    glUseProgram(shader->program);
    GLint modelLocation = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    GLint viewLocation = glGetUniformLocation(shader->GetProgramID(), "View");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    GLint projectionLocation = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    GLint loc_color = glGetUniformLocation(shader->GetProgramID(), "Object_Color");
    glUniform3fv(loc_color, 1, glm::value_ptr(color));
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


bool Tema2::VerifyCilindruCollision(glm::vec3 dronePosition, glm::vec3 treePosition, float radius, float cilindru_height) {
    float dist = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(treePosition.x, treePosition.z));
    return dist < radius && dronePosition.y < cilindru_height;
}


bool Tema2::VerifyConCollision(glm::vec3 dronePosition, glm::vec3 conePosition, float baseRadius, float coneHeight) {
    float radiusAtHeight = baseRadius;
    float dist = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(conePosition.x, conePosition.z));
    return dronePosition.y > conePosition.y && dronePosition.y < conePosition.y + coneHeight && dist < baseRadius + 2.2;
}


bool Tema2::VerifyTreeCollision(glm::vec3 dronePosition, glm::vec3 treePosition) {
    return VerifyCilindruCollision(dronePosition, treePosition, 1.8, 0.7) || 
        VerifyConCollision(dronePosition, treePosition - glm::vec3(0, 0.3, 0), 0.7, 1.7) || 
        VerifyConCollision(dronePosition, treePosition + glm::vec3(0, 1.3, 0), 0.4, 4);
}


glm::vec3 Tema2::GenerateRandomPosition() {
    glm::vec3 position;
    bool validPosition;

    do {
        validPosition = true;

        float x = rand() % 50 - 25;
        float z = rand() % 50 - 25;
        position = glm::vec3(x, 0.8f, z);

        for (auto& tree : treePositions) {
            float distance = glm::distance(glm::vec2(position.x, position.z), glm::vec2(tree.first, tree.second));
            if (distance < 3.0f) {
                validPosition = false; 
                break;
            }
        }

    } while (!validPosition);

    return position;
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{

    glm::vec3 forwardDirection = glm::vec3(sin(droneRotation), 0, cos(droneRotation));
    glm::vec3 rightDirection = glm::vec3(-cos(droneRotation), 0, sin(droneRotation));

    if (window->KeyHold(GLFW_KEY_W)) {
        dronePosition -= forwardDirection * 5.0f * deltaTime;
        for (auto pos : treePositions) {
            glm::vec3 treePos = glm::vec3(pos.first, 0.1f, pos.second);
            if (VerifyTreeCollision(dronePosition, treePos))
                dronePosition += forwardDirection * 5.0f * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        dronePosition += forwardDirection * 5.0f * deltaTime;
        for (auto pos : treePositions) {
            glm::vec3 treePos = glm::vec3(pos.first, 0.1f, pos.second);
            if (VerifyTreeCollision(dronePosition, treePos))
                dronePosition -= forwardDirection * 5.0f * deltaTime;
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        dronePosition += rightDirection * 5.0f * deltaTime;
        for (auto pos : treePositions) {
            glm::vec3 treePos = glm::vec3(pos.first, 0.1f, pos.second);
            if (VerifyTreeCollision(dronePosition, treePos))
                dronePosition -= rightDirection * 5.0f * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        dronePosition -= rightDirection * 5.0f * deltaTime;
        for (auto pos : treePositions) {
            glm::vec3 treePos = glm::vec3(pos.first, 0.1f, pos.second);
            if (VerifyTreeCollision(dronePosition, treePos))
                dronePosition += rightDirection * 5.0f * deltaTime;
        }
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        dronePosition.y += 5 * deltaTime;
        for (auto pos : treePositions) {
            glm::vec3 treePos = glm::vec3(pos.first, 0.1f, pos.second);
            if (VerifyTreeCollision(dronePosition, treePos))
                dronePosition.y -= 5.0f * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_E) && dronePosition.y >= -0.4) {
        dronePosition.y -= 5 * deltaTime;
        for (auto pos : treePositions) {
            glm::vec3 treePos = glm::vec3(pos.first, 0.1f, pos.second);
            if (VerifyTreeCollision(dronePosition, treePos))
                dronePosition.y += 5.0f * deltaTime;
        }
    }
    
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        droneRotation += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        droneRotation -= deltaTime;
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
