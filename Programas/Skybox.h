#pragma once

#include "CommonValues.h"
#include <vector>
#include <string>
#include<GL/glew.h>
#include<glm.hpp>
#include<GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include "Mesh.h"
#include "Shader_light.h"
class Skybox
{
public:
    Skybox();
    Skybox(std::vector<std::string> faceLocations);
    void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    Mesh* skyMesh;
        
    ~Skybox();
private:
    Shader* skyShader;
    GLuint textureId;
    GLuint uniformProjection, uniformView;

};

