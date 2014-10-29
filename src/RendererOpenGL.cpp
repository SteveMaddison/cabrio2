//
// OpenGL Renderer implementation.
//
#include "RendererOpenGL.hpp"
#include "Logger.hpp"

#include <ios>
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

// Vertices of two triangles, which together make a quad.
static const GLfloat g_vertex_quad[] = {
  // Bottom-left
  -1.0f,  1.0f, 0.0f,
  -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
  // Top-right
   1.0f, -1.0f, 0.0f,
   1.0f,  1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,
};


RendererOpenGL::RendererOpenGL() {

}

RendererOpenGL::~RendererOpenGL() {

}

GLuint LoadShaders(){
  Logger& logger = Logger::get_instance();

  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  std::string VertexShaderCode;
  VertexShaderCode += "#version 330 core\n";
  VertexShaderCode += "layout(location = 0) in vec3 vertexPosition_modelspace;\n";
  VertexShaderCode += "uniform mat4 MVP;\n";
  VertexShaderCode += "void main(){\n";
  VertexShaderCode += "  vec4 v = vec4(vertexPosition_modelspace,1);\n";
  VertexShaderCode += "  gl_Position = MVP * v;\n";
  VertexShaderCode += "}\n";

  std::string FragmentShaderCode;
  FragmentShaderCode += "#version 330 core\n";
  FragmentShaderCode += "out vec3 color;\n";
  FragmentShaderCode += "void main(){\n";
  FragmentShaderCode += "  color = vec3(1,0,0);\n";
  FragmentShaderCode += "}\n";

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    logger.log(LOG_ERROR, &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    logger.log(LOG_ERROR, &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    logger.log(LOG_ERROR, &ProgramErrorMessage[0]);
  }

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

int RendererOpenGL::init() {
  Logger& logger = Logger::get_instance();

  // Initialise GLFW
  if(!glfwInit()) {
    logger.log(LOG_ERROR, "Failed to initialise GLFW");
    return -1;
  }

  logger.log(LOG_INFO, "Initialised GLFW");

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Cabrio", NULL, NULL);
  if( window == NULL ){
    logger.log(LOG_ERROR, "Failed to open GLFW window (possible OpenGL version incompatibility)" );
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental=true;
  if (glewInit() != GLEW_OK) {
    logger.log(LOG_ERROR, "Failed to initialise GLEW");
    return -1;
  }

  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  program_id = LoadShaders();
  matrix_id = glGetUniformLocation(program_id, "MVP");

  return 0;
}

int RendererOpenGL::draw_all() {
  std::vector<Quad>::iterator q;

  q = this->quads.begin();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(program_id);

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

  // Camera matrix
  glm::mat4 View = glm::lookAt(
    glm::vec3(0,0,10), // Camera is at (0,0,10), in World Space
    glm::vec3(0,0,0),  // and looks at the origin
    glm::vec3(0,1,0)   // Head is up (set to 0,-1,0 to look upside-down)
  );

  while (q != this->quads.end()) {
    glm::mat4 Model = glm::mat4(1.0f);

    Model = glm::scale(Model, glm::vec3(q->size_x, q->size_y, 1.0f));
    Model = glm::rotate(Model, q->rotation_x, glm::vec3(1, 0, 0));
    Model = glm::rotate(Model, q->rotation_y, glm::vec3(0, 1, 0));
    Model = glm::rotate(Model, q->rotation_z, glm::vec3(0, 0, 1));
    Model = glm::translate(Model, glm::vec3(q->x, q->y, q->z));

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = Projection * View * Model;

    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &MVP[0][0]);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_quad), g_vertex_quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
       0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);

    q++;
  }

  glfwSwapBuffers(window);

  return 0;
}
