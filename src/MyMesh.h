#pragma once

#include <vector>
#include "Mesh.h"

namespace cs200 {
  class MyMesh final : public cs200::Mesh {
    std::vector<glm::vec4> vertices{};

    std::vector<Edge> edges{};
    std::vector<Face> faces{};

    struct {
      glm::vec4 center{}, size{};
    } bounding;

  public:
    MyMesh();

    int vertexCount() const override;

    const glm::vec4 *vertexArray() const override;

    glm::vec4 dimensions() const override;

    glm::vec4 center() const override;

    int faceCount() const override;

    const Face *faceArray() const override;

    int edgeCount() const override;

    const Edge *edgeArray() const override;

  private:
    void calculate_bounding_box();
    static float easing_function(float t);
    static glm::vec4 polar_to_cartesian(glm::vec2 polar, bool point = true);
  };
} // namespace cs200
