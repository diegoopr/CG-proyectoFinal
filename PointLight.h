#pragma once
#include "Light.h"

class PointLight :
    public Light
{
public:
    PointLight();
    void SetColor(glm::vec3 color);
    PointLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat aIntensity, GLfloat dIntensity,
        GLfloat xPos, GLfloat yPos, GLfloat zPos,
        GLfloat con, GLfloat lin, GLfloat exp);

    void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
        GLfloat diffuseIntensityLocation, GLfloat positionLocation,
        GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation);

    ~PointLight();

protected:
    glm::vec3 position;
    glm::vec3 color;

    GLfloat constant, linear, exponent;
};

