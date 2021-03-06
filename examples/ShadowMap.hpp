/**
 * @brief ShadowMap.hpp
 */

#ifndef SHADOW_MAP_HPP_
#define SHADOW_MAP_HPP_

#include <Utils/GlfwOpenGlWindow.hpp>
#include <Rigid3D/Rigid3D.hpp>

using namespace Rigid3D;

class ShadowMap : public GlfwOpenGlWindow {

public:
    ~ShadowMap() { }

    static shared_ptr<GlfwOpenGlWindow> getInstance();

private:
    // Mesh and Batch Containers
    MeshConsolidator meshConsolidator;
    unordered_map<const char *, BatchInfo> batchInfoMap;

    // Matrices
    mat4 modelMatrix_grid;
    mat4 modelMatrix_wallLeft;
    mat4 modelMatrix_wallBack;
    mat4 modelMatrix_bunny;
    mat4 modelMatrix_sphere;
    mat4 modelMatrix_light;
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat3 normalMatrix;
    mat4 shadowBiasMatrix;
    mat4 lightSPV; // light source matrices ShadowBias * Projection * View.

    ShaderProgram shaderProgram;
    ShaderProgram frustumShader;
    ShaderProgram depthTextureShader;

    RenderableFrustum renderableFrustum;

    int shadowMapHeight;
    int shadowMapWidth;

    GLuint vao;
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint shadowFBO;
    GLuint depthTexture;  // Shadow map texture.
    GLuint vao_shadowMap;
    GLuint vbo_shadowMap_data;

    struct SpotLight {
        vec3 position;      // Light position in world space.
        vec3 center;  // Where spot light is center on in eye coordinate space.
        vec3 rgbIntensity;  // Light intensity for each RGB component.
        Frustum frustum;
        mat4 viewMatrix;
        float exponent;  // Angular attenuation exponent.
        float conicAngle;  // Half angle of light cone (between 0 and 90 degrees).
    };
    SpotLight spotLight;

    struct MaterialProperties {
        vec3 emission;  // Emission light intensity from material for each RGB component.
        vec3 Ka;        // Coefficients of ambient reflectivity for each RGB component.
        vec3 Kd;        // Coefficients of diffuse reflectivity for each RGB component.
        float Ks;       // Coefficient of specular reflectivity, uniform across each RGB component.
        float shininess;   // Specular shininess factor.
    };
    MaterialProperties material_shadowBox;
    MaterialProperties material_bunny;
    MaterialProperties material_sphere;
    MaterialProperties material_light;

    // Key Input
    bool key_r_down = false;
    bool key_f_down = false;
    bool key_q_down = false;
    bool key_e_down = false;
    bool key_w_down = false;
    bool key_s_down = false;
    bool key_a_down = false;
    bool key_d_down = false;
    bool key_left_down = false;
    bool key_right_down = false;
    bool key_up_down = false;
    bool key_down_down = false;

    bool lookAt_bunny = false;
    bool lookAt_sphere = false;
    bool lookAt_light = false;
    bool render_light_frustum = false;
    bool render_shadow_map = false;

    ShadowMap(); // Singleton, prevent direct construction.

    virtual void init();
    virtual void logic();
    virtual void draw();
    virtual void keyInput(int key, int action, int mods);
    virtual void mouseScroll(double xOffSet, double yOffSet);
    virtual void cleanup();
    virtual void reloadShaderProgram();

    void setupShaders();
    void setupShadowFBO();
    void setupGLBuffers();
    void setupMatrices();
    void updateMatrices();
    void updateMaterialProperties(const MaterialProperties & m);
    void drawScene();
    void drawGrid();
    void drawLeftWall();
    void drawBackWall();
    void drawBunny();
    void drawSphere();
    void drawLight();
    void drawLightFrustum();
    void drawShadowMap();
    void processKeyInput();
};

#endif /* SHADOW_MAP_HPP_ */
