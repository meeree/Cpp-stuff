//GLEW and GLFW
#include <GL/glew.h>
#include <GL/glfw.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

//GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

void window(GLfloat render_dis, GLfloat x, GLfloat y, GLfloat z, int const &width, int const &height, float const &bgr=0.4*255, float const &bgg=0.4*255, float const &bgb=0.4*255) {
   projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, render_dis); 
   view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   if ( !glfwInit()) {
      std::cerr << "Failed to initialize GLFW! I'm out!" << std::endl;
      exit(-1);
   }

   glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);

   if( !glfwOpenWindow(width, height, 8, 8, 8, 0, 8, 0, GLFW_WINDOW)) {
      std::cerr << "Failed to open a window! I'm out!" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   int major, minor, rev;
   glfwGetGLVersion(&major, &minor, &rev);
   std::cout << "OpenGL - " << major << "." << minor << "." << rev << std::endl;

   glewExperimental = GL_TRUE;
   if(glewInit() != GLEW_OK) {
      std::cerr << "Failed to initialize GLEW! I'm out!" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   glClearColor(bgr/255, bgg/255, bgb/255, 1);
   glClear(GL_COLOR_BUFFER_BIT);
   glfwSwapBuffers();
}

GLuint load_and_compile_shader(const char *fname, GLenum shaderType) {
   std::vector<char> buffer;
   std::ifstream in;
   in.open(fname, std::ios::binary);

   if(in.is_open()) {
      in.seekg(0, std::ios::end);
      size_t length = (size_t)in.tellg();

      in.seekg(0, std::ios::beg);

      buffer.resize(length + 1);
      in.read(&buffer[0], length);
      in.close();
      buffer[length] = '\0';
   }
   else {
      std::cerr << "Unable to open " << fname << " I'm out!" << std::endl;
      exit(-1);
   }
   const char *src = &buffer[0];

   GLuint shader = glCreateShader(shaderType);
   glShaderSource(shader, 1, &src, NULL);
   glCompileShader(shader);
   GLint test;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
   if(!test) {
      std::cerr << "Shader compilation failed with this message:" << std::endl;
      std::vector<char> compilation_log(512);
      glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
      std::cerr << &compilation_log[0] << std::endl;
      glfwTerminate();
      exit(-1);
   }
   return shader;
}

GLuint shader(const char *path_vert_shader, const char *path_frag_shader) {
   GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
   GLuint fragmentShader = load_and_compile_shader(path_frag_shader, GL_FRAGMENT_SHADER);

   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   glLinkProgram(shaderProgram);
   glUseProgram(shaderProgram);

   return shaderProgram;
}

void initialize(std::vector<GLfloat> const &vertices, const &vao, GLuint const &vbo, float const &lnr=0, float const &lng=0, float const &lnb=0 ) {
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glGenBuffers(1, &vbo);
   GLuint shaderProgram = shader("shaders/vert.shader", "shaders/frag.shader");

   GLint position_attribute = glGetAttribLocation(shaderProgram, "position");
   GLint color_attribute = glGetAttribLocation(shaderProgram, "vert_color");
   PVM = glGetUniformLocation(shaderProgram, "PVM");

   glBindBuffer(GL_ARRAY_BUFFER, vbo);

   GLfloat colors [vertices.size()];
   int index = 0;
   for (int i = 0; i < vertices.size()/3; i++) {
      colors[index] = lnr;
      index++;
      colors[index] = lng;
      index++;
      colors[index] = lnb;
      index++;
   }

   glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (vertices.size() + colors.size()), NULL, GL_DYNAMIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices.size(), vertices.data());
   glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), sizeof(GLfloat) * colors.size(), colors.data());

   glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(GLfloat)*vertices.size()));

   glEnableVertexAttribArray(position_attribute);
   glEnableVertexAttribArray(color_attribute);
   
   index = 0;
   GLuint indices[2*(vertices.size()-1)];
   for (int i = 0; i < vertices.size()-1; i++) {
      indices[index] = vertices[i];
      index++;
      indices[index] = vertices[i];
      index++;
   }

   GLuint eab;
   glGenBuffers(1, &eab);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

}


void display(std::vector<GLfloat> vertices) {
   while(running) {
      glBindVertexArray(vao);
      glDrawElements( GL_LINES, 2*(vertices.size()-1), GL_UNSIGNED_INT, 0);
      glfwSwapBuffers();
   }
}


