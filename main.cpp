/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: lab09
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with GLSL shaders.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2017
 *
 */

//*************************************************************************************

// LOOKHERE #2
#include <GL/glew.h>    // include GLEW to get our OpenGL 3.0+ bindings
#include <GLFW/glfw3.h> // include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL/SOIL.h> // for image loading

#include <stdio.h>  // for printf functionality
#include <stdlib.h> // for exit functionality
#include <time.h>   // for time functionality

// note that all of these headers end in *3.hpp
// these class library files will only work with OpenGL 3.0+
#include <CSCI441/modelLoader3.hpp>   // to load in OBJ models
#include <CSCI441/OpenGLUtils3.hpp>   // to print info about OpenGL
#include <CSCI441/objects3.hpp>       // to render our 3D primitives
#include <CSCI441/ShaderProgram3.hpp> // our shader helper functions
#include <CSCI441/TextureUtils.hpp>   // our texture helper functions
#include <CSCI441/FramebufferUtils3.hpp>

#include "include/Shader_Utils.h" // our shader helper functions

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h> // For popups

#include "Particle.h"
#include "ParticleSystem.h"
#include "Spawner.h"
#include "Marble.h"

//*************************************************************************************
//
// Global Parameters

int windowWidth, windowHeight;
GLuint fbo;
GLuint rbo;
int framebufferWidth = 1024, framebufferHeight = 1024;
GLuint framebufferTextureHandle;
GLuint texturedQuadVAO;

CSCI441::ShaderProgram *postprocessingShaderProgram = NULL;
GLint uniform_post_proj_loc, uniform_post_fbo_loc, uniform_post_modelLocation;
GLint attrib_post_vpos_loc, attrib_post_vtex_loc;

bool controlDown = false;
bool leftMouseDown = false;
glm::vec2 mousePosition(-9999.0f, -9999.0f);

glm::vec3 cameraAngles(0.00f, 2.00f, 14.0f);
glm::vec3 eyePoint(10.0f, 10.0f, 10.0f);
glm::vec3 lookAtPoint(0.0f, 0.0f, 0.0f);
glm::vec3 upVector(0.0f, 1.0f, 0.0f);
glm::vec3 suzPoint = eyePoint;
glm::vec3 oldCamAngles;
bool ml = false;
bool mr = false;
bool fpv = false;

CSCI441::ModelLoader *model = NULL;
glm::mat4 suzModelMtx;
glm::vec3 location = glm::vec3(0.0f, 1.0f, 0.0f);
float rota = 0.0f;
glm::vec3 scaleFactor = glm::vec3(1.0f, 1.0f, 1.0f);
float incrementor = 0.01f;

GLuint shaderProgramHandle = 0;
GLint mvp_uniform_location = -1;
GLint vpos_attrib_location = -1;
GLint tex_attrib_location = -1;

GLuint objectShaderProgram = 0;
GLint MVPloc = -1;
GLint VALloc = -1;
GLint NALloc = -1;
GLint TALloc = -1;
GLint ambient = -1;
GLint diffuse = -1;
GLint specular = -1;
GLint LPUloc = -1;
GLint liAmbient = -1;
GLint liDiffuse = -1;
GLint liSpecular = -1;
GLint shine = -1;
GLint camUniform = -1;
GLint objModelMtx = -1;
GLint objViewMtx = -1;
GLint objNormMtx = -1;

CSCI441::ShaderProgram *billboardShaderProgram = NULL;
GLint bbmvp = -1;
GLint bbproj = -1;
GLint bbvpos = -1;
GLint bbsize = -1;
Vertex points[1];
GLuint pointsVAO, pointsVBO;

CSCI441::ShaderProgram *marbleShaderProgram = NULL;
GLint uniform_modelMtx_loc, uniform_viewMtx_loc, uniform_viewProjetionMtx_loc, uniform_normalMtx_loc, uniform_tex_loc, uniform_color_loc;
GLint attrib_vPos_loc, attrib_normal_loc, attrib_vTextureCoord_loc;
GLint uniform_light_pos;
GLint uniform_light_ambient;
GLint uniform_light_diffuse;
GLint uniform_light_specular;
GLint uniform_light_shine;
GLint uniform_camera_pos;

GLuint vaodnegy;
GLuint vaodposy;
GLuint vaodnegx;
GLuint vaodposx;
GLuint vaodnegz;
GLuint vaodposz;
const unsigned int numPlatform = 40;
GLuint vaod[numPlatform];
GLuint vbod;

GLuint negyTextureHandle;
GLuint posyTextureHandle;
GLuint negxTextureHandle;
GLuint posxTextureHandle;
GLuint negzTextureHandle;
GLuint poszTextureHandle;
GLuint platformTextureHandle;
GLuint particleHandle;
GLuint brickTexHandle;

vector<ParticleSystem> psystems;
vector<Spawner> spawners;

