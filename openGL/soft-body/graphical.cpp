#include "physics.h"
#include "physics.cpp"

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

//_______________________________________GRAPHICS_________________________________________
//DESCRIPTION_____________________________________________________________________________

//________________________________________________________________________________________


template <class T>
class Graphics {
   GLuint vbo, vao;
   GLint PVM;
   glm::mat4 projection, model, view;
   float camera_pos[3];
   Simulate<T> sim;
   std::vector<GLfloat> colors, vertices;
   public:

   Graphics(GLfloat render_dis, GLfloat x, GLfloat y, GLfloat z) {
      projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, render_dis); 
      view = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   }

   void window(int width, int height, float bgr=0.4*255, float bgg=0.4*255, float bgb=0.4*255) {
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

   void universe(T time_step, T grav, T drag_force) {
      sim.init(time_step, std::vector<Object<T> > {}, std::vector<Spring<T> > {}, grav, drag_force);
   }

   void cloth (int n, T struc_k, T shear_k, T bend_k, T c, T m, T dis, std::string mode, std::vector<std::vector<float>> fixed = {}) {
      colors.resize((n+1)*(n+1)*3);
      vertices.resize((n+1)*(n+1)*3);
      if (mode == "free") {
         T vel[3] = {0, 0, 0};
         for (int j = 0; j < n+1; j++) {
            for (int i = 0; i < n+1; i++) {
              T pos[3] = {dis * (i-n/2), 0, dis * (j-n/2)};
              for (int k = 0; k < fixed.size(); k++) {
                  if ((fixed[k][0] == i) and (fixed[k][1] == j)) {
                     sim.objects.push_back(Object<T> (m, pos, vel, false));
                  }
                  if (k == fixed.size()-1) {
                     sim.objects.push_back(Object<T> (m, pos, vel, true));
                  }
              }

               if (i < n) {
                  sim.springs.push_back(Spring<T> {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}});
               }
               if (j < n) {
                  sim.springs.push_back(Spring<T> {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}});
               }
               if (i < n-1) {
                  sim.springs.push_back(Spring<T> {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}});
               }
               if (j < n-1) {
                  sim.springs.push_back(Spring<T> {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}});
               }
               if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}});
               }
            }
         }
      }
      if (mode == "square") {
         T vel[3] = {0, 0, 0};
         for (int j = 0; j < n+1; j++) {
            for (int i = 0; i < n+1; i++) {
              T pos[3] = {dis * (i-n/2), 0, dis * (j-n/2)};
              if ((i==0) or (i==n) or (j==n) or (j==0)) {
                  sim.objects.push_back(Object<T> (m, pos, vel, false));
               }
               else {
                  sim.objects.push_back(Object<T> (m, pos, vel, true));
               }

               if (i < n) {
                  sim.springs.push_back(Spring<T> {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}});
               }
               if (j < n) {
                  sim.springs.push_back(Spring<T> {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}});
               }
               if (i < n-1) {
                  sim.springs.push_back(Spring<T> {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}});
               }
               if (j < n-1) {
                  sim.springs.push_back(Spring<T> {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}});
               }
               if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}});
               }
            }
         }
      }
      if (mode == "4 edges") {
         T vel[3] = {0, 0, 0};
         for (int j = 0; j < n+1; j++) {
            for (int i = 0; i < n+1; i++) {
              T pos[3] = {dis * (i-n/2), 0, dis * (j-n/2)};
              if (((i==0) and (j==0)) or ((i==n) and (j==0)) or ((i==0) and (j==n)) or ((i==n) and (j==n))) {
                  sim.objects.push_back(Object<T> (m, pos, vel, false));
               }
               else {
                  sim.objects.push_back(Object<T> (m, pos, vel, true));
               }

               if (i < n) {
                  sim.springs.push_back(Spring<T> {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}});
               }
               if (j < n) {
                  sim.springs.push_back(Spring<T> {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}});
               }
               if (i < n-1) {
                  sim.springs.push_back(Spring<T> {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}});
               }
               if (j < n-1) {
                  sim.springs.push_back(Spring<T> {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}});
               }
               if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}});
                  sim.springs.push_back(Spring<T> {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}});
               }
            }
         }
      }
      sim.springs.push_back(Spring<T> {shear_k, c, {1, n+1}});
      sim.springs.push_back(Spring<T> {shear_k, c, {n-1, 2*n+1}});
      sim.springs.push_back(Spring<T> {shear_k, c, {(n-1)*(n+1), n*(n+1)+1}});
      sim.springs.push_back(Spring<T> {shear_k, c, {n*(n+1)-1, n*(n+1)+n-1}});
      sim.assign_springs();
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

   void initialize() {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(1, &vbo);
      GLuint shaderProgram = shader("shaders/vert.shader", "shaders/frag.shader");

      GLint position_attribute = glGetAttribLocation(shaderProgram, "position");
      GLint color_attribute = glGetAttribLocation(shaderProgram, "vert_color");
      PVM = glGetUniformLocation(shaderProgram, "PVM");

      glBindBuffer(GL_ARRAY_BUFFER, vbo);

      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (vertices.size() + colors.size()), NULL, GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices.size(), vertices.data());
      glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), sizeof(GLfloat) * colors.size(), colors.data());

      glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(GLfloat)*vertices.size()));

      glEnableVertexAttribArray(position_attribute);
      glEnableVertexAttribArray(color_attribute);

      sim.define_methods();
      int index = 0;
      GLuint indices[2*sim.springs.size()];
      for (int i = 0; i < sim.springs.size(); i++) {
         indices[index] = sim.springs[i].connections[0];
         index++;
         indices[index] = sim.springs[i].connections[1];
         index++;
      }

      GLuint eab;
      glGenBuffers(1, &eab);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

   }


   void display() {
      int running = GL_TRUE;
      double cur = glfwGetTime();
      int frame = 0;
      int index;

      while(running) {
         glUniformMatrix4fv(PVM, 1, GL_FALSE, glm::value_ptr(projection * view * model));
         index = 0;
         for (int i = 0; i < sim.objects.size(); i++) {
            if (sim.objects[i].dynamic) {
               sim.forces(i);
               sim.integrate(i);
            }
            vertices[index] = sim.objects[i].pos[0];
            colors[index] = pow(fmod(sim.objects[i].pos[1], 1)-1, 2);
            index++;
            vertices[index] = sim.objects[i].pos[1];
            colors[index] = pow(fmod(sim.objects[i].pos[1], 1)-1, 2);
            index++;
            vertices[index] = sim.objects[i].pos[2];
            colors[index] = pow(fmod(sim.objects[i].pos[1], 1)-1, 2);
            index++;
         }

         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         glBindVertexArray(vao);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);

         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices.size(), vertices.data());
         glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), sizeof(GLfloat) * colors.size(), colors.data());

         glDrawElements( GL_LINES, 2*sim.springs.size(), GL_UNSIGNED_INT, 0);

         glfwSwapBuffers();

         frame++;
         if (glfwGetTime()-cur >= 1) {
            std::cout<<frame<<std::endl;
            cur = glfwGetTime();
            frame = 0;

         glfwPollEvents();
         running = glfwGetWindowParam(GLFW_OPENED);
         }
      }
   }
};

