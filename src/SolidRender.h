// SolidRender.h
// -- OpenGl 2D rendering interface
// cs200 9/20

#ifndef CS200_SOLIDRENDER_H
#define CS200_SOLIDRENDER_H

#include <GL/glew.h>

#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Mesh.h"

namespace cs200 {

  class SolidRender {
  public:
    SolidRender();

    ~SolidRender();

    static void clearFrame(const glm::vec4 &c);

    void setTransform(const glm::mat4 &M);

    void loadMesh(const cs200::Mesh &m);

    void unloadMesh();

    void displayEdges(const glm::vec4 &c);

    void displayFaces(const glm::vec4 &c);

  private:
    GLint ucolor, utransform;
    GLuint program, vao_edges, vao_faces, vertex_buffer, edge_buffer, face_buffer;
    size_t mesh_edge_count, mesh_face_count;
  };

} // namespace cs200

#endif
