// cs200opengl_demo.cpp
// -- Draw a rotating square using OpenGL with GLM, GLEW, and SDL
// cs200 9/20
//
// To compile using Visual Studio command prompt:
//   cl /EHsc cs200opengl_demo.cpp opengl32.lib glew32.lib\
//      SDL2.lib SDL2main.lib /link /subsystem:console
// To compile/link on Linux:
//   g++ cs200opengl_demo.cpp -lSDL2 -lGL -lGLEW

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
using namespace std;


class Client {
  public:
    Client(void);
    ~Client(void);
    void draw(double dt);
    void keypress(SDL_Keycode kc);
    void resize(int W, int H);
    void mouseclick(int x, int y);
  private:
    double time;
    GLuint program,
           vao_edges,
           vao_faces,
           vertex_buffer,
           edge_buffer,
           face_buffer;
    GLint utransform,
          ucolor;
    bool rotate;
};


Client::Client(void)
    : time(0),
      rotate(true) {
  GLint value;

  // compile fragment shader
  const char *fragment_shader_text =
    "#version 130\n\
     uniform vec4 color;\
     out vec4 frag_color;\
     void main(void) {\
       frag_color = color;\
     }";
  GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshader,1,&fragment_shader_text,0);
  glCompileShader(fshader);
  glGetShaderiv(fshader,GL_COMPILE_STATUS,&value);
  if (!value) {
    char buffer[1024];
    glGetShaderInfoLog(fshader,1024,0,buffer);
    cerr << buffer << endl;
  }

  // compile vertex shader
  const char *vertex_shader_text =
    "#version 130\n\
     in vec4 position;\
     uniform mat4 transform;\
     void main() {\
       gl_Position = transform * position;\
     }";
  GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vshader,1,&vertex_shader_text,0);
  glCompileShader(vshader);
  glGetShaderiv(vshader,GL_COMPILE_STATUS,&value);
  if (!value) {
    char buffer[1024];
    glGetShaderInfoLog(vshader,1024,0,buffer);
    cerr << buffer << endl;
  }

  // link shader program
  program = glCreateProgram();
  glAttachShader(program,fshader);
  glAttachShader(program,vshader);
  glLinkProgram(program);
  glGetProgramiv(program,GL_LINK_STATUS,&value);
  if (!value) {
    cerr << "shader program failed to link" << endl;
    char buffer[1024];
    glGetProgramInfoLog(program,1024,0,buffer);
    cerr << buffer << endl;
  }
  glDeleteShader(vshader);
  glDeleteShader(fshader);

  // vertex buffer for standard square
  glGenBuffers(1,&vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
  float vertices[16] = { 1,1,0,1, -1,1,0,1, -1,-1,0,1, 1,-1,0,1 };
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

  // edge buffer
  glGenBuffers(1,&edge_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,edge_buffer);
  unsigned edges[8] = { 0,1, 1,2, 2,3, 3,0 };
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(edges),edges,GL_STATIC_DRAW);

  // face buffer
  glGenBuffers(1,&face_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,face_buffer);
  unsigned faces[6] = { 0,1,2, 0,2,3 };
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(faces),faces,GL_STATIC_DRAW);

  // VAO for rendering edges
  GLint aposition = glGetAttribLocation(program,"position");
  glGenVertexArrays(1,&vao_edges);
  // start recording:
  glBindVertexArray(vao_edges);
  // associate position attribute with vertex buffer:
  glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
  glVertexAttribPointer(aposition,4,GL_FLOAT,false,sizeof(glm::vec4),0);
  glEnableVertexAttribArray(aposition);
  // select edge buffer:
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,edge_buffer);
  // stop recording:
  glBindVertexArray(0);

  // VAO for rendering faces
  glGenVertexArrays(1,&vao_faces);
  glBindVertexArray(vao_faces);
  glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
  glVertexAttribPointer(aposition,4,GL_FLOAT,false,sizeof(glm::vec4),0);
  glEnableVertexAttribArray(aposition);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,face_buffer);
  glBindVertexArray(0);

  // shader parameter locations
  utransform = glGetUniformLocation(program,"transform");
  ucolor = glGetUniformLocation(program,"color");
}


Client::~Client(void) {
  // delete vertex array objects used
  glDeleteVertexArrays(1,&vao_edges);
  glDeleteVertexArrays(1,&vao_faces);
  
  // delete GPU buffers used
  glDeleteBuffers(1,&face_buffer);
  glDeleteBuffers(1,&edge_buffer);
  glDeleteBuffers(1,&vertex_buffer);

  // delete shader program
  glUseProgram(0);
  glDeleteProgram(program);
}


void Client::draw(double dt) {
  const float PI = 4.0f * atan(1.0f),
              ROTATION_RATE = 2*PI/5.0f;

  // clear background
  glClearColor(0.95f,0.95f,0.95f,1);
  glClear(GL_COLOR_BUFFER_BIT);

  // select shader program
  glUseProgram(program);

  // set vertex transformation
  float c = cos(ROTATION_RATE*time),
        s = sin(ROTATION_RATE*time);
  glm::mat4 R(0.5f*c,  0.5f*s, 0, 0,
              -0.5f*s, 0.5f*c, 0, 0,
              0,       0,      0, 0,
              0,       0,      0, 1);
  glUniformMatrix4fv(utransform,1,false,&R[0][0]);

  // draw faces
  glUniform4f(ucolor,1,0.5f,1,1);
  glBindVertexArray(vao_faces);
  glDrawElements(GL_TRIANGLES,2*3,GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

  // draw edges
  glUniform4f(ucolor,0,0,0,1);
  glBindVertexArray(vao_edges);
  glDrawElements(GL_LINES,4*2,GL_UNSIGNED_INT,0);
  glBindVertexArray(0); // DO NOT FORGET TO DO THIS!

  if (rotate)
    time += dt;
}


void Client::keypress(SDL_Keycode kc) {
  if (kc == SDLK_SPACE)
    rotate = !rotate;
}


void Client::resize(int W, int H) {
  int D = min(W,H);
  glViewport(0,0,D,D);
}


void Client::mouseclick(int x, int y) {
  rotate = !rotate;
}


/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

  // SDL: initialize and create a window
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("OpenGL Demo",SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,500,500,
                                        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // GLEW: get function bindings (if possible)
  GLenum value = glewInit();
  if (value != GLEW_OK) {
    cout << glewGetErrorString(value) << endl;
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return -1;
  }

  // animation loop
  bool done = false;
  Client *client = new Client();
  Uint32 ticks_last = SDL_GetTicks();
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          done = true;
          break;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE)
            done = true;
          else
            client->keypress(event.key.keysym.sym);
          break;
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            client->resize(event.window.data1,event.window.data2);
          break;
        case SDL_MOUSEBUTTONDOWN:
          client->mouseclick(event.button.x,event.button.y);
          break;
      }
    }
    Uint32 ticks = SDL_GetTicks();
    double dt = 0.001*(ticks - ticks_last);
    ticks_last = ticks;
    client->draw(dt);
    SDL_GL_SwapWindow(window);
  }

  // clean up
  delete client;
  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}