void readFile(const char *filename)
{
    // Checking if it is .csv
    string extension = "@@@@";
    for (int i = 0; i < 4; i++)
    {
        extension[i] = filename[strlen(filename) - 4 + i];
    }

    if (extension != ".csv")
    {
        fprintf(stderr, "[ERROR]: INVALID FILE TYPE. ENSURE IT IS A .CSV\n");
        exit(EXIT_FAILURE);
    }

    ifstream input(filename);
    string line;

    // Checking if file exists and was sucessfully opened
    if (!input.is_open())
    {
        fprintf(stderr, "[ERROR]: UNABLE TO OPEN FILE\n");
        exit(EXIT_FAILURE);
    }

    while (getline(input, line))
    {
        stringstream ss(line);
        float f;
        char t;
        vector<float> parameters;

        ss >> t;
        if (t == 'F') // Fountain
        {
            ss >> t;
            while (ss >> f)
            {
                parameters.push_back(f);
                if (ss.peek() == ',' || ss.peek() == ' ')
                {
                    ss.ignore();
                }
            }

            if (parameters.size() != 9)
            {
                cerr << "[ERROR]: INVALID FILE STRUCTURE" << endl;
            }
            else
            {
                ParticleSystem psys;
                psys.startingLoc.x = parameters.at(0);
                psys.startingLoc.y = parameters.at(1);
                psys.startingLoc.z = parameters.at(2);
                psys.coneAngle = parameters.at(3);
                psys.minV = parameters.at(4);
                psys.maxV = parameters.at(5);
                psys.minLife = parameters.at(6);
                psys.maxLife = parameters.at(7);
                psys.spawn = 1.0f / parameters.at(8);
                psys.start = glfwGetTime();
                psys.updateStart = glfwGetTime();
                psystems.push_back(psys);
            }
            parameters.clear();
        }
        else // Enemy Marble Spawner
        {
            ss >> t;
            while (ss >> f)
            {
                parameters.push_back(f);
                if (ss.peek() == ',' || ss.peek() == ' ')
                {
                    ss.ignore();
                }
            }

            Spawner sp;
            sp.startingLoc.x = parameters.at(0);
            sp.startingLoc.y = parameters.at(1);
            sp.startingLoc.z = parameters.at(2);
            sp.startingDir.x = parameters.at(3);
            sp.startingDir.y = parameters.at(4);
            sp.startingDir.z = parameters.at(5);
            sp.size = parameters.at(6);
            sp.spawn = 1.0f / parameters.at(7);
            sp.start = glfwGetTime();
            sp.updateStart = glfwGetTime();
            //sp.add();
            spawners.push_back(sp);
        }
    }
    input.close();
}

//******************************************************************************
//
// Helper Functions

// convertSphericalToCartesian() ///////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraAngles is updated.
//
////////////////////////////////////////////////////////////////////////////////
void convertSphericalToCartesian()
{
    eyePoint.x = cameraAngles.z * sinf(cameraAngles.x) * sinf(cameraAngles.y);
    eyePoint.y = cameraAngles.z * -cosf(cameraAngles.y);
    eyePoint.z = cameraAngles.z * -cosf(cameraAngles.x) * sinf(cameraAngles.y);
}

// loadAndRegisterTexture() ////////////////////////////////////////////////////
//
// Load and register a texture with OpenGL
//
////////////////////////////////////////////////////////////////////////////////
GLuint loadAndRegisterTexture(const char *filename)
{
    GLuint texHandle = SOIL_load_OGL_texture(filename,
                                             SOIL_LOAD_AUTO,
                                             SOIL_CREATE_NEW_ID,
                                             SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    if (texHandle == 0)
    {
        printf("[ERROR]: Could not load texture \"%s\"\n[SOIL]: %s\n", filename, SOIL_last_result());
    }
    else
    {
        printf("[INFO]: Successfully loaded texture \"%s\"\n[SOIL]: %s\n", filename, SOIL_last_result());
        glBindTexture(GL_TEXTURE_2D, texHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    return texHandle;
}

bool registerOpenGLTexture(unsigned char *textureData, unsigned int texWidth, unsigned int texHeight, GLuint &textureHandle)
{
    if (textureData == 0)
    {
        fprintf(stderr, "Cannot register texture; no data specified.");
        return false;
    }

    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    return true;
}

//******************************************************************************
//
// Event Callbacks

// error_callback() ////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
////////////////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char *description)
{
    fprintf(stderr, "[ERROR]: %s\n", description);
}

void scaleModel()
{
    if (scaleFactor.y > 1.5 || scaleFactor.y < 0.5)
    {
        incrementor = -incrementor;
    }
    scaleFactor.y += incrementor;
}

// key_callback() //////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's keypress callback.
//	Responds to key presses and key releases
//
////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE || key == 'Q') && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS or action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        /*
        case GLFW_KEY_W:
            location += glm::vec3(0.15f * sin(rota * 3.14f / 180.0f), 0, 0.15f * cos(rota * 3.14f / 180.0f));
            break;
        case GLFW_KEY_S:
            location -= glm::vec3(0.15f * sin(rota * 3.14f / 180.0f), 0, 0.15f * cos(rota * 3.14f / 180.0f));
            break; 
        case GLFW_KEY_A:
            rota += 10.0f;
            if (rota > 360.0f)
            {
                rota = 0.0f;
            }
            break;
        case GLFW_KEY_D:
            rota -= 10.0f;
            if (rota < 0.0f)
            {
                rota = 360.0f;
            }
            break;
            */
        case GLFW_KEY_1:
            if (fpv)
            {
                fpv = false;
                cameraAngles = oldCamAngles;
                convertSphericalToCartesian();
            }
            break;
        case GLFW_KEY_2:
            if (!fpv)
            {
                fpv = true;
                oldCamAngles = cameraAngles;
            }
            break;
        case GLFW_KEY_A:
            ml = true;
            break;
        case GLFW_KEY_D:
            mr = true;
            break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        ml = false;
        mr = false;
    }
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//		We will register this function as GLFW's mouse button callback.
//	Responds to mouse button presses and mouse button releases.  Keeps track if
//	the control key was pressed when a left mouse click occurs to allow
//	zooming of our arcball camera.
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        leftMouseDown = true;
    }
    else
    {
        leftMouseDown = false;
        mousePosition.x = -9999.0f;
        mousePosition.y = -9999.0f;
    }
    controlDown = mods & GLFW_MOD_CONTROL;
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's cursor movement callback.
//	Responds to mouse movement.  When active motion is used with the left
//	mouse button an arcball camera model is followed.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
    // make sure movement is in bounds of the window
    // glfw captures mouse movement on entire screen
    if (xpos > 0 && xpos < windowWidth)
    {
        if (ypos > 0 && ypos < windowHeight)
        {
            // active motion
            if (leftMouseDown)
            {
                if ((mousePosition.x - -9999.0f) < 0.001f)
                {
                    mousePosition.x = xpos;
                    mousePosition.y = ypos;
                }
                else
                {
                    if (!controlDown)
                    {
                        cameraAngles.x += (xpos - mousePosition.x) * 0.005f;
                        cameraAngles.y += (ypos - mousePosition.y) * 0.005f;

                        if (cameraAngles.y < 0)
                            cameraAngles.y = 0.0f + 0.001f;
                        if (cameraAngles.y >= M_PI)
                            cameraAngles.y = M_PI - 0.001f;
                    }
                    else
                    {
                        double totChgSq = (xpos - mousePosition.x) + (ypos - mousePosition.y);
                        cameraAngles.z += totChgSq * 0.1f;

                        if (cameraAngles.z <= 2.0f)
                            cameraAngles.z = 2.0f;
                        if (cameraAngles.z >= 100.0f)
                            cameraAngles.z = 100.0f;
                    }
                    convertSphericalToCartesian();

                    mousePosition.x = xpos;
                    mousePosition.y = ypos;
                }
            }
            // passive motion
            else
            {
            }
        }
    }
}

