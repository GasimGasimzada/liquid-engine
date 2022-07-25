#include "liquid/core/Base.h"
#include "AssetData.h"
#include "liquid/scene/MeshAsset.h"
#include "liquid/renderer/MaterialAsset.h"
#include "DefaultObjects.h"

namespace liquid::default_objects {

AssetData<MeshAsset> createCube() {
  BaseGeometryAsset<Vertex> geometry;

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices{
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      0, 1, 2, 3, 2, 1, 4, 5, 6, 7, 6, 5,
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      8, 9, 10, 11, 10, 9, 12, 13, 14, 15, 14, 13, 16, 17, 18, 19, 18, 17, 20,
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
      21, 22, 23, 22, 21};

  vertices.push_back({-1, -1, -1, 0, 0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
                      0.0, 0.0}); // 0
  vertices.push_back({1, -1, -1, 0, 0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
                      1.0, 0.0}); // 1
  vertices.push_back({-1, 1, -1, 0, 0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
                      0.0, 1.0}); // 2
  vertices.push_back(
      {1, 1, -1, 0, 0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0}); // 3

  // back face (debug = red)
  vertices.push_back(
      {-1, 1, 1, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0}); // 4
  vertices.push_back(
      {1, 1, 1, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0}); // 5
  vertices.push_back(
      {-1, -1, 1, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}); // 6
  vertices.push_back(
      {1, -1, 1, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0}); // 7

  // left face (debug = green)
  vertices.push_back({-1, -1, 1, -1.0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0}); // 8
  vertices.push_back({-1, -1, -1, -1.0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                      1.0, 0.0}); // 9
  vertices.push_back({-1, 1, 1, 0, -1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 1.0}); // 10
  vertices.push_back({-1, 1, -1, 0, -1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                      1.0, 1.0}); // 11

  // right face (debug = yellow)
  vertices.push_back({1, -1, -1, 1.0, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0}); // 12
  vertices.push_back(
      {1, -1, 1, 1.0, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0}); // 13
  vertices.push_back(
      {1, 1, -1, 1.0, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0}); // 14
  vertices.push_back(
      {1, 1, 1, 1.0, 0, 0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0}); // 15

  // bottom face = cyan
  vertices.push_back({-1, 1, -1, 0, 1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0,
                      0.0, 0.0}); // 16
  vertices.push_back(
      {1, 1, -1, 0, 1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0}); // 17
  vertices.push_back(
      {-1, 1, 1, 0, 1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0}); // 18
  vertices.push_back(
      {1, 1, 1, 0, 1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0}); // 19

  // top face = blue
  vertices.push_back({-1, -1, 1, 0, -1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                      0.0, 0.0}); // 20
  vertices.push_back({1, -1, 1, 0, -1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                      1.0, 0.0}); // 21
  vertices.push_back({-1, -1, -1, 0, -1.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 1.0,
                      0.0, 1.0}); // 22
  vertices.push_back({1, -1, -1, 0, -1.0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                      1.0, 1.0}); // 23

  AssetData<MeshAsset> mesh;
  geometry.vertices = vertices;
  geometry.indices = indices;
  mesh.name = "__liquid.engine.defaultCube";
  mesh.data.geometries.push_back(geometry);

  return mesh;
}

AssetData<MaterialAsset> createDefaultMaterial() {
  AssetData<MaterialAsset> material;
  material.name = "__liquid.engine.defaultMaterial";

  return material;
}

} // namespace liquid::default_objects
