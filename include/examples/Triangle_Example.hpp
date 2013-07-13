#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <SfmlOpenGLWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <vector>

class Triangle_Example : public SfmlOpenGLWindow {
public:
    virtual void init();

    virtual void logic();

    virtual void draw();

    virtual void resize(int width, int height);

    virtual void cleanup();

    ~Triangle_Example() { }

protected:
    GLuint vertexBufferObject;
    GLuint vao;
    sf::Shader shader;

    const float vertexData[24] = {
        -0.5f, -0.366f, 0.0f, 1.0f,
        0.5f, -0.366f, 0.0f, 1.0f,
        0.0f,    0.5f, 0.0f, 1.0f,
        1.0f,    0.0f, 0.0f, 1.0f,
        0.0f,    1.0f, 0.0f, 1.0f,
        0.0f,    0.0f, 1.0f, 1.0f
    };

    void InitializeProgram();

    void InitializeVertexBuffer();
};


#endif /* TRIANGLE_HPP_ */
