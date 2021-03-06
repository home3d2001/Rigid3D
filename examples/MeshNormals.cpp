#include "MeshNormals.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <ctime>
#include <array>

using Rigid3D::cotangent;
using Rigid3D::degreesToRadians;

using namespace std;

int main() {
    shared_ptr<GlfwOpenGlWindow> meshDemo = MeshNormals::getInstance();
    meshDemo->create(800, 600, "Load Multiple Mesh Objects Example");

    return 0;
}

//---------------------------------------------------------------------------------------
shared_ptr<GlfwOpenGlWindow> MeshNormals::getInstance() {
    static GlfwOpenGlWindow * instance = new MeshNormals();
    if (p_instance == nullptr) {
        p_instance = shared_ptr<GlfwOpenGlWindow>(instance);
    }

    return p_instance;
}

//---------------------------------------------------------------------------------------
MeshNormals::MeshNormals()
    : renderTarget(MeshType::FLAT),
      vao(0),
      vbo_vertices(0),
      vbo_normals(0) {

}
//---------------------------------------------------------------------------------------
/*
 * Called after the window and OpenGL are initialized. Called exactly once,
 * before the main loop.
 */
void MeshNormals::init()
{
    meshFlat.fromObjFile("../data/meshes/bunny_smooth.obj");
    meshSmooth.fromObjFile("../data/meshes/wall.obj");

    setupShaders();
    setupGLBuffers();
    setupMatrices();

    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
}
//---------------------------------------------------------------------------------------

void MeshNormals::setupGLBuffers()
{
    //-- Concatenate vertex data from all meshes.
    size_t totalVertexBytes = meshFlat.getNumVertexPositionBytes() + meshSmooth.getNumVertexPositionBytes();

    float * vertexDataPtr = (float *)malloc(totalVertexBytes);
    float * data = vertexDataPtr;
    memcpy(data, meshFlat.getVertexPositionDataPtr(), meshFlat.getNumVertexPositionBytes());
    data += meshFlat.getNumVertexPositionBytes() / sizeof(float);
    memcpy(data, meshSmooth.getVertexPositionDataPtr(), meshSmooth.getNumVertexPositionBytes());

    //-- Concatenate normal data from all meshFlates.
    size_t totalNormalBytes = meshFlat.getNumVertexNormalBytes() + meshSmooth.getNumVertexNormalBytes();

    float * normalDataPtr = (float *)malloc(totalNormalBytes);
    data = normalDataPtr;
    memcpy(data, meshFlat.getVertexNormalDataPtr(), meshFlat.getNumVertexNormalBytes());
    data += meshFlat.getNumVertexNormalBytes() / sizeof(float);
    memcpy(data, meshSmooth.getVertexNormalDataPtr(), meshSmooth.getNumVertexNormalBytes());

    // Register vertex positions with OpenGL
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, totalVertexBytes, vertexDataPtr, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderProgram.getAttribLocation("vertexPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Register normals with OpenGL
    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, totalNormalBytes, normalDataPtr, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderProgram.getAttribLocation("vertexNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete vertexDataPtr; vertexDataPtr = nullptr;
    delete normalDataPtr; normalDataPtr = nullptr;
    data = nullptr;
    Rigid3D::checkGLErrors(__FILE__, __LINE__);
}

//---------------------------------------------------------------------------------------
void MeshNormals::setupShaders() {
    shaderProgram.loadFromFile("../data/shaders/PerFragLighting.vert",
                               "../data/shaders/TestNormals.frag");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(shaderProgram.getAttribLocation("vertexPosition"));
    glEnableVertexAttribArray(shaderProgram.getAttribLocation("vertexNormal"));

    Rigid3D::checkGLErrors(__FILE__, __LINE__);
}

//---------------------------------------------------------------------------------------
void MeshNormals::setupMatrices() {
    frustum = Frustum(45.0f, 4.0f/3.0f, 1.0f, 100.0f);
    projectionMatrix = frustum.getProjectionMatrix();

    viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0, 5.0),
                                      glm::vec3(0.0 , 0.0, -1.0),
                                      glm::vec3(0.0, 1.0, 0.0));

    modelMatrix = glm::mat4();

    // Translate the object into view.
    modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, -0.3f));

    updateMatrices();
}