// scroll_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's scroll wheel callback.
//	Responds to movement of the scroll where.  Allows zooming of the arcball
//	camera.
//
////////////////////////////////////////////////////////////////////////////////
static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    double totChgSq = yOffset;
    cameraAngles.z += totChgSq * 0.01f;

    if (cameraAngles.z <= 2.0f)
        cameraAngles.z = 2.0f;
    if (cameraAngles.z >= 50.0f)
        cameraAngles.z = 50.0f;

    convertSphericalToCartesian();
}

//******************************************************************************
//
// Setup Functions

// setupGLFW() /////////////////////////////////////////////////////////////////
//
//		Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
////////////////////////////////////////////////////////////////////////////////
GLFWwindow *setupGLFW()
{
    // set what function to use when registering errors
    // this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
    // all other GLFW calls must be performed after GLFW has been initialized
    glfwSetErrorCallback(error_callback);

    // initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "[ERROR]: Could not initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "[INFO]: GLFW initialized\n");
    }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // request forward compatible OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // request OpenGL Core Profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // request OpenGL 3.x context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // request OpenGL 3.3 context

    // create a window for a given size, with a given title
    GLFWwindow *window = glfwCreateWindow(640, 480, "Aaru 3.0 - 4.0", NULL, NULL);
    if (!window)
    { // if the window could not be created, NULL is returned
        fprintf(stderr, "[ERROR]: GLFW Window could not be created\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "[INFO]: GLFW Window created\n");
    }

    glfwMakeContextCurrent(window); // make the created window the current window
    glfwSwapInterval(1);            // update our screen after at least 1 screen refresh

    glfwSetKeyCallback(window, key_callback);                  // set our keyboard callback function
    glfwSetMouseButtonCallback(window, mouse_button_callback); // set our mouse button callback function
    glfwSetCursorPosCallback(window, cursor_callback);         // set our cursor position callback function
    glfwSetScrollCallback(window, scroll_callback);            // set our scroll wheel callback function

    return window; // return the window that was created
}

// setupOpenGL() ///////////////////////////////////////////////////////////////
//
//      Used to setup everything OpenGL related.
//
////////////////////////////////////////////////////////////////////////////////
void setupOpenGL()
{
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS);    // use less than depth test

    glFrontFace( GL_CCW );

    glEnable(GL_BLEND);                                // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use one minus blending equation

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the frame buffer to black
}

// setupGLEW() /////////////////////////////////////////////////////////////////
//
//      Used to initialize GLEW
//
////////////////////////////////////////////////////////////////////////////////
void setupGLEW()
{
    // LOOKHERE #1B
    glewExperimental = GL_TRUE;
    GLenum glewResult = glewInit();

    /* check for an error */
    if (glewResult != GLEW_OK)
    {
        printf("[ERROR]: Error initalizing GLEW\n");
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "[ERROR]: %s\n", glewGetErrorString(glewResult));
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "[INFO]: GLEW initialized\n");
        fprintf(stdout, "[INFO]: Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    }

    if (!glewIsSupported("GL_VERSION_2_0"))
    {
        printf("[ERROR]: OpenGL not version 2.0+.  GLSL not supported\n");
        exit(EXIT_FAILURE);
    }
}

// setupShaders() //////////////////////////////////////////////////////////////
//
//      Create our shaders.  Send GLSL code to GPU to be compiled.  Also get
//  handles to our uniform and attribute locations.
//
////////////////////////////////////////////////////////////////////////////////
void setupShaders(const char *vertexShaderFilename, const char *fragmentShaderFilename)
{
    shaderProgramHandle = createShaderProgram(vertexShaderFilename, fragmentShaderFilename);
    mvp_uniform_location = glGetUniformLocation(shaderProgramHandle, "mvpMatrix");
    vpos_attrib_location = glGetAttribLocation(shaderProgramHandle, "vPosition");
    tex_attrib_location = glGetAttribLocation(shaderProgramHandle, "tCoord");

    postprocessingShaderProgram = new CSCI441::ShaderProgram("shaders/grayscale.v.glsl", "shaders/grayscale.f.glsl");
    uniform_post_proj_loc = postprocessingShaderProgram->getUniformLocation("projectionMtx");
    uniform_post_fbo_loc = postprocessingShaderProgram->getUniformLocation("fbo");
    uniform_post_modelLocation = postprocessingShaderProgram->getUniformLocation("location");
    attrib_post_vpos_loc = postprocessingShaderProgram->getAttributeLocation("vPos");
    attrib_post_vtex_loc = postprocessingShaderProgram->getAttributeLocation("vTexCoord");
}

