#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "MaterialAssetManagerGroup.h"
#include "TextureAssetManagerGroup.h"

#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"
#include "liquid/asset/OutputBinaryStream.h"

namespace liquid {

static Result<bool> checkAssetFile(InputBinaryStream &stream,
                                   const Path &filePath, AssetType assetType) {
  if (!stream.good()) {
    return Result<bool>::Error("File cannot be opened for reading: " +
                               filePath.string());
  }

  AssetFileHeader header;
  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  stream.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  stream.read(header.version);
  stream.read(header.type);

  if (magic != header.magic) {
    return Result<bool>::Error("Opened file is not a liquid asset: " +
                               filePath.string());
  }

  if (header.type != assetType) {
    return Result<bool>::Error("Opened file is not a liquid " +
                               getAssetTypeString(assetType) +
                               " asset: " + filePath.string());
  }

  return Result<bool>::Ok(true);
}

/**
 * @brief Get relative path of the asset
 *
 * @tparam TAssetMap Asset map type
 * @param map Asset map
 * @param handle Asset handle
 * @return Relative path of asset
 */
template <class TAssetMap>
static String getAssetRelativePath(const TAssetMap &map,
                                   typename TAssetMap::Handle handle) {
  if (handle != TAssetMap::Handle::Invalid) {
    auto &texture = map.getAsset(handle);
    return texture.relativePath.string();
  }

  return String("");
}

Result<MaterialAssetHandle> MaterialAssetLoader::loadFromStream(
    InputBinaryStream &stream, const Path &filePath, AssetRegistry &registry,
    const Path &assetsPath) {

  TextureAssetLoader textureLoader;

  AssetData<MaterialAsset> material{};
  material.path = filePath;
  material.relativePath = std::filesystem::relative(filePath, assetsPath);
  material.name = material.relativePath.string();
  material.type = AssetType::Material;
  std::vector<String> warnings{};

  // Base color
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res =
        textureLoader.getOrLoadFromPath(texturePathStr, registry, assetsPath);
    if (res.hasData()) {
      material.data.baseColorTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.baseColorTextureCoord);
    stream.read(material.data.baseColorFactor);
  }

  // Metallic roughness
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res =
        textureLoader.getOrLoadFromPath(texturePathStr, registry, assetsPath);
    if (res.hasData()) {
      material.data.metallicRoughnessTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }

    stream.read(material.data.metallicRoughnessTextureCoord);
    stream.read(material.data.metallicFactor);
    stream.read(material.data.roughnessFactor);
  }

  // Normal
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res =
        textureLoader.getOrLoadFromPath(texturePathStr, registry, assetsPath);
    if (res.hasData()) {
      material.data.normalTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.normalTextureCoord);
    stream.read(material.data.normalScale);
  }

  // Occlusion
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res =
        textureLoader.getOrLoadFromPath(texturePathStr, registry, assetsPath);
    if (res.hasData()) {
      material.data.occlusionTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.occlusionTextureCoord);
    stream.read(material.data.occlusionStrength);
  }

  // Emissive
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res =
        textureLoader.getOrLoadFromPath(texturePathStr, registry, assetsPath);
    if (res.hasData()) {
      material.data.emissiveTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.emissiveTextureCoord);
    stream.read(material.data.emissiveFactor);
  }

  return Result<MaterialAssetHandle>::Ok(
      registry.getMaterials().addAsset(material), warnings);
}

Result<MaterialAssetHandle>
MaterialAssetLoader::loadFromPath(const Path &filePath, AssetRegistry &registry,
                                  const Path &assetsPath) {
  InputBinaryStream stream(filePath);

  if (!stream.good()) {
    return Result<MaterialAssetHandle>::Error(
        "File cannot be opened for reading: " + filePath.string());
  }

  const auto &header = checkAssetFile(stream, filePath, AssetType::Material);
  if (header.hasError()) {
    return Result<MaterialAssetHandle>::Error(header.getError());
  }

  return loadFromStream(stream, filePath, registry, assetsPath);
}

Result<MaterialAssetHandle> MaterialAssetLoader::getOrLoadFromPath(
    StringView relativePath, AssetRegistry &registry, const Path &assetsPath) {
  if (relativePath.empty()) {
    return Result<MaterialAssetHandle>::Ok(MaterialAssetHandle::Invalid);
  }

  Path fullPath = (assetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : registry.getMaterials().getAssets()) {
    if (asset.path == fullPath) {
      return Result<MaterialAssetHandle>::Ok(handle);
    }
  }

  return loadFromPath(fullPath, registry, assetsPath);
}

Result<Path>
MaterialAssetCreator::createAssetFile(const AssetData<MaterialAsset> &asset,
                                      AssetRegistry &registry,
                                      const Path &assetsPath) {
  String extension = ".lqmat";

  Path assetPath = (assetsPath / (asset.name + extension)).make_preferred();

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.version = createVersion(0, 1);

  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto baseColorTexturePath =
      getAssetRelativePath(registry.getTextures(), asset.data.baseColorTexture);
  file.write(baseColorTexturePath);
  file.write(asset.data.baseColorTextureCoord);
  file.write(asset.data.baseColorFactor);

  auto metallicRoughnessTexturePath = getAssetRelativePath(
      registry.getTextures(), asset.data.metallicRoughnessTexture);
  file.write(metallicRoughnessTexturePath);
  file.write(asset.data.metallicRoughnessTextureCoord);
  file.write(asset.data.metallicFactor);
  file.write(asset.data.roughnessFactor);

  auto normalTexturePath =
      getAssetRelativePath(registry.getTextures(), asset.data.normalTexture);
  file.write(normalTexturePath);
  file.write(asset.data.normalTextureCoord);
  file.write(asset.data.normalScale);

  auto occlusionTexturePath =
      getAssetRelativePath(registry.getTextures(), asset.data.occlusionTexture);
  file.write(occlusionTexturePath);
  file.write(asset.data.occlusionTextureCoord);
  file.write(asset.data.occlusionStrength);

  auto emissiveTexturePath =
      getAssetRelativePath(registry.getTextures(), asset.data.emissiveTexture);
  file.write(emissiveTexturePath);
  file.write(asset.data.emissiveTextureCoord);
  file.write(asset.data.emissiveFactor);

  return Result<Path>::Ok(assetPath);
}

} // namespace liquid