//---------------------------------------------------------------------------------------
void MeshNormals::draw()
{
    static const unsigned int flatMeshStartIndex = 0;
    static const unsigned int smoothMeshStartIndex = meshFlat.getNumVertexPositions();

    shaderProgram.enable();
    switch(renderTarget){
    case MeshType::FLAT:
        glDrawArrays(GL_TRIANGLES, flatMeshStartIndex, meshFlat.getNumVertexPositions());
        break;
    case MeshType::SMOOTH:
        glDrawArrays(GL_TRIANGLES, smoothMeshStartIndex, meshSmooth.getNumVertexPositions());
        break;
    }
    shaderProgram.disable();

    Rigid3D::checkGLErrors(__FILE__, __LINE__);
}

//---------------------------------------------------------------------------------------
void MeshNormals::resize(int width, int height)
{
    float aspectRatio = ((float) width) / height;
    float frustumYScale = cotangent(degreesToRadians(frustum.getFieldOfViewY() / 2));

    float frustumXScale = frustumYScale * frustum.getAspectRatio();

    if (width > height) {
        // Shrink the x scale in eye-coordinate space, so that when geometry is
        // projected to ndc-space, it is widened out to become square.
        projectionMatrix[0][0] = frustumXScale / aspectRatio;
        projectionMatrix[1][1] = frustumYScale;
    }
    else {
        // Shrink the y scale in eye-coordinate space, so that when geometry is
        // projected to ndc-space, it is widened out to become square.
        projectionMatrix[0][0] = frustumXScale;
        projectionMatrix[1][1] = frustumYScale * aspectRatio;
    }

    // Use entire window for rendering.
    glViewport(0, 0, width, height);
}

//---------------------------------------------------------------------------------------
void MeshNormals::logic() {
    updateMatrices();
}

//---------------------------------------------------------------------------------------
void MeshNormals::updateMatrices() {
    modelViewMatrix = viewMatrix * modelMatrix;
    normalMatrix = mat3(modelViewMatrix);

    shaderProgram.setUniform("ModelViewMatrix", modelViewMatrix);
    shaderProgram.setUniform("NormalMatrix", normalMatrix);
    shaderProgram.setUniform("ProjectionMatrix", projectionMatrix);
}

//---------------------------------------------------------------------------------------
void MeshNormals::cleanup() {
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo_normals);
    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &vao);
}

//---------------------------------------------------------------------------------------
void MeshNormals::keyInput(int key, int action, int mods) {
    static const float xDelta = 0.5f;
    static const float yDelta = 0.2f;
    static const float zDelta = 0.5f;

    // Object Movement
    if (key == GLFW_KEY_A) {
        modelMatrix = translate(modelMatrix, vec3(-1 * xDelta, 0.0f, 0.0f));
    } else if (key == GLFW_KEY_D) {
        modelMatrix = translate(modelMatrix, vec3(xDelta, 0.0f, 0.0f));
    } else if (key == GLFW_KEY_W) {
        modelMatrix = translate(modelMatrix, vec3(0.0f, yDelta, 0.0f));
    } else if (key == GLFW_KEY_S) {
        modelMatrix = translate(modelMatrix, vec3(0.0f, -1 * yDelta, 0.0f));
    } else if (key == GLFW_KEY_R) {
        modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, -1 * zDelta));
    } else if (key == GLFW_KEY_F) {
        modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, zDelta));
    }

    // Change Render Target
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) {
            renderTarget = MeshType::FLAT;
            cout << "Flat Shading" << endl;
        } else if (key == GLFW_KEY_2) {
            renderTarget = MeshType::SMOOTH;
            cout << "Smooth Shading" << endl;
        }
    }
}
