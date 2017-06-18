#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLES3/gl3.h>


class Shader
{
public:
    explicit Shader() = default;

    ~Shader();

    void Use() const;

    GLuint CreateShader(const GLenum shaderType,
                        const std::string &shaderSource) const;

    void CreateProgram(std::string const & vertexShaderCode,
                       std::string const & fragmentShaderCode);

    void DeleteProgram();
    GLuint GetId() const;

    void SetFloat    ( GLchar const *name, GLfloat value);
    void SetInteger  ( GLchar const *name, GLint value);
    void SetVector2f ( GLchar const *name, GLfloat x, GLfloat y);
    void SetVector2f ( GLchar const *name, glm::vec2 const &value);
    void SetVector3f ( GLchar const *name, GLfloat x, GLfloat y, GLfloat z);
    void SetVector3f ( GLchar const *name, glm::vec3 const &value);
    void SetVector4f ( GLchar const *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void SetVector4f ( GLchar const *name, glm::vec4 const &value);
    void SetMatrix4  ( GLchar const *name, glm::mat4 const &matrix);

private:
    GLuint mId;
};