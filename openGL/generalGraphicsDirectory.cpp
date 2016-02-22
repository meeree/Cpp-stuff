#ifndef GL
#include <GL/glew.h>
#include <GL/glfw.h>
#endif

#ifndef iostream
#include <iostream>
#endif

#ifndef fstream
#include <fstream>
#endif

#ifndef vector
#include <vector>
#endif

#define GLM_FORCE_RADIANS
#ifndef glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#endif

void initializePrograms(GLint const &width, GLint const &height, bool const &fullScreen=false, GLfloat const &backGroundRed=1, GLfloat const &backGroundGreen=1, GLfloat const &backGroundBlue=1, GLfloat const &backGroundAlpha=1, GLint const &redBits=8, GLint const &greenBits=8, GLint const &blueBits=8) {

   if ( !glfwInit()) { /* Initialize glfw */
      std::cerr << "Failed to initialize GLFW" << std::endl;
      exit(-1);
   }

   glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); /* Telling glfw openGL version is 3.2 */
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);

   if ((fullScreen) && (!glfwOpenWindow(width, height, redBits, greenBits, blueBits, 0, 8, 0, GLFW_FULLSCREEN)) ) { /* Open fullscreen glfw window */
      std::cerr << "Failed to open a window" << std::endl;
      glfwTerminate();
      exit(-1);
   } else if ( !glfwOpenWindow(width, height, redBits, greenBits, blueBits, 0, 8, 0, GLFW_WINDOW)) { /* Open windowed mode glfw window */
      std::cerr << "Failed to open a window" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK) { /* Initialize GLEW */
      std::cerr << "Failed to initialize GLEW" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   glClearColor(backGroundRed, backGroundGreen, backGroundBlue, backGroundAlpha); /* Setting background clear color */
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* Clearing color and depth buffers */
   glfwSwapBuffers(); /* Swapping buffers */
}

glm::mat4 setProjection(GLfloat const &render_dis, GLfloat const &fieldOfView=45) {  /* Sets projection matrix */
   return glm::perspective(glm::radians(fieldOfView), 1.0f, 0.1f, render_dis); 
}

glm::mat4 setView(GLfloat const &x, GLfloat const &y, GLfloat const &z) {  /* Sets view matrix */
   return glm::lookAt(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


GLuint loadInShader(char const *fname, GLenum const &shaderType) { /* Called by shaders function */
   std::vector<char> buffer;
   std::ifstream in;
   in.open(fname, std::ios::binary);

   if (in.is_open()) {
      in.seekg(0, std::ios::end);
      size_t const &length = in.tellg();

      in.seekg(0, std::ios::beg);

      buffer.resize(length + 1);
      in.read(&buffer[0], length);
      in.close();
      buffer[length] = '\0';
   } else {
      std::cerr << "Unable to open " << fname << std::endl;
      exit(-1);
   }

   GLchar const *src = &buffer[0];

   GLuint shader = glCreateShader(shaderType);
   glShaderSource(shader, 1, &src, NULL);
   glCompileShader(shader);
   GLint test;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

   if (!test) {
      std::cerr << "Shader compilation failed with this message:" << std::endl;
      std::vector<char> compilationLog(512);
      glGetShaderInfoLog(shader, compilationLog.size(), NULL, &compilationLog[0]);
      std::cerr << &compilationLog[0] << std::endl;
      glfwTerminate();
      exit(-1);
   }

   return shader;
}

GLuint shaders(char const *pathToVertShader, char const *pathToFragShader) { /* Loads in shaders to shader program and deletes their copies*/
   GLuint vertexShader = loadInShader(pathToVertShader, GL_VERTEX_SHADER); /* Compile shader configuration file to variable */ 
   GLuint fragmentShader = loadInShader(pathToFragShader, GL_FRAGMENT_SHADER); /* Compile shader configuration file to variable */

   GLuint shaderProgram = glCreateProgram(); /*Creating shader program*/
   glAttachShader(shaderProgram, vertexShader); /* Binds vertexShader to shader program */
   glAttachShader(shaderProgram, fragmentShader); /* Binds vertexShader to shader program */

   glDeleteShader(vertexShader); /* Deletes copy of vertexShader */
   glDeleteShader(fragmentShader); /* Deletes copy of fragmentShader */

   glLinkProgram(shaderProgram); /* Links shader program to openGL */
   glUseProgram(shaderProgram); /* Tells openGL we are using shaderProgram */

   return shaderProgram; /* Returning shaderProgram */
}

int main () {
   initializePrograms(100, 100);
   GLuint shaderProgram = shaders("./shader/vert.shader", "./shader/frag.shader");
}
