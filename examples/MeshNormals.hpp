#ifndef MESH_NORMALS_HPP
#define MESH_NORMALS_HPP

#include <Utils/GlfwOpenGlWindow.hpp>
#include <Rigid3D/Rigid3D.hpp>

#include <glm/gtc/reciprocal.hpp>
#include <vector>

using namespace Rigid3D;
using namespace glm;

class MeshNormals : public GlfwOpenGlWindow {

public:
    ~MeshNormals() { }

    virtual void resize(int width, int height);

    virtual void cleanup();

    static shared_ptr<GlfwOpenGlWindow> getInstance();

private:
    Mesh meshFlat, meshSmooth;

    enum class MeshType {FLAT, SMOOTH};
    MeshType renderTarget;

    // Shader Program Data
    ShaderProgram shaderProgram;
    Frustum frustum;
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 modelViewMatrix;
    mat4 projectionMatrix;
    mat3 normalMatrix;

    GLuint vao;
    GLuint vbo_vertices;
    GLuint vbo_normals;

    MeshNormals();

    virtual void init();
    virtual void logic();
    virtual void draw();
    virtual void keyInput(int key, int action, int mods);

    void setupGLBuffers();
    void setupShaders();
    void setupMatrices();
    void updateMatrices();
};

#endif /* MESH_NORMALS_HPP */