void setupObjectShaders(const char *vertexShaderFilename, const char *fragmentShaderFilename)
{
    objectShaderProgram = createShaderProgram(vertexShaderFilename, fragmentShaderFilename);
    MVPloc = glGetUniformLocation(objectShaderProgram, "mvpMatrix");
    VALloc = glGetAttribLocation(objectShaderProgram, "vPosition");
    NALloc = glGetAttribLocation(objectShaderProgram, "normal");
    TALloc = glGetAttribLocation(objectShaderProgram, "tCoord");
    ambient = glGetUniformLocation(objectShaderProgram, "ambient");
    diffuse = glGetUniformLocation(objectShaderProgram, "diffuse");
    specular = glGetUniformLocation(objectShaderProgram, "specular");
    LPUloc = glGetUniformLocation(objectShaderProgram, "lightPos");
    liAmbient = glGetUniformLocation(objectShaderProgram, "lightambient");
    liDiffuse = glGetUniformLocation(objectShaderProgram, "lightdiffuse");
    liSpecular = glGetUniformLocation(objectShaderProgram, "lightspecular");
    shine = glGetUniformLocation(objectShaderProgram, "shine");
    camUniform = glGetUniformLocation(objectShaderProgram, "cameraPos");
    objModelMtx = glGetUniformLocation(objectShaderProgram, "modelMtx");
    objViewMtx = glGetUniformLocation(objectShaderProgram, "viewMtx");
    objNormMtx = glGetUniformLocation(objectShaderProgram, "normMtx");
}

void setupBillboardShaders()
{
    // LOOKHERE #1
    billboardShaderProgram = new CSCI441::ShaderProgram("shaders/billboardQuadShader.v.glsl",
                                                        "shaders/billboardQuadShader.g.glsl",
                                                        "shaders/billboardQuadShader.f.glsl");
    bbmvp = billboardShaderProgram->getUniformLocation("mvMatrix");
    bbproj = billboardShaderProgram->getUniformLocation("projMatrix");
    bbvpos = billboardShaderProgram->getAttributeLocation("vPos");
    bbsize = billboardShaderProgram->getUniformLocation("size");
}

void setupMarbleShaders()
{
    marbleShaderProgram = new CSCI441::ShaderProgram("shaders/marbleShader.v.glsl", "shaders/marbleShader.f.glsl");
    uniform_modelMtx_loc = marbleShaderProgram->getUniformLocation("modelMtx");
    uniform_viewMtx_loc = marbleShaderProgram->getUniformLocation("viewMtx");
    uniform_viewProjetionMtx_loc = marbleShaderProgram->getUniformLocation("viewProjectionMtx");
    uniform_tex_loc = marbleShaderProgram->getUniformLocation("tex");
    uniform_color_loc = marbleShaderProgram->getUniformLocation("color");
    attrib_vPos_loc = marbleShaderProgram->getAttributeLocation("vPosition");
    attrib_normal_loc = marbleShaderProgram->getAttributeLocation("normal");
    attrib_vTextureCoord_loc = marbleShaderProgram->getAttributeLocation("vTextureCoord");
    uniform_normalMtx_loc = marbleShaderProgram->getUniformLocation("normalMtx");
    uniform_light_pos = marbleShaderProgram->getUniformLocation("lightPosition");
    uniform_light_ambient = marbleShaderProgram->getUniformLocation("lightAmbient");
    uniform_light_diffuse = marbleShaderProgram->getUniformLocation("lightDiffuse");
    uniform_light_specular = marbleShaderProgram->getUniformLocation("lightSpecular");
    uniform_light_shine = marbleShaderProgram->getUniformLocation("shine");
    uniform_camera_pos = marbleShaderProgram->getUniformLocation("cameraPosition");
}

// setupTextures() /////////////////////////////////////////////////////////////
//
//      Load and register all the tetures for our program
//
////////////////////////////////////////////////////////////////////////////////
void setupTextures()
{
    negyTextureHandle = loadAndRegisterTexture("textures/negy.jpg");
    posyTextureHandle = loadAndRegisterTexture("textures/posy.jpg");
    negxTextureHandle = loadAndRegisterTexture("textures/negx.jpg");
    posxTextureHandle = loadAndRegisterTexture("textures/posx.jpg");
    negzTextureHandle = loadAndRegisterTexture("textures/negz.jpg");
    poszTextureHandle = loadAndRegisterTexture("textures/posz.jpg");
    platformTextureHandle = loadAndRegisterTexture("textures/asphalt.jpg");
    particleHandle = loadAndRegisterTexture("textures/fire.png");
    brickTexHandle = loadAndRegisterTexture("textures/Stones.jpg");
}