//void keyboard(Graphics<float>& gui, int key, int action) {
//   if(key == 'S' && action == GLFW_PRESS) {
//      gui.camera_pos[2] -= 0.5;
//      gui.view = glm::lookAt(glm::vec3(gui.camera_pos[0], gui.camera_pos[1], gui.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//   }
//   if(key == 'W' && action == GLFW_PRESS) {
//      gui.camera_pos[2] += 0.5;
//      gui.view = glm::lookAt(glm::vec3(gui.camera_pos[0], gui.camera_pos[1], gui.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//   }
//   if(key == 'Q' && action == GLFW_PRESS) {
//      gui.camera_pos[1] -= 0.5;
//      gui.view = glm::lookAt(glm::vec3(gui.camera_pos[0], gui.camera_pos[1], gui.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//   }
//   if(key == 'E' && action == GLFW_PRESS) {
//      gui.camera_pos[1] += 0.5;
//      gui.view = glm::lookAt(glm::vec3(gui.camera_pos[0], gui.camera_pos[1], gui.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//   }
//   if(key == 'D' && action == GLFW_PRESS) {
//      gui.camera_pos[0] += 0.5;
//      gui.view = glm::lookAt(glm::vec3(gui.camera_pos[0], gui.camera_pos[1], gui.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//   }
//   if(key == 'A' && action == GLFW_PRESS) {
//      gui.camera_pos[0] -= 0.5;
//      gui.view = glm::lookAt(glm::vec3(gui.camera_pos[0], gui.camera_pos[1], gui.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//   }
//}

int main () {
   Graphics<float> gui(60.0f, 0, 36, 0.1);
   gui.window(700, 700);
   gui.universe(1.0/50, 0.05, 1.01);
   gui.cloth(25, 0.1, 0.1, 0.1, 0, 1, 1, "fixed");
   gui.initialize();
   gui.display();
}
