#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>


static struct {
   GLuint vbo, vao;
   GLint n = 20;
   std::ofstream fl;
   std::vector<GLfloat> vertices;
   float colors[(20+1)*(20+1)*3];
   glm::mat4 model, projection, view;
   float camera_pos[3], head[3], mouse_pos[2];
   float ke[3];
//   std::vector<float> colors((50+1)*(50+1)*3);
   GLint PVM;
} info;
struct Object {
   float m;
   std::vector<double> pos;
   std::vector<double> vel;
   bool dynamic;
   std::vector<double> forces;
   std::vector<double> a;
};

struct Spring {
   double k;
   double c;
   int connections[2];
   double rest_length;
};

class Simulate {
   public:
      double g;
      float dt;
      std::vector<Object> objects;
      std::vector<Spring> springs;
         
      void set_rest_length() {
         for (int i = 0; i < springs.size(); i++) {
            springs[i].rest_length = pow(pow(objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0], 2)+pow(objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1], 2)+pow(objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2], 2), 0.5);
         }
      }

      void integrate() {
         info.ke[0] = 0; info.ke[1] = 0; info.ke[2] = 0;
         for (int i=0; i < objects.size(); i++) {
            if (objects[i].dynamic) {
               info.ke[0] += 0.5 * objects[i].m * (0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]) * 0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]) - objects[i].vel[0] * objects[i].vel[0]);
               info.ke[1] += 0.5 * objects[i].m * (0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]) * 0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]) - objects[i].vel[1] * objects[i].vel[1]);
               info.ke[2] += 0.5 * objects[i].m * (0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]) * 0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]) - objects[i].vel[2] * objects[i].vel[2]);
               objects[i].vel[0] += 0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]);
               objects[i].vel[1] += 0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]);
               objects[i].vel[2] += 0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]);
               objects[i].pos[0] += 0.5 * objects[i].a[0]*dt*dt + objects[i].vel[0]*dt;
               objects[i].pos[1] += 0.5 * objects[i].a[1]*dt*dt + objects[i].vel[1]*dt;
               objects[i].pos[2] += 0.5 * objects[i].a[2]*dt*dt + objects[i].vel[2]*dt;
               objects[i].forces[0] = 0; objects[i].forces[1] = 0; objects[i].forces[2] = 0;
               objects[i].a[0] = objects[i].forces[0]/objects[i].m; objects[i].a[1] = objects[i].forces[1]/objects[i].m; objects[i].a[2] = objects[i].forces[2]/objects[i].m;
            }       
         }
         info.fl<<info.ke[0]<<","<<info.ke[1]<<","<<info.ke[2]<<std::endl;
      }

      void forces() {
         for (int i=0; i < objects.size(); i++) {
            if (objects[i].dynamic) {
               objects[i].forces[1] -= objects[i].m * g;
            }
         }

         for (int i = 0; i < springs.size(); i++) {
            if ((objects[springs[i].connections[0]].dynamic) or (objects[springs[i].connections[1]].dynamic)) {
               double cur_length = pow(pow(objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0], 2)+pow(objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1], 2)+pow(objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2], 2), 0.5);

               double norm[3];
               norm[0] = (objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0])/cur_length;
               norm[1] = (objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1])/cur_length;
               norm[2] = (objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2])/cur_length;