// setupBuffers() //////////////////////////////////////////////////////////////
//
//      Create our VAOs & VBOs. Send vertex data to the GPU for future rendering
//
////////////////////////////////////////////////////////////////////////////////
void setupBuffers(const char *objt)
{
    // IMPORTANT
    model = new CSCI441::ModelLoader();
    model->loadModelFile(objt);
    //suzModelMtx = glm::rotate(suzModelMtx, 180.0f * 3.14f / 180.0f, glm::vec3(0, 1, 0));

    struct VertexTextured
    {
        float x, y, z, s, t;
    };
    float s = 1000.0f;

    // Negative Y
    struct VertexTextured negy[4] = {
        {-s, -s, -s, 0.0f, 0.0f},
        {s, -s, -s, 1.0f, 0.0f},
        {s, -s, s, 1.0f, 1.0f},
        {-s, -s, s, 0.0f, 1.0f}};

    unsigned short indexnegy[6] = {3, 1, 0, 3, 2, 1};

    glGenVertexArrays(1, &vaodnegy);
    if (vaodnegy == 0)
        return;
    glBindVertexArray(vaodnegy);

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbod);

    glBufferData(GL_ARRAY_BUFFER, sizeof(negy), negy, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_attrib_location);
    glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

    glEnableVertexAttribArray(tex_attrib_location);
    glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexnegy), indexnegy, GL_STATIC_DRAW);

    // Positive Y
    struct VertexTextured posy[4] = {
        {-s, s, -s, 0.0f, 1.0f},
        {s, s, -s, 1.0f, 1.0f},
        {s, s, s, 1.0f, 0.0f},
        {-s, s, s, 0.0f, 0.0f}};

    unsigned short indexposy[6] = {0, 1, 3, 1, 2, 3};

    glGenVertexArrays(1, &vaodposy);
    if (vaodposy == 0)
        return;
    glBindVertexArray(vaodposy);

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbod);

    glBufferData(GL_ARRAY_BUFFER, sizeof(posy), posy, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_attrib_location);
    glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

    glEnableVertexAttribArray(tex_attrib_location);
    glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexposy), indexposy, GL_STATIC_DRAW);

    struct VertexTextured negx[4] = {
        {-s, -s, -s, 0.0f, 0.0f},
        {-s, s, -s, 0.0f, 1.0f},
        {-s, s, s, 1.0f, 1.0f},
        {-s, -s, s, 1.0f, 0.0f}};

    unsigned short indexnegx[6] = {3, 1, 0, 3, 2, 1};

    glGenVertexArrays(1, &vaodnegx);
    if (vaodnegx == 0)
        return;
    glBindVertexArray(vaodnegx);

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbod);

    glBufferData(GL_ARRAY_BUFFER, sizeof(negx), negx, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_attrib_location);
    glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

    glEnableVertexAttribArray(tex_attrib_location);
    glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexnegx), indexnegx, GL_STATIC_DRAW);

    struct VertexTextured posx[4] = {
        {s, -s, -s, 1.0f, 0.0f},
        {s, s, -s, 1.0f, 1.0f},
        {s, s, s, 0.0f, 1.0f},
        {s, -s, s, 0.0f, 0.0f}};

    unsigned short indexposx[6] = {0, 1, 3, 1, 2, 3};

    glGenVertexArrays(1, &vaodposx);
    if (vaodposx == 0)
        return;
    glBindVertexArray(vaodposx);

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbod);

    glBufferData(GL_ARRAY_BUFFER, sizeof(posx), posx, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_attrib_location);
    glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

    glEnableVertexAttribArray(tex_attrib_location);
    glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexposx), indexposx, GL_STATIC_DRAW);

    struct VertexTextured negz[4] = {
        {-s, -s, -s, 1.0f, 0.0f},
        {s, -s, -s, 0.0f, 0.0f},
        {s, s, -s, 0.0f, 1.0f},
        {-s, s, -s, 1.0f, 1.0f}};

    unsigned short indexnegz[6] = {0, 1, 3, 1, 2, 3};

    glGenVertexArrays(1, &vaodnegz);
    if (vaodnegz == 0)
        return;
    glBindVertexArray(vaodnegz);

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbod);

    glBufferData(GL_ARRAY_BUFFER, sizeof(negz), negz, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_attrib_location);
    glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

    glEnableVertexAttribArray(tex_attrib_location);
    glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexnegz), indexnegz, GL_STATIC_DRAW);

    struct VertexTextured posz[4] = {
        {-s, -s, s, 0.0f, 0.0f},
        {s, -s, s, 1.0f, 0.0f},
        {s, s, s, 1.0f, 1.0f},
        {-s, s, s, 0.0f, 1.0f}};

    unsigned short indexposz[6] = {0, 3, 1, 3, 2, 1};

    glGenVertexArrays(1, &vaodposz);
    if (vaodposz == 0)
        return;
    glBindVertexArray(vaodposz);

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbod);

    glBufferData(GL_ARRAY_BUFFER, sizeof(posz), posz, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_attrib_location);
    glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

    glEnableVertexAttribArray(tex_attrib_location);
    glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    glGenBuffers(1, &vbod);
    if (vbod == 0)
        return;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexposz), indexposz, GL_STATIC_DRAW);

    // Platform

    for (unsigned int i = 0; i < numPlatform; i++)
    {
        float t = 12 * i * 2;
        // TODO #02: create our platform vertex array
        struct VertexTextured VertexArray[4] = {
            {-12.0f, 0.0f, t - 12.0f, 0.0f, 0.0f},
            {12.0f, 0.0f, t - 12.0f, 1.0f, 0.0f},
            {12.0f, 0.0f, t + 12.0f, 1.0f, 1.0f},
            {-12.0f, 0.0f, t + 12.0f, 0.0f, 1.0f}};

        // TODO #03: create the index array
        unsigned short indexArray[6] = {0, 1, 2, 2, 3, 0};

        // TODO #04B: generate and bind the VAO
        glGenVertexArrays(1, &vaod[i]);
        if (vaod[i] == 0)
        {
            return;
        }
        glBindVertexArray(vaod[i]);

        // TODO #05A: generate and bind the array VBO
        glGenBuffers(1, &vbod);
        glBindBuffer(GL_ARRAY_BUFFER, vbod);
        if (vbod == 0)
        {
            return;
        }

        // TODO #05B: send the vertex data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexArray), VertexArray, GL_STATIC_DRAW);

        // TODO #06A: state where the vertex position is located within our array data
        glEnableVertexAttribArray(vpos_attrib_location);
        glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

        // TODO #06B: state where the texture coordinate is located within our array data
        glEnableVertexAttribArray(tex_attrib_location);
        glVertexAttribPointer(tex_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

        // TODO #07: generate and bind the element array VBO.  send data to the GPU
        glGenBuffers(1, &vbod);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbod);
        if (vbod == 0)
        {
            return;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
    }

    // For points

    glGenVertexArrays(1, &pointsVAO);
    glBindVertexArray(pointsVAO);

    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(bbvpos);
    glVertexAttribPointer(bbvpos, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // Post processing
    struct VertexTextured texturedQuadVerts[4] = {
        {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}, // 0 - BL
        {1.0f, -1.0f, 0.0f, 1.0f, 0.0f},  // 1 - BR
        {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f},  // 2 - TL
        {1.0f, 1.0f, 0.0f, 1.0f, 1.0f}    // 3 - TR
    };

    unsigned short texturedQuadIndices[4] = {0, 1, 2, 3};
    GLuint postvbods[2];
    glGenVertexArrays(1, &texturedQuadVAO);
    glBindVertexArray(texturedQuadVAO);
    glGenBuffers(2, postvbods);
    glBindBuffer(GL_ARRAY_BUFFER, postvbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedQuadVerts), texturedQuadVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, postvbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texturedQuadIndices), texturedQuadIndices, GL_STATIC_DRAW);
    postprocessingShaderProgram->useProgram();
    glEnableVertexAttribArray(attrib_post_vpos_loc);
    glVertexAttribPointer(attrib_post_vpos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);
    glEnableVertexAttribArray(attrib_post_vtex_loc);
    glVertexAttribPointer(attrib_post_vtex_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));
}

