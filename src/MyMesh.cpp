/**
 * Name: Kishan S Patel
 * Email: kishan.patel@digipen.edu
 * Assignment Number: 2
 * Course: CS200
 * Term: Fall 2024
 *
 * File: MyMesh.cpp
 *
 *  CPP file for custom student mesh, generates a spriral mesh
 *
 *
 *  Uncomment out "#define EPIC_MESH_MODE" to have a cooler one :)
 */

/* #define EPIC_MESH_MODE 1 */

#include "MyMesh.h"
#include "Affine.h"

namespace cs200 {

#if EPIC_MESH_MODE
  constexpr size_t REVOLUTIONS = 20;
  constexpr size_t DETAIL = 10000;
  constexpr float MIN_WIDTH = 0.1f;
  constexpr float MAX_WIDTH = -0.1f;
#else
  constexpr size_t REVOLUTIONS = 20;
  constexpr size_t DETAIL = 10000;
  constexpr float MIN_WIDTH = 0.01f;
  constexpr float MAX_WIDTH = 0.01f;
#endif

  const float PI = 4.f * glm::atan(1.f);

  MyMesh::MyMesh() {
    using cs200::point;
    using glm::vec4;

    vertices.push_back(point(0.f, 0.f));
    vertices.push_back(point(0.f, 0.f));

    for (size_t i = 0; i < DETAIL; i++) {
      const float t = (float) (i) / (float) (DETAIL);

      const vec4 ref_point_polar = point(t, PI * 2.f * REVOLUTIONS * t);
      // reference point
      const vec4 ref_point = polar_to_cartesian(ref_point_polar);

      constexpr float dt = 1E-5f;
      const vec4 derivative =
          glm::normalize((polar_to_cartesian(point((t + dt), PI * 2.f * REVOLUTIONS * (t + dt))) - ref_point) / dt);

      const vec4 normal = vector(-derivative.y, derivative.x) * (MIN_WIDTH + easing_function(t) * MAX_WIDTH);

      const vec4 point0 = ref_point + normal;
      const vec4 point1 = ref_point - normal;

      const int p0 = (int) vertices.size();
      vertices.push_back(point0);

      const int p1 = (int) vertices.size();
      vertices.push_back(point1);

      /* edges.emplace_back(p0, p1); */
      edges.emplace_back(p0, p0 - 2);
      edges.emplace_back(p1, p1 - 2);

      faces.emplace_back(p0, p1, p0 - 1);
      faces.emplace_back(p0, p0 - 2, p0 - 1);

      /* faces.emplace_back(p1, p0, p0 - 2); */
    }

    calculate_bounding_box();
  };

  int MyMesh::vertexCount() const { return (int) vertices.size(); }

  const glm::vec4 *MyMesh::vertexArray() const { return vertices.data(); }

  glm::vec4 MyMesh::dimensions() const { return bounding.size; }

  glm::vec4 MyMesh::center() const { return bounding.center; }

  int MyMesh::faceCount() const { return (int) faces.size(); }

  const cs200::Mesh::Face *MyMesh::faceArray() const { return faces.data(); }

  int MyMesh::edgeCount() const { return (int) edges.size(); }

  const cs200::Mesh::Edge *MyMesh::edgeArray() const { return edges.data(); }

  void MyMesh::calculate_bounding_box() {
    /*
    glm::vec4 min{0}, max{0};

    for (const auto &vertex: vertices) {
      min = glm::min(min, vertex);
      max = glm::max(max, vertex);
    }

    bounding.size = glm::abs(max - min);
    bounding.center = (max + min) / 2.f;
    */

    bounding.center = point(0.f, 0.f);
    bounding.size = point(2.f, 2.f);
  }

  glm::vec4 MyMesh::polar_to_cartesian(glm::vec2 polar, bool point) {
    return {polar.x * glm::cos(polar.y), polar.x * glm::sin(polar.y), 0.f, point ? 1.0f : 0.0f};
  }
  float MyMesh::easing_function(float t) { return t * t; }
} // namespace cs200
