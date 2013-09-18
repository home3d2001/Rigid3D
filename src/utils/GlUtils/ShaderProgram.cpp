#include <ShaderProgram.hpp>
#include <ShaderException.hpp>
#include <GlErrorCheck.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace GlUtils {

    //------------------------------------------------------------------------------------
    /**
     * Constructs a \c ShaderProgram with no attached shaders.  Vertex and fragment
     * shader programs can be attached to this \c ShaderProgram by calling the
     * method \c loadFromFile(...).
     */
    ShaderProgram::ShaderProgram() {
        initializeShaders();
    }

    //------------------------------------------------------------------------------------
    /**
     * Constructs a \c ShaderProgram object from vertex shader and fragment shader
     * source files.
     * @param vertexShaderFile - location to vertex shader source file.
     * @param fragmentShaderFile - location to fragment shader source file.
     */
    ShaderProgram::ShaderProgram(const char * vertexShaderFile, const char * fragmentShaderFile) {
        initializeShaders();

        loadFromFile(vertexShaderFile, fragmentShaderFile);
    }

    //------------------------------------------------------------------------------------
    void ShaderProgram::initializeShaders() {
        vertexShader = Shader();
        fragmentShader = Shader();
    }

    //------------------------------------------------------------------------------------
    /**
     * Constructs a vertex and fragment shader from the files provided, compiles them,
     * and attaches them to the \c ShaderProgram for use.
     *
     * @param vertexShaderFile - path to vertex shader file.
     * @param fragmentShaderFile - path to fragment shader file.
     *
     * @throws ShaderException in the event any of the following conditions occur:
     *   - either \c vertexShaderfile or \fragmentShaderFile cannot be opened or located.
     *   - shader files contain compilation errors.
     *   - shader files result in link errors when linked together.
     */
    void ShaderProgram::loadFromFile(const char * vertexShaderFile,
            const char * fragmentShaderFile) {
        checkGLErrors(__FILE__, __LINE__);
        extractSourceCode(vertexShaderFile, vertexShader);
        extractSourceCode(fragmentShaderFile, fragmentShader);

        createVertexShader();
        compileShader(vertexShader);

        createFragmentShader();
        compileShader(fragmentShader);

        createShaderProgram();
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteShader(vertexShader.shaderObject);
        glDeleteShader(fragmentShader.shaderObject);
        glDeleteProgram(programObject);
    }

    //------------------------------------------------------------------------------------
    void ShaderProgram::createVertexShader() {
        vertexShader.shaderObject = glCreateShader( GL_VERTEX_SHADER );
        checkGLErrors(__FILE__, __LINE__);
    }

    //------------------------------------------------------------------------------------
    void ShaderProgram::createFragmentShader() {
        fragmentShader.shaderObject = glCreateShader( GL_FRAGMENT_SHADER);
        checkGLErrors(__FILE__, __LINE__);
    }

    //------------------------------------------------------------------------------------
    void ShaderProgram::compileShader(const Shader &shader) {
        const char *source = shader.sourceCode.c_str();
        glShaderSource(shader.shaderObject, 1, (const GLchar **)&source, NULL);
        checkGLErrors(__FILE__, __LINE__);

        glCompileShader(shader.shaderObject);
        checkGLErrors(__FILE__, __LINE__);
        checkCompilationStatus(shader);
    }

    //------------------------------------------------------------------------------------
    /**
     * Checks for shader compilation errors. If found, a \c ShaderException is thrown
     * containing the compilation error messages.
     *
     * @param shader - \c Shader object.
     */
    void ShaderProgram::checkCompilationStatus(const Shader &shader) {
        GLint compileSuccess;

        glGetShaderiv(shader.shaderObject, GL_COMPILE_STATUS, &compileSuccess);
        if (compileSuccess == GL_FALSE) {
            GLint errorMessageLength;
            // Get the length in chars of the compilation error message.
            glGetShaderiv(shader.shaderObject, GL_INFO_LOG_LENGTH, &errorMessageLength);

            // Retrieve the compilation error message.
            GLchar errorMessage[errorMessageLength];
            glGetShaderInfoLog(shader.shaderObject, errorMessageLength, NULL, errorMessage);

            stringstream strStream;
            strStream << "Error Compiling Shader: " << errorMessage << endl;

            throw ShaderException(strStream.str());
        }
    }

    //------------------------------------------------------------------------------------
    void ShaderProgram::createShaderProgram() {
        programObject = glCreateProgram();
        checkGLErrors(__FILE__, __LINE__);

        glAttachShader(programObject, vertexShader.shaderObject);
        glAttachShader(programObject, fragmentShader.shaderObject);

        glLinkProgram(programObject);
        checkLinkStatus();
    }

    void ShaderProgram::enable() {
        glUseProgram(programObject);
    }

    void ShaderProgram::disable() {
        glUseProgram((GLuint)NULL);
    }

    //------------------------------------------------------------------------------------
    void ShaderProgram::checkLinkStatus() {
        GLint linkSuccess;

        glGetProgramiv(programObject, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess == GL_FALSE) {
            GLint errorMessageLength;
            // Get the length in chars of the link error message.
            glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &errorMessageLength);

            // Retrieve the link error message.
            GLchar errorMessage[errorMessageLength];
            glGetProgramInfoLog(programObject, errorMessageLength, NULL, errorMessage);

            stringstream strStream;
            strStream << "Error Linking Shaders: " << errorMessage << endl;

            throw ShaderException(strStream.str());
        }
    }

    //------------------------------------------------------------------------------------
    /**
     *
     * @return a \c GLuint representing the program object for this \c ShaderProgram
     */
    GLuint ShaderProgram::getProgramObject() {
        return programObject;
    }

    //------------------------------------------------------------------------------------
    /**
     * Extract text from \c sourceFile and store it in the field \c shader.sourceCode.
     *
     * @param sourceFileName
     * @param shader
     *
     * @throws ShaderException if \c sourceFile cannot be found or opened for reading.
     */
    void ShaderProgram::extractSourceCode(const char * sourceFile, Shader &shader) {
        ifstream file;

        file.open(sourceFile);
        if (!file) {
            stringstream strStream;
            strStream << "Error -- Failed to open file: " << sourceFile << endl;
            throw ShaderException(strStream.str());
        }

        stringstream strBuffer;
        string str;

        while(file.good()) {
            getline(file, str, '\r');
            strBuffer << str;
        }
        file.close();

        strBuffer << '\0';  // Append null terminator.

        shader.sourceCode = strBuffer.str();
    }

    //------------------------------------------------------------------------------------
    /**
     * Gets the location of a uniform variable within the shader program.
     *
     * @param uniformName - string representing the name of the uniform variable.
     *
     * @return a GLint location of the requested uniform variable.
     *
     * @throws ShaderException  if \c uniformName does not correspond to an active
     * uniform variable within the shader program or if \c uniformName
     * starts with the reserved prefix "gl_".
     */
    GLint ShaderProgram::getUniformLocation(const char * uniformName) {
        GLint result = glGetUniformLocation(programObject, (const GLchar *)uniformName);

        if (result == -1) {
            stringstream errorMessage;
            errorMessage << "Error obtaining uniform location: " << uniformName;
            throw ShaderException(errorMessage.str());
        }

        return result;
    }

    //------------------------------------------------------------------------------------
    /**
     * Gets the location of an attribute variable within the shader program.
     *
     * @param attributeName - string representing the name of the attribute variable.
     *
     * @return a GLint location of requested attribute variable.
     *
     * @throws ShaderException  if \c attributeName does not correspond to an active
     * attribute variable within the shader program or if \c attributeName
     * starts with the reserved prefix "gl_".
     */
    GLint ShaderProgram::getAttribLocation(const char * attributeName) {
        GLint result = glGetAttribLocation(programObject, (const GLchar *)attributeName);

        if (result == -1) {
            stringstream errorMessage;
            errorMessage << "Error obtaining attribute location: " << attributeName;
            throw ShaderException(errorMessage.str());
        }

        return result;
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param b
     */
    void ShaderProgram::setUniform(const char * uniformName, bool b) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniform1i(uniformLocation, b);
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param i
     */
    void ShaderProgram::setUniform(const char * uniformName, int i) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniform1i(uniformLocation, i);
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param ui
     */
    void ShaderProgram::setUniform(const char * uniformName, unsigned int ui) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniform1ui(uniformLocation, ui);
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param f
     */
    void ShaderProgram::setUniform(const char * uniformName, float f) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniform1f(uniformLocation, f);
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param x
     * @param y
     * @param z
     */
    void ShaderProgram::setUniform(const char * uniformName, float x, float y, float z) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniform3f(uniformLocation, x, y, z);;
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param v
     */
    void ShaderProgram::setUniform(const char * uniformName, vec2 v) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniform2fv(uniformLocation, 1, glm::value_ptr(v));;
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param v
     */
    void ShaderProgram::setUniform(const char * uniformName, vec3 v) {
        this->setUniform(uniformName, v.x, v.y, v.z);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param m
     */
    void ShaderProgram::setUniform(const char * uniformName, mat2 & m) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(m));
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param m
     */
    void ShaderProgram::setUniform(const char * uniformName, mat3 & m) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(m));
        glUseProgram(0);
    }

    //------------------------------------------------------------------------------------
    /**
     * Set the value of a uniform variable within the shader program.
     *
     * @param uniformName - name of the uniform variable to be set.
     * @param m
     */
    void ShaderProgram::setUniform(const char * uniformName, mat4 & m) {
        glUseProgram(programObject);
        GLint uniformLocation = getUniformLocation(uniformName);
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(m));
        glUseProgram(0);
    }

} // end namespace GlUtils
