// MeshRenderDemo.cpp
// -- multiple meshes and multiple renderers
// cs200 9/20
//
// Visual Studio command prompt:
//   cl /EHsc MeshRenderDemo.cpp SolidRender.cpp MyMesh.cpp SquareMesh.cpp Affine.lib
//      opengl32.lib glew32.lib SDL2.lib SDL2main.lib /link /subsystem:console
// Linux command line:
//   g++ MeshRenderDemo.cpp SolidRender.cpp MyMesh.cpp SquareMesh.cpp Affine.cpp
//     -lGL -lGLEW -lSDL2

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "MyMesh.h"
#include "SquareMesh.h"
#include "SolidRender.h"
using namespace std;


const glm::vec4 O = cs200::point(0,0),
                EX = cs200::vector(1,0),
                EY = cs200::vector(0,1),
                BLACK(0,0,0,1),
                GRAY(0.95f,0.95f,0.95f,1);


class Client {
  public:
    Client(void);
    ~Client(void);
    void draw(double dt);
    void keypress(SDL_Keycode kc);
    void resize(int W, int H);
  private:
    double time;
    cs200::SolidRender *mm_render,
                       *sq_render;
    float rot_rate;
    glm::vec4 mm_center,
              sq_center;
    int count;
    vector<glm::mat4> mm_objs,
                      sq_objs;
    bool rotate_on;
};


Client::Client(void)
    : time(0),
      rotate_on(true) {
  mm_render = new cs200::SolidRender();
  cs200::MyMesh mm_mesh;
  mm_render->loadMesh(mm_mesh);

  sq_render = new cs200::SolidRender();
  cs200::SquareMesh sq_mesh;
  sq_render->loadMesh(sq_mesh);

  const int COUNT = 10;
  rot_rate = 360.0f/8.0f;
  mm_center = cs200::point(0.3f,0.3f);
  sq_center = cs200::point(-0.3f,-0.3f);

  for (int i=0; i < COUNT; ++i) {
    float theta_rad = glm::radians(360.0f/COUNT),
          theta_deg = 360.0f/COUNT,
          radius = 0.5f;
    glm::mat4 M = cs200::translate(mm_center-O)
                  * cs200::rotate(theta_deg*i)
                  * cs200::translate(radius*EY)
                  * cs200::scale(0.84f*radius*theta_rad/mm_mesh.dimensions().x,
                                 0.4f*radius/mm_mesh.dimensions().y)
                  * cs200::translate(O-mm_mesh.center());
    mm_objs.push_back(M);
    M = cs200::translate(sq_center-O)
        * cs200::rotate(theta_deg*i)
        * cs200::translate(radius*EY)
        * cs200::scale(0.5f*0.84f*radius*theta_rad,0.5f*0.4f*radius);
    sq_objs.push_back(M);
  }
}


Client::~Client(void) {
  sq_render->unloadMesh();
  delete sq_render;
  mm_render->unloadMesh();
  delete mm_render;
}


void Client::draw(double dt) {
  cs200::SolidRender::clearFrame(GRAY);

  glUseProgram(0);  // unnecessary, for testing purposes only
  glm::mat4 R = cs200::translate(mm_center-O)
                * cs200::rotate(rot_rate*time)
                * cs200::translate(O-mm_center);
  for (unsigned i=0; i < mm_objs.size(); ++i) {
    mm_render->setTransform(R*mm_objs[i]);
    mm_render->displayFaces(glm::vec4(0.75f,0,1,1));
    mm_render->displayEdges(BLACK);
  }

  glUseProgram(0);  // unnecessary, for testing purposes only
  R = cs200::translate(sq_center-O)
      * cs200::rotate(-rot_rate*time)
      * cs200::translate(O-sq_center);
  for (unsigned i=0; i < sq_objs.size(); ++i) {
    sq_render->setTransform(R*sq_objs[i]);
    sq_render->displayFaces(glm::vec4(1,0,0.75f,1));
    sq_render->displayEdges(BLACK);
  }

  if (rotate_on)
    time += dt;
}


void Client::keypress(SDL_Keycode kc) {
  if (kc == SDLK_SPACE)
    rotate_on = !rotate_on;
}


void Client::resize(int W, int H) {
  int D = min(W,H);
  glViewport(0,0,D,D);
}


/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

  // SDL: initialize and create a window
  SDL_Init(SDL_INIT_VIDEO);
  const char *title = "Multiple Mesh/Renderer Demo";
  int width = 600,
      height = 600;
  SDL_Window *window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,width,height,
                                        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // GLEW: get function bindings (if possible)
  glewInit();
  if (!GLEW_VERSION_2_0) {
    cout << "needs OpenGL version 2.0 or better" << endl;
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

