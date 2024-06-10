#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "core/ck_debug.h"

ck::Shader::Shader(const std::string& vertexShader_path,
                   const std::string& fragmentShader_path,
                   const std::string& geometryShader_path)
{
    bool use_geomShader = !geometryShader_path.empty();
    if (use_geomShader) { LOG(INFO) << "use geometry shader"; }

    // 从文件路径中获取顶点 / 片元着色器源码
    std::string   vertexShader_code;
    std::string   fragShader_code;
    std::string   geomShader_code;
    std::ifstream vertexShader_file;
    std::ifstream fragShader_file;
    std::ifstream geomShader_file;
    // 保证ifstream对象可以抛出异常：
    vertexShader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragShader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    geomShader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // 打开文件
        vertexShader_file.open(vertexShader_path);
        fragShader_file.open(fragmentShader_path);
        std::stringstream vertexShader_stream;
        std::stringstream fragShader_stream;
        // 读取数据至StringStream
        vertexShader_stream << vertexShader_file.rdbuf();
        fragShader_stream << fragShader_file.rdbuf();
        // 关闭文件处理器
        vertexShader_file.close();
        fragShader_file.close();
        // 转换数据流到String
        vertexShader_code = vertexShader_stream.str();
        fragShader_code   = fragShader_stream.str();
        if (use_geomShader)
        {
            geomShader_file.open(geometryShader_path);
            std::stringstream geomShader_stream;
            geomShader_stream << geomShader_file.rdbuf();
            geomShader_file.close();
            geomShader_code = geomShader_stream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        LOG(ERROR) << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what();
    }

    const char* vShaderCode = vertexShader_code.c_str();
    const char* fShaderCode = fragShader_code.c_str();

    // 编译着色器程序
    GLuint vertexShade = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader  = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint geomShader  = glCreateShader(GL_GEOMETRY_SHADER);
    // 设置着色器的源码
    glShaderSource(vertexShade, 1, &vShaderCode, nullptr);
    glShaderSource(fragShader, 1, &fShaderCode, nullptr);
    glCompileShader(vertexShade);
    glCompileShader(fragShader);
    // 检查着色器编译情况
    checkShaderCompiling(vertexShade);
    checkShaderCompiling(fragShader);
    if (use_geomShader)
    {
        const char* gShaderCode = geomShader_code.c_str();
        glShaderSource(geomShader, 1, &gShaderCode, nullptr);
        glCompileShader(geomShader);
        checkShaderCompiling(geomShader);
    }

    // 着色器程序
    id = glCreateProgram();
    glAttachShader(id, vertexShade);
    glAttachShader(id, fragShader);
    if (use_geomShader) { glAttachShader(id, geomShader); }
    glLinkProgram(id);
    // 检查链接情况
    checkShaderProgramCompiling(id);
    // 删除已经不需要的着色器源码
    glDeleteShader(vertexShade);
    glDeleteShader(fragShader);
    if (use_geomShader) { glDeleteShader(geomShader); }

    use();
    GL_CHECK();
}

ck::Shader::~Shader()
{
    glDeleteProgram(id);
}

void ck::Shader::use() const
{
    glUseProgram(id);
}

void ck::Shader::setParameter(const std::string& name, const bool& value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

void ck::Shader::setParameter(const std::string& name, const int& value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ck::Shader::setParameter(const std::string& name, const float& value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ck::Shader::setParameter(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void ck::Shader::setParameter(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void ck::Shader::setParameter(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ck::Shader::checkShaderCompiling(const GLuint shader)
{
    int         success = 0;
    std::string infoLog(512, '\0');
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog.data());
        LOG(ERROR) << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog;
    }
    else { LOG(INFO) << "Shader Compile success!"; }
}

void ck::Shader::checkShaderProgramCompiling(const GLuint shaderProgram)
{
    int         success = 0;
    std::string infoLog(512, '\0');
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog.data());
        LOG(ERROR) << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog;
    }
    else { LOG(INFO) << "Shader Program Compile success!"; }
}
