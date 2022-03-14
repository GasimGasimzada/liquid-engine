#include "liquid/core/Base.h"
#include "SceneRenderer.h"
#include "StandardPushConstants.h"

namespace liquid {

SceneRenderer::SceneRenderer(EntityContext &entityContext_,
                             bool bindMaterialData_)
    : entityContext(entityContext_), bindMaterialData(bindMaterialData_) {}

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline) {
  entityContext.iterateEntities<MeshComponent, TransformComponent>(
      [&commandList, &pipeline, this](Entity entity, const MeshComponent &mesh,
                                      const TransformComponent &transform) {
        if (entityContext.hasComponent<EnvironmentComponent>(entity)) {
          return;
        }
        const auto &instance = mesh.instance;

        auto *transformConstant = new StandardPushConstants;
        transformConstant->modelMatrix = transform.worldTransform;

        commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                  sizeof(StandardPushConstants),
                                  transformConstant);

        for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
          commandList.bindVertexBuffer(instance->getVertexBuffers().at(i));

          if (instance->getMaterials().at(i) && bindMaterialData) {
            commandList.bindDescriptor(
                pipeline, 2, instance->getMaterials().at(i)->getDescriptor());
          }

          if (rhi::isHandleValid(instance->getIndexBuffers().at(i))) {
            commandList.bindIndexBuffer(instance->getIndexBuffers().at(i),
                                        VK_INDEX_TYPE_UINT32);
            commandList.drawIndexed(instance->getIndexCounts().at(i), 0, 0);
          } else {
            commandList.draw(instance->getVertexCounts().at(i), 0);
          }
        }
      });
}

void SceneRenderer::renderSkinned(rhi::RenderCommandList &commandList,
                                  rhi::PipelineHandle pipeline,
                                  uint32_t descriptorSet) {
  entityContext.iterateEntities<SkinnedMeshComponent, SkeletonComponent,
                                TransformComponent>(
      [&pipeline, &commandList, descriptorSet,
       this](Entity entity, const auto &mesh, const SkeletonComponent &skeleton,
             const auto &transform) {
        const auto &instance = mesh.instance;

        auto *transformConstant = new StandardPushConstants;
        transformConstant->modelMatrix = transform.worldTransform;

        commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                  sizeof(StandardPushConstants),
                                  transformConstant);

        rhi::Descriptor skeletonDescriptor;
        skeletonDescriptor.bind(0, skeleton.skeleton.getBuffer(),
                                rhi::DescriptorType::UniformBuffer);

        commandList.bindDescriptor(pipeline, descriptorSet, skeletonDescriptor);

        for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
          commandList.bindVertexBuffer(instance->getVertexBuffers().at(i));

          if (instance->getMaterials().at(i) && bindMaterialData) {
            commandList.bindDescriptor(
                pipeline, 2, instance->getMaterials().at(i)->getDescriptor());
          }

          if (rhi::isHandleValid(instance->getIndexBuffers().at(i))) {
            commandList.bindIndexBuffer(instance->getIndexBuffers().at(i),
                                        VK_INDEX_TYPE_UINT32);
            commandList.drawIndexed(instance->getIndexCounts().at(i), 0, 0);
          } else {
            commandList.draw(instance->getVertexCounts().at(i), 0);
          }
        }
      });
}

} // namespace liquid