//               if (cur_length > 1.1*springs[i].rest_length) {
//                  objects.push_back(objects[springs[i].connections[1]]);
//                  springs[i].connections[1] = objects.size()-1;
//                  cur_length = pow(pow(objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0], 2)+pow(objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1], 2)+pow(objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2], 2), 0.5);
//
//                  norm[0] = (objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0])/cur_length;
//                  norm[1] = (objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1])/cur_length;
//                  norm[2] = (objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2])/cur_length;
//               }

               double dot = (objects[springs[i].connections[0]].vel[0]-objects[springs[i].connections[1]].vel[0]) * norm[0] + (objects[springs[i].connections[0]].vel[1]-objects[springs[i].connections[1]].vel[1]) * norm[1] + (objects[springs[i].connections[0]].vel[2]-objects[springs[i].connections[1]].vel[2]) * norm[2];

               if (objects[springs[i].connections[0]].dynamic) {
                  objects[springs[i].connections[0]].forces[0] -= (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[0];
                  objects[springs[i].connections[0]].forces[1] -= (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[1];
                  objects[springs[i].connections[0]].forces[2] -= (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[2];
               }

               if (objects[springs[i].connections[1]].dynamic) {
                  objects[springs[i].connections[1]].forces[0] += (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[0];
                  objects[springs[i].connections[1]].forces[1] += (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[1];
                  objects[springs[i].connections[1]].forces[2] += (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[2];
               }
            }
         }
      }
};


void cloth (int n, float struc_k, float shear_k, float bend_k, float c, float m, float dis, std::string mode, std::vector<Object>& objects, std::vector<Spring>& springs) {
   if (mode == "square") {
      for (int j = 0; j < n+1; j++) {
         for (int i = 0; i < n+1; i++) {
            if ((i == n) or (j == n) or (i == 0) or (j == 0)) {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, false, {0, 0, 0}, {0, 0, 0}});
            }
            else {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, true, {0, 0, 0}, {0, 0, 0}});
            }

            if (i < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}, 0});
            }
            if (j < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}, 0});
            }
            if (i < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}, 0});
            }
            if (j < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}, 0});
            }
            if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}, 0});
            }
         }
      }
   }
   if (mode == "4 edges") {
      for (int j = 0; j < n+1; j++) {
         for (int i = 0; i < n+1; i++) {
            if (((i == n) and (j == n)) or ((i == n) and (j == 0)) or ((i == 0) and (j == n)) or ((i == 0) and (j == 0))) {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, false, {0, 0, 0}, {0, 0, 0}});
            }
            else {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, true, {0, 0, 0}, {0, 0, 0}});
            }
            if (i < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}, 0});
            }
            if (j < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}, 0});
            }
            if (i < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}, 0});
            }
            if (j < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}, 0});
            }
            if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}, 0});
            }
         }
      }
   }
   if (mode == "1 edge") {
      for (int j = 0; j < n+1; j++) {
         for (int i = 0; i < n+1; i++) {
            if ((i == n) and (j == n)) {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, false, {0, 0, 0}, {0, 0, 0}});
            }
            else {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, true, {0, 0, 0}, {0, 0, 0}});
            }

            if (i < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}, 0});
            }
            if (j < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}, 0});
            }
            if (i < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}, 0});
            }
            if (j < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}, 0});
            }
            if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}, 0});
            }
         }
      }
   }
}

void read_shader_src(const char *fname, std::vector<char> &buffer);

GLuint load_and_compile_shader(const char *fname, GLenum shaderType);

GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);

void GLFWCALL window_resized(int width, int height);

void keyboard(int key, int action);

void display(Simulate &sim);

void initialize(Simulate &sim);

int main () {
   if ( !glfwInit()) {
      std::cerr << "Failed to initialize GLFW! I'm out!" << std::endl;
      exit(-1);
   }

   glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);

   if( !glfwOpenWindow(700, 700, 8, 8, 8, 0, 8, 0, GLFW_WINDOW)) {
      std::cerr << "Failed to open a window! I'm out!" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   glfwSetWindowSizeCallback( window_resized );
   glfwSetKeyCallback(keyboard); 

   int major, minor, rev;
   glfwGetGLVersion(&major, &minor, &rev);
   std::cout << "OpenGL - " << major << "." << minor << "." << rev << std::endl;

   glewExperimental = GL_TRUE;
   if(glewInit() != GLEW_OK) {
      std::cerr << "Failed to initialize GLEW! I'm out!" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   std::vector<Object> objects;
   std::vector<Spring> springs;
   Simulate sim {0.00001, 0.7, objects, springs};
   initialize(sim);

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   int running = GL_TRUE;
   double cur = glfwGetTime();
   int frame = 0;
   int index;

   while(running) {
      glUniformMatrix4fv(info.PVM, 1, GL_FALSE, glm::value_ptr(info.projection * info.view * info.model));
      display(sim);
      sim.forces();
      sim.integrate();
      index = 0;
      for (int i = 0; i < (info.n+1)*(info.n+1); i++) {
         info.vertices[index] = sim.objects[i].pos[0];
         index++;
         info.vertices[index] = sim.objects[i].pos[1];
         index++;
         info.vertices[index] = sim.objects[i].pos[2];
         index++;
      }

      frame++;
      if (glfwGetTime()-cur >= 1) {
         std::cout<<frame<<std::endl;
         cur = glfwGetTime();
         frame = 0;

      glfwPollEvents();
      running = glfwGetWindowParam(GLFW_OPENED);
      }
   }

   glfwTerminate();

   return 0;
}

void display(Simulate &sim) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glBindVertexArray(info.vao);
   glBindBuffer(GL_ARRAY_BUFFER, info.vbo);

   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*info.vertices.size(), info.vertices.data());

   glDrawElements( GL_LINES, 2*sim.springs.size(), GL_UNSIGNED_INT, 0);

   glfwSwapBuffers();
}

