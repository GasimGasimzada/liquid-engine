#pragma once

#include <math.h>
#include "scene/Mesh.h"

static constexpr float PI = 3.14159265;

enum SphereColor { RED = 0x1, GREEN = 0x2, BLUE = 0x4 };

liquid::Mesh createSphere(float radius, uint32_t stacks, uint32_t slices,
                          unsigned char color) {
  liquid::Mesh mesh;
  liquid::Geometry geom;

  for (uint32_t i = 0; i < stacks; ++i) {
    float theta = ((float)(i + 1) / stacks) * PI;

    for (uint32_t j = 0; j < slices; ++j) {
      float phi = 2.0f * PI * (float)j / slices;

      liquid::Vertex vertex{};
      vertex.x = radius * sin(theta) * cos(phi);
      vertex.y = radius * sin(theta) * sin(phi);
      vertex.z = radius * cos(theta);

      float colorValue = (float)(j + i) / (stacks + slices);
      vertex.r = (color & RED) == RED ? colorValue : 0;
      vertex.g = (color & BLUE) == BLUE ? colorValue : 0;
      vertex.b = (color & GREEN) == GREEN ? colorValue : 0;

      vertex.u0 = phi / (2.0 * PI);
      vertex.v0 = 0.5 + vertex.z / 2;

      geom.addVertex(vertex);
    }
  }

  for (uint32_t i = 0; i < stacks; ++i) {
    for (uint32_t j = 0; j < slices; ++j) {
      auto i0 = i * stacks + j;
      auto i1 = i * stacks + (j + 1) % slices;
      auto i2 = i * stacks + j + slices;
      auto i3 = i * stacks + slices + (j + 1) % slices;

      geom.addTriangle(i0, i1, i2);
      geom.addTriangle(i1, i3, i2);
    }
  }

  uint32_t bottomIndex = static_cast<uint32_t>(geom.getVertices().size());
  uint32_t topIndex = static_cast<uint32_t>(geom.getVertices().size());

  geom.addVertex({0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0});
  geom.addVertex({0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0});

  uint32_t indexStart = (slices - 1) * slices;
  for (uint32_t i = 0; i < stacks; ++i) {
    geom.addTriangle(bottomIndex, (i + 1) % slices, i);
    geom.addTriangle(topIndex, indexStart + 1, indexStart + (i + 1) % slices);
  }

  mesh.addGeometry(geom);

  return mesh;
}
