/*
Semestre 2021-2
Pr·ctica : IluminaciÛn
Cambios en el shader, en lugar de enviar la textura en el shader de fragmentos, enviaremos el finalcolor
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"


//para iluminaciÛn
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture Tagave;

Model Hangar;
Model Camaro;
Model Avion;
Model Avion2;
Model Pajaro;
Model Poste;
Model Linterna;

Model Piso;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;//sol
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];// ilumina alrededor como esfera
SpotLight spotLights[MAX_SPOT_LIGHTS];//ilumina en direccion como cono


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat offset = 0.0f;

//*****************Avion********************
GLboolean adelanteAvion = true;
GLfloat activoAvion = 0.0f;
GLfloat topeRotacionAvion = 90.0f;
GLfloat vueltaAvion = 0.0f;

GLfloat xaAvion = 0.0f;
GLfloat yaAvion = 0.0f;
GLfloat xbAvion = 0.0f;
GLfloat ybAvion = 0.0f;

GLfloat xBezierAvion = 0.0f;
GLfloat zBezierAvion = 0.0f;
GLfloat cuentaAvion = 0.0f;

GLfloat posXAvion = 0.0f;
GLfloat posYAvion = 0.0f;

//*****************Avion2********************
GLfloat posXAvion2 = 0.0f;
GLfloat posYAvion2 = 0.0f;

//*****************Pajaros********************
GLfloat rotacionPajaros = 0.0f;
GLboolean arribaPajaros = true;
GLfloat offsetPajaros = 0.0f;
GLfloat posYPajaros = 0.0f;

//****************Camaro***************************
GLfloat aux1=mainWindow.getmuevex();
GLboolean camino1Camaro = true;
GLboolean adelanteCamaro = true;
GLboolean despuesVuelta = false;
GLfloat activoCamaro = 0.0f;
GLfloat topeRotacionCamaro = 180.0f;
GLboolean terminaCamaro = false;

GLfloat xaCamaro = 0.0f;
GLfloat yaCamaro = 0.0f;
GLfloat xbCamaro = 0.0f;
GLfloat ybCamaro = 0.0f;

GLfloat xBezierCamaro = 0.0f;
GLfloat zBezierCamaro = 0.0f;
GLfloat cuentaCamaro = 0.0f;

GLfloat posXCamaro = 0.0f;
GLfloat posYCamaro = 0.0f;
GLfloat terminoCamaro=0.0;
GLfloat skyboxTime = 0.0f;
//********************luces*************************
int contadorLuces = 1;

class wcPt3D{
public:
    GLfloat x,y,z;
};


static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//funcion para la curva de bezier
GLfloat getPt( GLfloat n1 , GLfloat n2 , GLfloat perc )
{
    GLfloat diff = n2 - n1;
    return n1 + ( diff * perc );
}


//c·lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}




void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};


	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
    meshList[0]->Bind();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	pisoTexture = Texture("Textures/tierra.tga");
	pisoTexture.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();

    Camaro = Model();
    Camaro.LoadModel("Models/camaro.obj");
    Avion= Model();
    Avion.LoadModel("Models/avion.obj");
    Avion2= Model();
    Avion2.LoadModel("Models/avion2.obj");
    Pajaro=Model();
    Pajaro.LoadModel("Models/pajaro.obj");
    Hangar = Model();
    Hangar.LoadModel("Models/Hangar.fbx");
    Poste = Model();
    Poste.LoadModel("Models/poste_luz.obj");
    Linterna = Model();
    Linterna.LoadModel("Models/linterna.obj");
    
    Piso = Model();
    Piso.LoadModel("Models/floor.obj");

	std::vector<std::string> skyboxFaces1;
	skyboxFaces1.push_back("Textures/Skybox/manana/1_rt.jpg");
	skyboxFaces1.push_back("Textures/Skybox/manana/1_lf.jpg");
	skyboxFaces1.push_back("Textures/Skybox/manana/1_dn.jpg");
	skyboxFaces1.push_back("Textures/Skybox/manana/1_up.jpg");
	skyboxFaces1.push_back("Textures/Skybox/manana/1_bk.jpg");
	skyboxFaces1.push_back("Textures/Skybox/manana/1_ft.jpg");
    
    std::vector<std::string> skyboxFaces2;
    skyboxFaces2.push_back("Textures/Skybox/tarde/1_rt.jpg");
    skyboxFaces2.push_back("Textures/Skybox/tarde/1_lf.jpg");
    skyboxFaces2.push_back("Textures/Skybox/tarde/1_dn.jpg");
    skyboxFaces2.push_back("Textures/Skybox/tarde/1_up.jpg");
    skyboxFaces2.push_back("Textures/Skybox/tarde/1_bk.jpg");
    skyboxFaces2.push_back("Textures/Skybox/tarde/1_ft.jpg");
    
    std::vector<std::string> skyboxFaces3;
    skyboxFaces3.push_back("Textures/Skybox/noche/1_rt.jpg");
    skyboxFaces3.push_back("Textures/Skybox/noche/1_lf.jpg");
    skyboxFaces3.push_back("Textures/Skybox/noche/1_dn.jpg");
    skyboxFaces3.push_back("Textures/Skybox/noche/1_up.jpg");
    skyboxFaces3.push_back("Textures/Skybox/noche/1_bk.jpg");
    skyboxFaces3.push_back("Textures/Skybox/noche/1_ft.jpg");
    
    skybox = Skybox(skyboxFaces1);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//posiciÛn inicial del helicÛptero
	glm::vec3 avionInicio = glm::vec3(0.0f, 40.0f, 40.0);
    glm::vec3 desplazamientoAvion = avionInicio;
    
    glm::vec3 avionInicio2 = glm::vec3(-80.0f, 5.0f, 25.0f);
    glm::vec3 desplazamientoAvion2 = avionInicio2;
    
    glm::vec3 camaroInicio= glm::vec3(-180.0f, 0.0f, 100.0f);
    glm::vec3 desplazamientoCamaro = camaroInicio;
    
    glm::vec3 pajaro1 = glm::vec3(100.0f, 30.0f, -50.0f);
    glm::vec3 pajaro2 = glm::vec3(90.0f, 30.0f, -40.0f);
    glm::vec3 pajaro3 = glm::vec3(100.0f, 30.0f, -30.0f);

	//luz direccional, sÛlo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,//color de la luz
		0.3f, 0.3f,//1.- color 2.-
		0.0f, 0.0f, -1.0f);//hacia donde olumina
	//contador de luces puntuales
	unsigned int pointLightCount = 0;// arrelo de tipo puntual
	//DeclaraciÛn de primer luz puntual
    
    pointLights[0] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -150.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[1] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -150.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[2] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -120.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[3] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -120.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[4] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -113.0f, 3.0f, 35.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[5] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -90.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[6] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -60.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[7] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -60.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[8] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -60.0f, 3.0f, 35.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[9] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -60.0f, 3.0f, 6.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[10] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -30.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[11] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -30.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[12] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -30.0f, 3.0f, 19.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[13] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        -30.0f, 3.0f, -10.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[14] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        0.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[15] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        0.0f, 3.0f, 2.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[16] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        0.0f, 1.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[17] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        0.0f, 3.0f, -26.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[18] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        30.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[19] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        30.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[20] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        30.0f, 3.0f, -13.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[21] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        30.0f, 3.0f, -41.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[22] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        60.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[23] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        60.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[24] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        60.0f, 3.0f, -30.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[25] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        60.0f, 3.0f, -58.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[26] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        90.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[27] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        90.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[28] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        120.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[29] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        120.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[30] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        150.0f, 3.0f, 70.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;
    
    pointLights[31] = PointLight(1.0f, 1.0f, 1.0f, //rojo
        0.0f, 1.0f,
        150.0f, 3.0f, 39.0f,
        0.3f, 0.2f, 0.1f);//sirven para la atenuacion, alcance de la iluminacion
    pointLightCount++;

    

	unsigned int spotLightCount = 0;//arreglo spotlight
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,//color
		0.0f, 2.0f,//ambienta y difusa
		0.0f, 0.0f, 0.0f,//pos
		0.0f, -1.0f, 0.0f,//vector de direccion
		1.0f, 0.0f, 0.0f,//ecuacion
		5.0f);//apertura del cono
	spotLightCount++;
    
    //poste 1
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
        1.0f, 2.0f,
        -150.0f, 24.0f, 105.0f,
        0.0f, -1.0f, 0.0f,//vector de direccion
        1.0f, 0.0f, 0.0f,//ecuacion
        15.0f);
    spotLightCount++;
    
    //poste 2
    spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
        1.0f, 2.0f,
        -200.0f, 24.0f, 105.0f,
        0.0f, -1.0f, 0.0f,//vector de direccion
        1.0f, 0.0f, 0.0f,//ecuacion
        15.0f);
    spotLightCount++;
    
    //luz interna
    spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
        1.0f, 2.0f,
        -180.0f, 35.0f, 170.0f,
        0.0f, -1.0f, 0.0f,//vector de direccion
        1.0f, 0.0f, 0.0f,//ecuacion
        15.0f);
    spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	
	////Loop mientras no se cierra la ventan
    GLfloat t0 = glfwGetTime();
    GLfloat t0Luces = glfwGetTime();
    while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //cambiar el skybox cada 5 segundos
        if((glfwGetTime()-t0)>=5.0f){
            t0=glfwGetTime();
            if(skyboxTime==0.0f){//de mañana a tarde
                skyboxTime=1.0f;
            }
            else if(skyboxTime==1.0f){//de tarde a noche
                skyboxTime=2.0f;
            }
            else if(skyboxTime==2.0f){//de noche a mañana
               skyboxTime=0.0f;
            }
        }
        if(skyboxTime==0.0f){
            skybox = Skybox(skyboxFaces2);
        }
        if(skyboxTime==1.0f){
            skybox = Skybox(skyboxFaces3);
        }
        if(skyboxTime==2.0f){
            skybox = Skybox(skyboxFaces1);
        }
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//informaciÛn en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la c·mara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaciÛn al shader de fuentes de iluminaciÛn
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
        glm::mat4 modelaux(1.0);
        glm::mat4 modelCamaro(1.0);

		model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

        
		//agregar su coche y ponerle material
        
//*************************************DESPLAZAMIENTO Y TOTACION DEL CAMARO***********************************
        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(desplazamientoCamaro));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        modelCamaro = model;
        
        if(despuesVuelta == false and terminaCamaro==false){
            if(activoCamaro==0.0f){
                 model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
                posXCamaro=posXCamaro-0.2;
            }
            desplazamientoCamaro = glm::vec3(camaroInicio.x + xBezierCamaro, camaroInicio.y,camaroInicio.z + zBezierCamaro + posXCamaro );
        }else if (activoCamaro==0.0f and terminaCamaro==false){
            if(desplazamientoCamaro.x<100){
                model = glm::rotate(model, topeRotacionCamaro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            posYCamaro=posYCamaro+1.2;
            desplazamientoCamaro = glm::vec3(camaroInicio.x + xBezierCamaro + posYCamaro, camaroInicio.y,camaroInicio.z + zBezierCamaro + posXCamaro);
        }
        if(desplazamientoCamaro.z <= 80.0f and cuentaCamaro<1 and terminaCamaro==false){
            //Funciones de bezier para trazar las curvas
            activoCamaro=1.0f;
            cuentaCamaro=cuentaCamaro+0.01;
            xaCamaro = getPt( 0.0f , 7.0f , cuentaCamaro );
            yaCamaro = getPt( 0.0f , -30.0f , cuentaCamaro);
            xbCamaro = getPt( 7.0f , 20.0f , cuentaCamaro );
            ybCamaro = getPt( -30.0f , -35.0f ,cuentaCamaro );

            // The Black Dot
            xBezierCamaro = getPt( xaCamaro , xbCamaro , cuentaCamaro );
            zBezierCamaro = getPt( yaCamaro , ybCamaro , cuentaCamaro );
            
            model = glm::rotate(model, topeRotacionCamaro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            topeRotacionCamaro-=1.2;
            
            if(topeRotacionCamaro<=90){
                topeRotacionCamaro=90;
            }
        }
        if(cuentaCamaro>0.99 and activoCamaro==1.0){
            activoCamaro =0.0f;
            adelanteCamaro = false;
            despuesVuelta=true;
            topeRotacionCamaro=90.0;
        }
        
        if (desplazamientoCamaro.x >=100){
            if(terminaCamaro==false){
                topeRotacionCamaro=topeRotacionCamaro-1.0;
                model = glm::rotate(model, topeRotacionCamaro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            if(topeRotacionCamaro==45.0){
                terminaCamaro=true;
                model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            }
        }
        
        
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Camaro.RenderModel();
//*************************************DESPLAZAMIENTO Y TOTACION DEL CAMARO***********************************
        
//*************************************DESPLAZAMIENTO Y TOTACION DEL Avion***********************************
		
		//agregar incremento en X con teclado
		model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(0.80f, 0.80f, 0.80f));
        
        if(adelanteAvion==false && activoAvion ==0.0f){
            offset += 0.1;
            posYAvion = 2 * sin(10*offset * toRadians); //arriba y abajo
        }
        //Primera vuelta de ida del avion
        if(desplazamientoAvion.x <= -100.0f and cuentaAvion<1){
            //Funciones de bezier para trazar las curvas
            activoAvion=1.0f;
            adelanteAvion=false;
            cuentaAvion=cuentaAvion+0.01;
            xaAvion = getPt( 0.0f , -20.0f , cuentaAvion );
            yaAvion = getPt( 0.0f , -15.0f , cuentaAvion);
            xbAvion = getPt( -20.0f , 0.0f , cuentaAvion );
            ybAvion = getPt( -15.0f , -30.0f ,cuentaAvion );

            // The Black Dot
            xBezierAvion = getPt( xaAvion , xbAvion , cuentaAvion );
            zBezierAvion = getPt( yaAvion , ybAvion , cuentaAvion );
        }
        
        if(cuentaAvion>0.99 and activoAvion == 1.0f){
            activoAvion =0.0f;
            adelanteAvion = false;
            cuentaAvion=0.0f;
            posXAvion=posXAvion+2;
            desplazamientoAvion = glm::vec3(avionInicio.x+xBezierAvion+posXAvion, avionInicio.y+posYAvion,avionInicio.z+zBezierAvion);
        }
        
        //Segunda vuelta de ida del avion
        if(desplazamientoAvion.x >=100.0f and cuentaAvion<1){
            //Funciones de bezier para trazar las curvas
            activoAvion=2.0f;
            adelanteAvion=false;
            cuentaAvion=cuentaAvion+0.01;
            xaAvion= getPt( 0.0f , 20.0f , cuentaAvion );
            yaAvion = getPt( -30.0f , -15.0f , cuentaAvion );
            xbAvion = getPt( 20.0f , 0.0f , cuentaAvion );
            ybAvion = getPt( -15.0f , 0.0f , cuentaAvion );

            // The Black Dot
            xBezierAvion = getPt( xaAvion , xbAvion , cuentaAvion );
            zBezierAvion = getPt( yaAvion , ybAvion , cuentaAvion );
        }
        
        if(cuentaAvion>0.99 and activoAvion == 2.0f){
            activoAvion =0.0f;
            adelanteAvion= true;
            cuentaAvion=0.0f;
            posXAvion=posXAvion-2;
            desplazamientoAvion = glm::vec3(avionInicio.x+xBezierAvion+posXAvion, avionInicio.y+posYAvion,avionInicio.z+zBezierAvion);
        }
        
        //movimiento recto de frente del avion
        if(adelanteAvion and activoAvion==0.0f){
            posXAvion -= 0.1*deltaTime;
        }
        else if(activoAvion==0.0f){
            posXAvion += 0.1*deltaTime;
        }
        
        model = glm::translate(model, glm::vec3(desplazamientoAvion));
        
        //Rotaciones del avion para que de vuelta realista
        
        if(adelanteAvion and activoAvion==0.0f){
            topeRotacionAvion=0.0f;
            vueltaAvion=0.0f;
        }else if (activoAvion == 0.0f){
            topeRotacionAvion=180.0f;
            vueltaAvion=0.0f;
            model = glm::rotate(model, topeRotacionAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        if ( activoAvion == 1.0f){
            topeRotacionAvion=topeRotacionAvion-1.8;
            vueltaAvion=vueltaAvion-3.5;
            model = glm::rotate(model, topeRotacionAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, vueltaAvion * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
            if(topeRotacionAvion <= -180.0f){
                topeRotacionAvion=-180.0f;
            }
        }
        if ( activoAvion == 2.0f){
            topeRotacionAvion=topeRotacionAvion-1.8;
            vueltaAvion=vueltaAvion-3.5;
            model = glm::rotate(model, topeRotacionAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, vueltaAvion * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
            if(topeRotacionAvion <= 0.0f){
                topeRotacionAvion=0.0f;
            }
        }
        
        
        //Transformaciones al modelo final
        
        desplazamientoAvion = glm::vec3(avionInicio.x+xBezierAvion+posXAvion, avionInicio.y+posYAvion,avionInicio.z+zBezierAvion);

		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Avion.RenderModel();

//*************************************DESPLAZAMIENTO Y TOTACION DEL Avion 2***********************************
        
        if(desplazamientoAvion2.y <= 200.0f){
            model = glm::mat4(1.0);
            model = glm::scale(model, glm::vec3(0.80f, 0.80f, 0.80f));
        
            if(mainWindow.getArranca()==true){
            desplazamientoAvion2=glm::vec3(avionInicio2.x+posXAvion2,avionInicio2.y+posYAvion2,+avionInicio2.z-(posXAvion2 / 2));
                
                model = glm::translate(model, glm::vec3(desplazamientoAvion2));
                posXAvion2=posXAvion2+0.3;
            }else{
                model = glm::translate(model, glm::vec3(desplazamientoAvion2));
            }
            if(desplazamientoAvion2.x>=60.0f){
                posYAvion2=posYAvion2+0.5;
                if((posYAvion2/2)>=50.0f){
                    model = glm::rotate(model, 50 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
                }else{
                    model = glm::rotate(model, posYAvion2 / 2 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
                }
            }
            model = glm::rotate(model, 207 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Avion2.RenderModel();
        }
        
//***********************************Pajaro1****************************************+
        if(pajaro1.x-posYAvion2>=-200){
            if(mainWindow.getArranca()==true){
                //rotacion
                if(arribaPajaros==true and rotacionPajaros<=50){
                    rotacionPajaros=rotacionPajaros+1;
                }else{
                    arribaPajaros=false;
                    rotacionPajaros=rotacionPajaros-1;
                    if(rotacionPajaros<=-20){
                        arribaPajaros=true;
                    }
                }
                //traslacion en Y
                offsetPajaros += 0.1;
                posYPajaros = 2 * sin(10 * offsetPajaros * toRadians);
            }
            
            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(pajaro1.x-posXAvion2,pajaro1.y+posYPajaros,pajaro1.z));
            
            model = glm::rotate(model, rotacionPajaros * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
            
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Pajaro.RenderModel();
            
    //***********************************Pajaro2****************************************+
            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(pajaro2.x-posXAvion2,pajaro2.y+posYPajaros,pajaro2.z));
            
            model = glm::rotate(model, rotacionPajaros * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
            
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Pajaro.RenderModel();
            
    //***********************************Pajaro3****************************************+
            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(pajaro3.x-posXAvion2,pajaro3.y+posYPajaros,pajaro3.z));
            
            model = glm::rotate(model, rotacionPajaros * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
            
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Pajaro.RenderModel();
        }
           
//******************************************Hangar****************************************
        model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(20.0f, 20.0f, 3.0f));
        model = glm::translate(model, glm::vec3(-9.0f, 0.0f, 70.0f));
        model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Hangar.RenderModel();
        
//*****************************************Luces de pista**********************************
        //Fondo atras
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        //2
        Linterna.RenderModel();
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //3
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-120.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //4
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-90.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //5
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //6
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //7
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //8
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(60.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //9
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(90.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //10
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(120.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //11
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(150.0f, 0.0f, 70.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        
        //Fondo delante
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //2
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //3
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-120.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //4 eliminada
        //5
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //6
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //7
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //8
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(60.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //9
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(90.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //10
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(120.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //11
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(150.0f, 0.0f, 39.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        
        //segunda pista delante
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 35.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //2
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 19.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //3
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //4
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 0.0f, -13.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //5
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(60.0f, 0.0f, -30.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();

        
        //segunda pista atras
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-113.0f, 0.0f, 35.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //2
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -10.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //3
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -26.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //4
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 6.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //5
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 0.0f, -41.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();
        //6
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(60.0f, 0.0f, -58.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();

//**************************serie de luces***************************
        if((glfwGetTime()-t0Luces)>=0.4f){
            t0Luces=glfwGetTime();
            for (int i=0; i<=31; i++) {
                pointLights[i].SetColor(glm::vec3(1.0f,1.0f,1.0f));
            }
            //switch
            switch (contadorLuces) {
                case 1:
                    pointLights[0].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[1].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 2:
                    pointLights[2].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[3].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[4].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 3:
                    pointLights[5].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 4:
                    pointLights[6].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[7].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[8].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[9].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 5:
                    pointLights[10].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[11].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[12].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[13].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 6:
                    pointLights[14].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[15].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[16].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[17].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 7:
                    pointLights[18].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[19].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[20].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[21].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 8:
                    pointLights[22].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[23].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[24].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[25].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 9:
                    pointLights[26].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[27].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 10:
                    pointLights[28].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[29].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;
                    
                case 11:
                    pointLights[30].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                    pointLights[31].SetColor(glm::vec3(0.0f,0.0f,0.0f));
                break;

                    
                default:
                    break;
            }
            if(contadorLuces>=1){
                contadorLuces=contadorLuces+1;
            }
            if(contadorLuces==12){
                contadorLuces=1;
            }
        }
        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-150.0f, 10.0f, 115.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Poste.RenderModel();
        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-200.0f, 10.0f, 115.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Poste.RenderModel();
        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-180.0f, 35.0f, 170.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Linterna.RenderModel();

        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Piso.RenderModel();
        
        //Agave que sucede si lo renderizan antes del coche y la pista
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f,0.0f,10.0f));
        model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        //blending transparencia o traslucidez = SOLO SE TRANSPARENTA EL SKYBOX
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Tagave.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[3]->RenderMesh();
        glDisable(GL_BLEND);
        
        //ANIMACION BASICA NO ES UNA TRASLACION NI ROTACION NI ESCALA
        //DEBE SER LA COMBINACION DE AMBAS
        
        //COMPLEJA = UNIMACION BASADA EN FUNCIONES, ECUACIONES QUE EXISTEN COMO
        //CAIDA LIBRE TIRO PRABOLICO, FUNCIONES DE CURVA, MOVIMIENTOS FISICOS
        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f,0.0f,-10.0f));
        model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        //blending transparencia o traslucidez
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Tagave.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[3]->RenderMesh();
        glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