void setupFramebuffer()
{
    // TODO #1 - Setup everything with the framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create Framebuffer Object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // Attach Renderbuffer Objects
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Attach Texture Image to FBO
    glGenTextures(1, &framebufferTextureHandle);
    glBindTexture(GL_TEXTURE_2D, framebufferTextureHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTextureHandle, 0);

    CSCI441::FramebufferUtils::printFramebufferStatusMessage(GL_FRAMEBUFFER);
    CSCI441::FramebufferUtils::printFramebufferInfo(GL_FRAMEBUFFER, fbo);
}

//******************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// renderScene() ///////////////////////////////////////////////////////////////
//
//		This method will contain all of the objects to be drawn.
//
////////////////////////////////////////////////////////////////////////////////

void drawSkybox()
{
    glBindTexture(GL_TEXTURE_2D, negyTextureHandle);
    glBindVertexArray(vaodnegy);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

    glBindTexture(GL_TEXTURE_2D, posyTextureHandle);
    glBindVertexArray(vaodposy);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

    glBindTexture(GL_TEXTURE_2D, negxTextureHandle);
    glBindVertexArray(vaodnegx);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

    glBindTexture(GL_TEXTURE_2D, posxTextureHandle);
    glBindVertexArray(vaodposx);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

    glBindTexture(GL_TEXTURE_2D, negzTextureHandle);
    glBindVertexArray(vaodnegz);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

    glBindTexture(GL_TEXTURE_2D, poszTextureHandle);
    glBindVertexArray(vaodposz);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

    for (unsigned int i = 0; i < numPlatform; i++)
    {
        glBindTexture(GL_TEXTURE_2D, platformTextureHandle);
        glBindVertexArray(vaod[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);
    }
}

void drawSuzanne(glm::mat4 viewMtx, glm::mat4 projMtx)
{
    glm::mat4 modelMtx;
    suzModelMtx = glm::translate(suzModelMtx, location);
    suzModelMtx = glm::rotate(suzModelMtx, rota * 3.14f / 180.0f, glm::vec3(0, 1, 0));
    suzModelMtx = glm::scale(suzModelMtx, scaleFactor);
    lookAtPoint = location;
    suzPoint = location + eyePoint;

    // precompute our MVP CPU side so it only needs to be computed once
    glm::mat4 suzmvpMtx = projMtx * viewMtx * suzModelMtx;

    glUseProgram(objectShaderProgram);
    glUniformMatrix4fv(MVPloc, 1, GL_FALSE, &suzmvpMtx[0][0]);
    glUniformMatrix4fv(objModelMtx, 1, GL_FALSE, &suzModelMtx[0][0]);
    glUniformMatrix4fv(objViewMtx, 1, GL_FALSE, &viewMtx[0][0]);

    glm::mat4 normalMtx = glm::transpose(glm::inverse(viewMtx * suzModelMtx));
    glUniformMatrix4fv(objNormMtx, 1, GL_FALSE, &normalMtx[0][0]);
    suzModelMtx = modelMtx;

    glUniform3f(ambient, 0.135f, 0.2225f, 0.1575f);
    glUniform3f(diffuse, 0.54f, 0.89f, 0.63f);
    glUniform3f(specular, 0.316228f, 0.316228f, 0.316228f);

    glUniform3f(LPUloc, 0.0f, 0.0f, 1000.0f);

    glUniform3f(liAmbient, 1.0f, 1.0f, 1.0f);
    glUniform3f(liDiffuse, 1.0f, 1.0f, 1.0f);
    glUniform3f(liSpecular, 1.0f, 1.0f, 1.0f);
    glUniform1f(shine, 12.0f);

    glUniform3f(camUniform, suzPoint.x, suzPoint.y, suzPoint.z);

    model->draw(VALloc, NALloc, TALloc, diffuse, specular, shine, ambient);
}

void spawnAndUpdate()
{
    unsigned int sp[spawners.size()];
    for (unsigned int i = 0; i < spawners.size(); i++)
    {
        sp[i] = 0;
        double end = glfwGetTime();
        // Add new particle
        if (end - spawners.at(i).start > spawners.at(i).spawn)
        {
            if (((float)rand() / (float)RAND_MAX * 3) > 2)
            {
                //spawners.at(i).add();
                sp[i] = 1;
            }
            spawners.at(i).start = end;
        }

        double updateEnd = glfwGetTime();

        // Updating particles' position
        if (updateEnd - spawners.at(i).updateStart > 0.01)
        {
            spawners.at(i).update(location);
            spawners.at(i).updateStart = updateEnd;
        }
    }

    int g1 = 0;
    int g2 = 0;
    for (int i = 0; i < spawners.size(); i++)
    {
        if (sp[i] == 1 && i < 3)
            g1++;
        else if (sp[i] == 1 && i > 2)
            g2++;
    }

    if (g1 == 3)
    {
        int i = rand() % 3;
        sp[i] = 0;
    }
    if (g2 == 3)
    {
        int i = rand() % 3;
        sp[i + 3] = 0;
    }

    for (unsigned int i = 0; i < spawners.size(); i++)
    {
        if (spawners.at(i).startingLoc.z - location.z > 100.0f && sp[i] == 1)
        {
            spawners.at(i).add();
        }
    }
}

void drawParticleSystem(glm::mat4 viewMtx, glm::mat4 projMtx)
{
    // precompute our MVP CPU side so it only needs to be computed once
    glm::mat4 modelMtx;
    glm::mat4 mvMtx = viewMtx * modelMtx;

    // send MVP to GPU
    glUniformMatrix4fv(bbmvp, 1, GL_FALSE, &mvMtx[0][0]);
    glUniformMatrix4fv(bbproj, 1, GL_FALSE, &projMtx[0][0]);

    for (unsigned int i = 0; i < psystems.size(); i++)
    {
        double end = glfwGetTime();
        // Add new particle
        if (end - psystems.at(i).start > psystems.at(i).spawn)
        {
            psystems.at(i).add();
            psystems.at(i).start = end;
        }

        double updateEnd = glfwGetTime();

        // Updating particles' position
        if (updateEnd - psystems.at(i).updateStart > 0.01)
        {
            psystems.at(i).update();
            psystems.at(i).updateStart = updateEnd;
        }
    }

    // The following sorts ParticleSystem from camera for billboarding
    glm::vec3 viewVec = lookAtPoint - suzPoint;
    glm::normalize(viewVec);

    int orderedIndices[psystems.size()];
    double distances[psystems.size()];

    for (unsigned int i = 0; i < psystems.size(); i++)
    {
        Vertex point = psystems.at(i).startingLoc;
        glm::vec4 p = glm::vec4(point.x, point.y, point.z, 1);
        p = p * modelMtx;
        glm::vec4 ep = p - glm::vec4(suzPoint, 1);

        orderedIndices[i] = i;
        distances[i] = glm::dot(glm::vec4(viewVec, 0), ep);
    }

    for (unsigned int i = 0; i < psystems.size(); i++)
    {
        for (unsigned int j = 0; j < psystems.size(); j++)
        {
            if (distances[j] <= distances[i])
            {
                double temp = distances[i];
                int tempLoc = orderedIndices[i];

                orderedIndices[i] = orderedIndices[j];
                orderedIndices[j] = tempLoc;

                distances[i] = distances[j];
                distances[j] = temp;
            }
        }
    }

    vector<ParticleSystem> orderedPoints;

    for (unsigned int i = 0; i < psystems.size(); i++)
    {
        orderedPoints.push_back(psystems.at(orderedIndices[i]));
    }

    for (unsigned int i = 0; i < orderedPoints.size(); i++)
    {
        orderedPoints.at(i).draw(lookAtPoint, suzPoint, points, bbsize, particleHandle, pointsVAO, pointsVBO);
    }
}

void checkEnemiesCollision()
{
    for (int i = 0; i < spawners.size(); i++)
    {
        if (spawners.at(i).enemies.size() < 2)
        {
            return;
        }
        for (int j = 0; j < spawners.at(i).enemies.size() - 1; j++)
        {
            for (int k = j + 1; k < spawners.at(i).enemies.size(); k++)
            {
                Marble *m1 = spawners.at(i).enemies.at(j);
                Marble *m2 = spawners.at(i).enemies.at(k);
                if (glm::distance(m1->location, m2->location) < m1->radius + m2->radius)
                {
                    int ta = j;
                    int tb = k;
                    spawners.at(i).enemies.erase(spawners.at(i).enemies.begin() + tb);
                    spawners.at(i).enemies.erase(spawners.at(i).enemies.begin() + ta);
                    if (spawners.at(i).enemies.size() < 2)
                    {
                        return;
                    }
                }
            }
        }
    }
}

void checkDeaths()
{
    if (location.x > 12.0 || location.x < -12.0)
    {
        MessageBox(NULL, "You fell off!", "R.I.P", MB_OK);
        exit(EXIT_SUCCESS);
    }

    for (unsigned int i = 0; i < spawners.size(); i++)
    {
        for (unsigned int j = 0; j < spawners.at(i).enemies.size(); j++)
        {
            if (glm::distance(location, spawners.at(i).enemies.at(j)->location) < spawners.at(i).enemies.at(j)->radius)
            {
                MessageBox(NULL, "The enemy got you!", "R.I.P", MB_OK);
                exit(EXIT_SUCCESS);
            }
        }
    }
}

void renderScene(glm::mat4 viewMtx, glm::mat4 projMtx)
{
    if (fpv)
    {
        cameraAngles = glm::vec3(0.00f, 3.14f, 100.0f);
        convertSphericalToCartesian();
    }
    // Updating location of player
    glm::mat4 modelMtx;
    location = location + glm::vec3(0, 0, 0.25f);
    if (ml)
    {
        location = location + glm::vec3(0.75f, 0, 0);
    }
    if (mr)
    {
        location = location - glm::vec3(0.75f, 0, 0);
    }

    glUseProgram(shaderProgramHandle);
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    glUniformMatrix4fv(mvp_uniform_location, 1, GL_FALSE, &mvpMtx[0][0]);
    drawSkybox();
    drawSuzanne(viewMtx, projMtx);

    marbleShaderProgram->useProgram();
    CSCI441::setVertexAttributeLocations(attrib_vPos_loc, attrib_normal_loc, attrib_vTextureCoord_loc);
    glm::mat4 m, vp = projMtx * viewMtx;
    //glm::mat4 normalMtx = glm::transpose(glm::inverse(m * viewMtx));
    //glUniformMatrix4fv(uniform_normalMtx_loc, 1, GL_FALSE, &normalMtx[0][0]);
    //glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &m[0][0]);
    glUniformMatrix4fv(uniform_viewMtx_loc, 1, GL_FALSE, &viewMtx[0][0]);
    glUniformMatrix4fv(uniform_viewProjetionMtx_loc, 1, GL_FALSE, &vp[0][0]);
    glUniform1ui(uniform_tex_loc, GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexHandle);

    glUniform3f(uniform_light_pos, 0.0f, 0.0f, 1000.0f);
    glUniform3f(uniform_light_ambient, 0.3f, 0.3f, 0.3f);
    glUniform3f(uniform_light_diffuse, 0.7f, 0.7f, 0.7f);
    glUniform3f(uniform_light_specular, 1.0f, 1.0f, 1.0f);
    glUniform1f(uniform_light_shine, 10.0f);
    glUniform3f(uniform_camera_pos, suzPoint.x, suzPoint.y, suzPoint.z);

    glm::vec3 white(1, 1, 1);
    glUniform3fv(uniform_color_loc, 1, &white[0]);
    spawnAndUpdate();

    for (unsigned int i = 0; i < spawners.size(); i++)
    {
        spawners.at(i).draw(m, viewMtx, uniform_modelMtx_loc, uniform_color_loc, uniform_normalMtx_loc);
    }

    // Following is for particle system
    billboardShaderProgram->useProgram();

    drawParticleSystem(viewMtx, projMtx);
}

///*****************************************************************************
//
// Our main function

// main() ///////////////////////////////////////////////////////////////
//
//		Really you should know what this is by now.
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // ensure proper number of arguments provided at runtime
    /*if (argc != 2)
    {
        // we need a vertex and fragment shader
        fprintf(stderr, "Usage: ./a7.exe input.csv\n");
        exit(EXIT_FAILURE);
    } */
    srand(time(NULL));

    // GLFW sets up our OpenGL context so must be done first
    GLFWwindow *window = setupGLFW(); // initialize all of the GLFW specific information releated to OpenGL and our window
    setupOpenGL();                    // initialize all of the OpenGL specific information
    // LOOKHERE #1A
    setupGLEW(); // initialize all of the GLEW specific information

    CSCI441::OpenGLUtils::printOpenGLInfo();

    setupShaders("shaders/shader.v.glsl", "shaders/shader.f.glsl"); // load our shader program into memory
    setupObjectShaders("shaders/objectShader.v.glsl", "shaders/objectShader.f.glsl");
    setupBillboardShaders();
    setupBuffers("models/suzanne.obj"); // load all our VAOs and VBOs into memory
    setupTextures();
    setupMarbleShaders();
    setupFramebuffer();
    readFile("systems.csv");
    // needed to connect our 3D Object Library to our shader
    // LOOKHERE #3
    CSCI441::setVertexAttributeLocations(vpos_attrib_location);

    convertSphericalToCartesian(); // set up our camera position
    CSCI441::setVertexAttributeLocations(attrib_vPos_loc, -1, attrib_vTextureCoord_loc);
    CSCI441::drawSolidSphere(1, 16, 16); // strange hack I need to make spheres draw - don't have time to investigate why..it's a bug with my library
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while (!glfwWindowShouldClose(window))
    {                                                       // check if the window was instructed to be closed
        glDrawBuffer(GL_BACK);                              // work with our back frame buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore
        // query what the actual size of the window we are rendering to is.
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        // update the viewport - tell OpenGL we want to render to the whole window
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 2000].
        glm::mat4 projectionMatrix = glm::perspective(45.0f, windowWidth / (float)windowHeight, 0.001f, 2000.0f);

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix = glm::lookAt(suzPoint, lookAtPoint, upVector);

        // draw everything to the window
        // pass our view and projection matrices as well as deltaTime between frames
        renderScene(viewMatrix, projectionMatrix);

        glFlush();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 orthoMatrix = glm::ortho(-1, 1, -1, 1);
        postprocessingShaderProgram->useProgram();
        glUniformMatrix4fv(uniform_post_proj_loc, 1, GL_FALSE, &orthoMatrix[0][0]);
        glUniform3f(uniform_post_modelLocation, location.x, location.y, location.z);

        glBindTexture(GL_TEXTURE_2D, framebufferTextureHandle);
        glBindVertexArray(texturedQuadVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void *)0);

        //checkEnemiesCollision(); // Checks for enemy running into each other
        //checkDeaths();           // Checks if player death occurs

        glfwSwapBuffers(window); // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();        // check for any events and signal to redraw screen
    }

    glfwDestroyWindow(window); // clean up and close our window
    glfwTerminate();           // shut down GLFW to clean up our context

    return EXIT_SUCCESS;
}