void initialize(Simulate &sim) {
   glGenVertexArrays(1, &info.vao);
   glBindVertexArray(info.vao);

   info.fl.open("fl", std::ios::out);
   info.fl.clear();
   cloth (info.n, 0.01, 0.01, 0.01, 0.3, 1, 0.04, "4 edges", sim.objects, sim.springs);
   sim.set_rest_length();

   for (int i = 0; i < (info.n+1)*(info.n+1); i++) {
      info.vertices.push_back(sim.objects[i].pos[0]);
      info.vertices.push_back(sim.objects[i].pos[1]);
      info.vertices.push_back(sim.objects[i].pos[2]);
   }

   GLuint indices[2*sim.springs.size()];
   int index = 0;
   for (int i = 0; i < sim.springs.size(); i++) {
      indices[index] = sim.springs[i].connections[0];
      index++;
      indices[index] = sim.springs[i].connections[1];
      index++;
   }

   glGenBuffers(1, &info.vbo);

   glBindBuffer(GL_ARRAY_BUFFER, info.vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(info.colors) + sizeof(GLfloat) * info.vertices.size(), NULL, GL_DYNAMIC_DRAW);

   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*info.vertices.size(), info.vertices.data());

   glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*info.vertices.size(), sizeof(info.colors), info.colors);

   GLuint eab;
   glGenBuffers(1, &eab);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

   GLuint shaderProgram = create_program("shaders/vert.shader", "shaders/frag.shader");

   GLint position_attribute = glGetAttribLocation(shaderProgram, "position");
   info.PVM = glGetUniformLocation(shaderProgram, "PVM");
   
   info.camera_pos[2] = -3;
   info.projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
   info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(info.head[0], info.head[1], info.head[2]), glm::vec3(0, 1, 0));

   glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glEnableVertexAttribArray(position_attribute);

   GLint color_attribute = glGetAttribLocation(shaderProgram, "color");
   glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(color_attribute);

}

void GLFWCALL window_resized(int width, int height) {
   glClearColor(0.4, 0.4, 0.4, 1);

   glViewport(0, 0, width, height);

   glClear(GL_COLOR_BUFFER_BIT);
   glfwSwapBuffers();
}

void keyboard(int key, int action) {
   if(key == 'Z' && action == GLFW_PRESS) {
      glfwTerminate();
      exit(0);
   }
   if(key == 'S' && action == GLFW_PRESS) {
      info.camera_pos[2] -= 0.5;
      info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(info.head[0], info.head[1], info.head[2]), glm::vec3(0, 1, 0));
   }
   if(key == 'W' && action == GLFW_PRESS) {
      info.camera_pos[2] += 0.5;
      info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   }
   if(key == 'Q' && action == GLFW_PRESS) {
      info.camera_pos[1] -= 0.5;
      info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   }
   if(key == 'E' && action == GLFW_PRESS) {
      info.camera_pos[1] += 0.5;
      info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   }
   if(key == 'D' && action == GLFW_PRESS) {
      info.camera_pos[0] += 0.5;
      info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   }
   if(key == 'A' && action == GLFW_PRESS) {
      info.camera_pos[0] -= 0.5;
      info.view = glm::lookAt(glm::vec3(info.camera_pos[0], info.camera_pos[1], info.camera_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
   }
}

void read_shader_src(const char *fname, std::vector<char> &buffer) {
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
}

GLuint load_and_compile_shader(const char *fname, GLenum shaderType) {
   std::vector<char> buffer;
   read_shader_src(fname, buffer);
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

GLuint create_program(const char *path_vert_shader, const char *path_frag_shader) {
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

