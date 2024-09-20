#include "MyMesh.h"

namespace cs200 {
  MyMesh::MyMesh() {
    vertices_length = 12;
    vertices = new glm::vec4[vertices_length];
    for (size_t i = 0; i < vertices_length; i++) {
      const float theta = 6.2f * (float) i / (float) vertices_length;
      const float mag = theta + 0.1f;
      vertices[i] = {mag * glm::cos(theta), mag * glm::sin(theta), 0.f, 1.f};
    }

    faces_length = 6;
    faces = new Face[faces_length];

    for (size_t i = 0; i < faces_length; i++) {
      faces[i] = {
          (int) ((i) % vertices_length),
          (int) ((i + vertices_length / 3) % vertices_length),
          (int) ((i + 2 * vertices_length / 3) % vertices_length),
      };
    }

    // generate edges automatically
    edges_length = vertices_length * 2;
    edges = new Edge[edges_length];

    for (size_t i = 0; i < edges_length; i++) {
      edges[i] = {(int) (i / 2 % vertices_length), (int) (i / 2 % vertices_length)};
    }

    /* for (size_t i = 0; i < faces_length; i++) { */
    /*   const Face &face = faces[i]; */
    /**/
    /*   edges[3 * i] = {(int) face.index1, (int) face.index2}; */
    /*   edges[3 * i + 1] = {(int) face.index2, (int) face.index3}; */
    /*   edges[3 * i + 2] = {(int) face.index3, (int) face.index1}; */
    /* } */

    calculate_bounding_box();
  };

  MyMesh::~MyMesh() {
    delete[] faces;
    delete[] edges;
    delete[] vertices;
  }

  int MyMesh::vertexCount() const { return (int) vertices_length; }

  const glm::vec4 *MyMesh::vertexArray() const { return vertices; }

  glm::vec4 MyMesh::dimensions() const { return bounding.size; }

  glm::vec4 MyMesh::center() const { return bounding.center; }

  int MyMesh::faceCount() const { return (int) faces_length; }

  const cs200::Mesh::Face *MyMesh::faceArray() const { return faces; }

  int MyMesh::edgeCount() const { return (int) edges_length; }

  const cs200::Mesh::Edge *MyMesh::edgeArray() const { return edges; }

  void MyMesh::calculate_bounding_box() {
    glm::vec4 min{0}, max{0};

    for (size_t i = 0; i < vertices_length; i++) {
      min = glm::min(min, vertices[i]);
      max = glm::max(max, vertices[i]);
    }

    bounding.size = glm::abs(max - min);
    bounding.center = (max + min) / 2.f;
  }
} // namespace cs200
