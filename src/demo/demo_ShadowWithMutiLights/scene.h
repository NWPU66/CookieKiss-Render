#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>

#include "camera.h"
#include "light.h"
#include "model.h"

namespace ck {

class Scene {
private:
    std::vector<Model> models;
    LightGroup         light_group;
    Camera             camera;

public:
    Scene();

    void add_model_from_file(const std::string& path);
    void add_light(const Light& light);
};

};  // namespace ck