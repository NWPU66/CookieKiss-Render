#include "util/stdInclude.h"

#include "glm/matrix.hpp"
#include "imgui.h"

int main(int argc, char** argv)
{
    std::cout << "Hello World!" << std::endl;
    std::cout << "Welcome to CookieKiss Render!" << std::endl;
    glm::mat4 m = glm::mat4(1.0f);
    std::cout << glm::determinant(m) << std::endl;
    IMGUI_CHECKVERSION();
    return 0;
}