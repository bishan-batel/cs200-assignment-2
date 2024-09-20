#pragma once
#include "Mesh.h"

namespace cs200 {
  class MyMesh final : public cs200::Mesh {
    std::size_t vertices_length{0}, faces_length{0}, edges_length{0};
    glm::vec4 *vertices{nullptr};

    struct {
      glm::vec4 center{}, size{};
    } bounding;

    Face *faces{nullptr};
    Edge *edges{nullptr};

  public:
    MyMesh();

    ~MyMesh() override;

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
  };
} // namespace cs200
