#pragma once
#include "stdInclude.h"
//make sure you have init the glad, before you include this file
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/matrix.hpp"
#include "glm/gtc/type_ptr.hpp"

class SimpleHardcodeGeom
{
private:
    /// @brief 从一组顶点的硬编码创建几何体
    static void createObjFromHardcode(GLuint&               vao,
                                      GLuint&               vbo,
                                      GLuint&               ebo,
                                      std::vector<GLfloat>& vertices,
                                      std::vector<GLuint>&  vertexIdx);

public:
    SimpleHardcodeGeom()  = default;
    ~SimpleHardcodeGeom() = default;

    static void createTriangleObj(GLuint& vao,
                                  GLuint& vbo,
                                  GLuint& ebo);

    static void createScreenObj(GLuint& vao,
                                GLuint& vbo,
                                GLuint& ebo);
};
