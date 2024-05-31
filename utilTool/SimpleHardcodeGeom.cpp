#include "SimpleHardcodeGeom.h"

/**NOTE - Hardcode Geom Data
 */
// 屏幕几何数据的硬编码
static std::vector<GLfloat> screenVertices = {
    // 位置               // 纹理坐标
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // 左上
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // 右上
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 左下
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f   // 右下
};
static std::vector<GLuint> screenVerticesIdx = {
    0, 2, 1, // 第一个三角形
    1, 2, 3  // 第二个三角形
};

// 简单三角形的硬编码
static std::vector<GLfloat> triangleVertices = {
    // 位置               // 纹理坐标
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // 
    0.0f, 0.5f, 0.0f, 0.5f, 0.1f,   // 
};
static std::vector<GLuint> triangleVerticesIdx = {
    0, 1, 2, // 第一个三角形
};
/**FIXME - 错题本
 * 这里为什么用static？
 * main.cpp编译的时候，全局变量triangleVertices初始化了一次
 * 在编译utilTool.lib库文件的时候，用到了SimpleHardcodeGeom.cpp
 * 这里全局变量triangleVertices又初始化了一次，触发了重定义错误
 *
 * 设置成static后，静态变量只初始化一次。
 */

void SimpleHardcodeGeom::createObjFromHardcode(GLuint&               vao,
                                               GLuint&               vbo,
                                               GLuint&               ebo,
                                               std::vector<GLfloat>& vertices,
                                               std::vector<GLuint>&  vertexIdx)
{
    bool useEBO = (vertexIdx.size() > 0);
    // VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(),
                 GL_STATIC_DRAW);
    // VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // EBO
    if (useEBO)
    {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIdx.size() * sizeof(GLuint), vertexIdx.data(),
                     GL_STATIC_DRAW);
    }
    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (useEBO) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
}

void SimpleHardcodeGeom::createScreenObj(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    createObjFromHardcode(vao, vbo, ebo, screenVertices, screenVerticesIdx);
}

void SimpleHardcodeGeom::createTriangleObj(GLuint& vao, GLuint& vbo, GLuint& ebo)
{
    createObjFromHardcode(vao, vbo, ebo, triangleVertices, triangleVerticesIdx);
}
