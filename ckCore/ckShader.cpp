#include "ckShader.h"

ckShader::ckShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    /**FIXME - 关于带有默认值的函数
     * 只能在定义或声明时指出默认参数，同时指定会报错。
     * 如果函数有声明就会以声明的为准，定义时指出的默认参数就会无效，所以最好在声明时指定。
     */
    bool useGeometryShader = (geometryPath != nullptr);
    std::cout << "useGeometryShader: " << useGeometryShader << std::endl;

    // 1. 从文件路径中获取顶点/片元着色器源码
    //---------------------------------------------------------------------
    std::string   vertexCode,  fragmentCode, geometryCode;
    std::ifstream vShaderFile, fShaderFile,  gShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取数据至StringStream
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到String
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure e)
    {
        // std::cout << vertexPath << std::endl;
        // std::cout << fragmentPath << std::endl;
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. 编译着色器程序
    //---------------------------------------------------------------------
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
    // 设置着色器的源码
    glShaderSource(vs, 1, &vShaderCode, NULL);
    glShaderSource(fs, 1, &fShaderCode, NULL);
    glCompileShader(vs);
    glCompileShader(fs);
    // 检查着色器编译情况
    checkShaderCompiling(vs);
    checkShaderCompiling(fs);
    if (geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        glShaderSource(gs, 1, &gShaderCode, NULL);
        glCompileShader(gs);
        checkShaderCompiling(gs);
    }

    // 着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    if (geometryPath != nullptr) { glAttachShader(ID, gs); }
    glLinkProgram(ID);
    // 检查链接情况
    checkShaderProgramCompiling(ID);
    // 删除已经不需要的着色器源码
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (geometryPath != nullptr) { glDeleteShader(gs); }

    use();
}

ckShader::~ckShader()
{
    glDeleteProgram(ID);
}

void ckShader::use() const
{
    glUseProgram(ID);
}

void ckShader::setParameter(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ckShader::setParameter(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ckShader::setParameter(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ckShader::setParameter(const std::string& name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

void ckShader::setParameter(const std::string& name, glm::vec2 value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

void ckShader::setParameter(const std::string& name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value));
}

void ckShader::checkShaderCompiling(GLuint shader) const
{
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog << std::endl;
    }
    else { std::cout << "Shader Compile success!" << std::endl; }
}

void ckShader::checkShaderProgramCompiling(GLuint shaderProgram) const
{
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog << std::endl;
    }
    else { std::cout << "Shader Program Compile success!" << std::endl; }
}
