#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

using AssetManagerDeathTest = AssetManagerTest;

TEST_F(AssetManagerTest, CreatesSkeletonFileFromSkeletonAsset) {
  liquid::AssetData<liquid::SkeletonAsset> asset;
  asset.name = "test-skel0";

  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);

    size_t countJoints = 20;
    for (size_t i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = manager.createSkeletonFromAsset(asset);

  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Skeleton);

  uint32_t numJoints = 0;
  file.read(numJoints);
  EXPECT_EQ(numJoints, 20);

  std::vector<glm::vec3> actualPositions(numJoints);
  std::vector<glm::quat> actualRotations(numJoints);
  std::vector<glm::vec3> actualScales(numJoints);
  std::vector<glm::mat4> actualInverseBindMatrices(numJoints);
  std::vector<liquid::JointId> actualParents(numJoints);
  std::vector<liquid::String> actualNames(numJoints);
  file.read(actualPositions);
  file.read(actualRotations);
  file.read(actualScales);
  file.read(actualParents);
  file.read(actualInverseBindMatrices);
  file.read(actualNames);

  for (uint32_t i = 0; i < numJoints; ++i) {
    EXPECT_EQ(actualPositions.at(i), asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(actualRotations.at(i), asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(actualScales.at(i), asset.data.jointLocalScales.at(i));
    EXPECT_EQ(actualParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(actualInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(actualNames.at(i), asset.data.jointNames.at(i));
  }
}

TEST_F(AssetManagerTest, LoadsSkeletonAssetFromFile) {
  liquid::AssetData<liquid::SkeletonAsset> asset;
  asset.name = "test-skel0";
  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);

    size_t countJoints = 20;
    for (size_t i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = manager.createSkeletonFromAsset(asset);
  auto handle = manager.loadSkeletonFromFile(filePath.getData());

  EXPECT_NE(handle.getData(), liquid::SkeletonAssetHandle::Invalid);

  auto &actual =
      manager.getRegistry().getSkeletons().getAsset(handle.getData());

  for (size_t i = 0; i < actual.data.jointLocalPositions.size(); ++i) {
    EXPECT_EQ(actual.data.jointLocalPositions.at(i),
              asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(actual.data.jointLocalRotations.at(i),
              asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(actual.data.jointLocalScales.at(i),
              asset.data.jointLocalScales.at(i));
    EXPECT_EQ(actual.data.jointParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(actual.data.jointInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(actual.data.jointNames.at(i), asset.data.jointNames.at(i));
  }
}
