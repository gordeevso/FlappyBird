
#include "Shader.h"
#include "Log.h"
#include "GLState.h"

Shader::~Shader() {
    DeleteProgram();
}

void Shader::DeleteProgram() {
    glDeleteProgram(mId);
}

void Shader::Use() const {
    glUseProgram(mId);
}

GLuint Shader::GetId() const {
    return mId;
}

GLuint Shader::CreateShader(const GLenum shaderType,
                            const std::string &shaderSource) const {
    GLint compileResult{};
    GLchar log[256];
    GLuint shader{};

    shader = glCreateShader(shaderType);
    const GLchar * ptrShaderCode = shaderSource.c_str();
    const GLint shaderCodeSize = shaderSource.size();

    glShaderSource(shader, 1, &ptrShaderCode, &shaderCodeSize);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

    if (compileResult == GL_FALSE) {
        glGetShaderInfoLog(shader, sizeof(log), 0, log);
        switch(shaderType) {
            case GL_VERTEX_SHADER:
                Log::error("Vertex shader error: %s", log);
                break;
            case GL_FRAGMENT_SHADER:
                Log::error("Fragment shader error: %s", log);
                break;
        }

        if (shader > 0) {
            glDeleteShader(shader);
        }

        throw std::runtime_error("error compiling shader");
    }

    return shader;
}

void Shader::CreateProgram(const std::string& vertexShaderCode,
                           const std::string& fragmentShaderCode) {
    assert(GLState::GetInstance().IsInitialized());

    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderCode);
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    GLchar log[256];
    GLint  link_result{};

    mId = glCreateProgram();
    glAttachShader(mId, vertexShader);
    glAttachShader(mId, fragmentShader);

    glLinkProgram(mId);
    glGetProgramiv(mId, GL_LINK_STATUS, &link_result);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (link_result == GL_FALSE) {
        glGetProgramInfoLog(mId, sizeof(log), 0, log);
        Log::error("Shader program error: %s", log);
        throw std::runtime_error("error linking shader");
    }
}


void Shader::SetFloat(GLchar const *name, GLfloat value) {
    glUniform1f(glGetUniformLocation(mId, name), value);
}

void Shader::SetInteger(GLchar const *name, GLint value) {
    glUniform1i(glGetUniformLocation(mId, name), value);
}

void Shader::SetVector2f(GLchar const *name, GLfloat x, GLfloat y) {
    glUniform2f(glGetUniformLocation(mId, name), x, y);
}

void Shader::SetVector2f(GLchar const *name, glm::vec2 const & value) {
    glUniform2f(glGetUniformLocation(mId, name), value.x, value.y);
}

void Shader::SetVector3f(GLchar const *name, GLfloat x, GLfloat y, GLfloat z) {
    glUniform3f(glGetUniformLocation(mId, name), x, y, z);
}

void Shader::SetVector3f(GLchar const *name, glm::vec3 const & value) {
    glUniform3f(glGetUniformLocation(mId, name), value.x, value.y, value.z);
}

void Shader::SetVector4f(GLchar const *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    glUniform4f(glGetUniformLocation(mId, name), x, y, z, w);
}

void Shader::SetVector4f(GLchar const *name, glm::vec4 const & value) {
    glUniform4f(glGetUniformLocation(mId, name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4(GLchar const *name, glm::mat4 const & matrix) {
    glUniformMatrix4fv(glGetUniformLocation(mId, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

