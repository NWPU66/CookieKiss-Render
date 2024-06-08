#include "util/std_include.h"

#include "glm/matrix.hpp"
#include "imgui.h"

#include "core/ck_core.h"

int main(int argc, char** argv)
{
    std::cout << "Hello World!" << std::endl;
    std::cout << "Welcome to CookieKiss Render!" << std::endl;
    auto m = glm::mat4(1.0f);
    std::cout << glm::determinant(m) << std::endl;
    IMGUI_CHECKVERSION();
    return 0;
}