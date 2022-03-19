#include "liquid/core/Base.h"
#include "PhysicsSystem.h"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "PhysxMapping.h"

#include "liquid/core/EngineGlobals.h"

#ifdef LIQUID_PROFILER
static const bool RECORD_MEMORY_ALLOCATIONS = true;
#else
static const bool RECORD_MEMORY_ALLOCATIONS = false;
#endif

using namespace physx;

namespace liquid {

class PhysicsSystem::PhysicsSystemImpl {
public:
  /**
   * @brief Create physics system
   *
   * Initialize Physx
   */
  PhysicsSystemImpl() {
    constexpr uint32_t PVD_PORT = 5425;
    constexpr uint32_t PVD_TIMEOUT_IN_MS = 2000;
    constexpr glm::vec3 GRAVITY(0.0f, -9.8f, 0.0f);

    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator,
                                     mDefaultErrorCallback);

    mPvd = PxCreatePvd(*mFoundation);
    PxPvdTransport *transport = PxDefaultPvdSocketTransportCreate(
        "127.0.0.1", PVD_PORT, PVD_TIMEOUT_IN_MS);
    mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    mPhysics =
        PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),
                        RECORD_MEMORY_ALLOCATIONS, mPvd);

    PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    mDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = mDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.gravity = PxVec3(GRAVITY.x, GRAVITY.y, GRAVITY.z);
    sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    mScene = mPhysics->createScene(sceneDesc);

    auto *pvdClient = mScene->getScenePvdClient();
    if (pvdClient) {
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
  }

  PhysicsSystemImpl(const PhysicsSystemImpl &) = delete;
  PhysicsSystemImpl &operator=(const PhysicsSystemImpl &) = delete;
  PhysicsSystemImpl(PhysicsSystemImpl &&) = delete;
  PhysicsSystemImpl &operator=(PhysicsSystemImpl &&) = delete;

  /**
   * @brief Destroy physics system
   *
   * Destroy PhysX objects
   */
  ~PhysicsSystemImpl() {
    mScene->release();
    mDispatcher->release();
    mPhysics->release();
    mPvd->release();
    mFoundation->release();
  }

  /**
   * @brief Create Physx shape
   *
   * @param geometryDesc Geometry description
   * @param material Physx material
   * @return
   */
  PxShape *createShape(const PhysicsGeometryDesc &geometryDesc,
                       PxMaterial &material) {
    if (geometryDesc.type == PhysicsGeometryType::Sphere) {
      const auto &[radius] =
          std::get<PhysicsGeometrySphere>(geometryDesc.params);
      const auto &geometry = PxSphereGeometry(radius);
      return mPhysics->createShape(geometry, material, true);
    }

    if (geometryDesc.type == PhysicsGeometryType::Box) {
      const auto &[halfExtents] =
          std::get<PhysicsGeometryBox>(geometryDesc.params);
      const auto &geometry =
          PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z);
      return mPhysics->createShape(geometry, material, true);
    }

    if (geometryDesc.type == PhysicsGeometryType::Capsule) {
      const auto &[radius, halfHeight] =
          std::get<PhysicsGeometryCapsule>(geometryDesc.params);
      const auto &geometry = PxCapsuleGeometry(radius, halfHeight);
      return mPhysics->createShape(geometry, material, true);
    }

    if (geometryDesc.type == PhysicsGeometryType::Plane) {
      const auto &geometry = PxPlaneGeometry();
      return mPhysics->createShape(geometry, material, true);
    }

    return nullptr;
  }

  /**
   * @brief Update physx shape with geometry data
   *
   * @param geometryDesc Geometry description
   * @param shape Physx shape
   */
  void updateShapeWithGeometryData(const PhysicsGeometryDesc &geometryDesc,
                                   PxShape *shape) {
    if (geometryDesc.type == PhysicsGeometryType::Sphere) {
      const auto &[radius] =
          std::get<PhysicsGeometrySphere>(geometryDesc.params);
      const auto &geometry = PxSphereGeometry(radius);
      shape->setGeometry(geometry);
    } else if (geometryDesc.type == PhysicsGeometryType::Box) {
      const auto &[halfExtents] =
          std::get<PhysicsGeometryBox>(geometryDesc.params);
      const auto &geometry =
          PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z);
      shape->setGeometry(geometry);
    } else if (geometryDesc.type == PhysicsGeometryType::Capsule) {
      const auto &[radius, halfHeight] =
          std::get<PhysicsGeometryCapsule>(geometryDesc.params);
      const auto &geometry = PxCapsuleGeometry(radius, halfHeight);
      shape->setGeometry(geometry);
    } else if (geometryDesc.type == PhysicsGeometryType::Plane) {
      const auto &geometry = PxPlaneGeometry();
      shape->setGeometry(geometry);
    }
  }

  /**
   * @brief Get PhysX scene
   *
   * @return PhysX scene
   */
  inline PxScene *getScene() { return mScene; }

  /**
   * @brief Get PhysX physics SDK
   * @return PhysX physics SDK
   */
  inline PxPhysics *getPhysics() { return mPhysics; }

private:
  PxDefaultAllocator mDefaultAllocator;
  PxDefaultErrorCallback mDefaultErrorCallback;

  PxPvd *mPvd = nullptr;
  PxFoundation *mFoundation = nullptr;
  PxPhysics *mPhysics = nullptr;
  PxDefaultCpuDispatcher *mDispatcher = nullptr;

  PxScene *mScene = nullptr;
};

PhysicsSystem::PhysicsSystem(EntityContext &entityContext)
    : mEntityContext(entityContext) {
  mImpl = new PhysicsSystemImpl;
}

PhysicsSystem::~PhysicsSystem() { delete mImpl; }

void PhysicsSystem::update(float dt) {
  LIQUID_PROFILE_EVENT("PhysicsSystem::update");

  synchronizeComponents();

  mImpl->getScene()->simulate(dt);
  mImpl->getScene()->fetchResults(true);

  synchronizeTransforms();
}

void PhysicsSystem::synchronizeComponents() {
  LIQUID_PROFILE_EVENT("PhysicsSystem::synchronizeEntitiesWithPhysx");

  {
    LIQUID_PROFILE_EVENT("Cleanup unused static rigid bodies");
    PxActorTypeFlags desiredType = PxActorTypeFlag::eRIGID_STATIC;
    PxU32 count = mImpl->getScene()->getNbActors(desiredType);
    PxActor **buffer = new PxActor *[count];
    mImpl->getScene()->getActors(desiredType, buffer, count);
    for (PxU32 i = 0; i < count; ++i) {
      PxRigidStatic *actor = static_cast<PxRigidStatic *>(buffer[i]);

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      // Destroy actor and shape if there is
      // no collidable component
      if (!mEntityContext.hasComponent<CollidableComponent>(entity)) {
        physx::PxShape *shape = nullptr;
        actor->getShapes(&shape, 1);

        actor->detachShape(*shape);
        shape->release();

        mImpl->getScene()->removeActor(*actor);
      }
    }
    delete[] buffer;
  }

  {
    LIQUID_PROFILE_EVENT("Cleanup unused rigid bodies");
    PxActorTypeFlags desiredType = PxActorTypeFlag::eRIGID_DYNAMIC;
    PxU32 count = mImpl->getScene()->getNbActors(desiredType);
    PxActor **buffer = new PxActor *[count];
    mImpl->getScene()->getActors(desiredType, buffer, count);
    for (PxU32 i = 0; i < count; ++i) {
      PxRigidDynamic *actor = static_cast<PxRigidDynamic *>(buffer[i]);

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      // Destroy shape in actor if collidable component
      // is removed
      if (!mEntityContext.hasComponent<CollidableComponent>(entity) &&
          actor->getNbShapes() > 0) {
        PxShape *shape = nullptr;
        actor->getShapes(&shape, 1);

        actor->detachShape(*shape);
        shape->release();
      }

      // Destroy actor itself if rigid body component is removed
      if (!mEntityContext.hasComponent<RigidBodyComponent>(entity)) {
        mImpl->getScene()->removeActor(*actor);
        actor->release();
      }
    }
    delete[] buffer;
  }

  {
    LIQUID_PROFILE_EVENT("Synchronize collidable components");
    mEntityContext.iterateEntities<CollidableComponent, TransformComponent>(
        [this](auto entity, CollidableComponent &collidable,
               const TransformComponent &transform) {
          // Create or set material
          if (!collidable.material) {
            collidable.material = mImpl->getPhysics()->createMaterial(
                collidable.materialDesc.staticFriction,
                collidable.materialDesc.dynamicFriction,
                collidable.materialDesc.restitution);
          } else {
            collidable.material->setRestitution(
                collidable.materialDesc.restitution);
            collidable.material->setStaticFriction(
                collidable.materialDesc.staticFriction);
            collidable.material->setDynamicFriction(
                collidable.materialDesc.dynamicFriction);
          }

          // Create or set shape
          if (!collidable.shape) {
            collidable.shape = mImpl->createShape(collidable.geometryDesc,
                                                  *collidable.material);
            collidable.material->release();
          } else if (PhysxMapping::getPhysxGeometryType(
                         collidable.geometryDesc.type) ==
                     collidable.shape->getGeometryType()) {
            mImpl->updateShapeWithGeometryData(collidable.geometryDesc,
                                               collidable.shape);
          } else {
            auto *newShape = mImpl->createShape(collidable.geometryDesc,
                                                *collidable.material);

            if (mEntityContext.hasComponent<RigidBodyComponent>(entity)) {
              RigidBodyComponent &rigidBody =
                  mEntityContext.getComponent<RigidBodyComponent>(entity);
              rigidBody.actor->detachShape(*collidable.shape);
              rigidBody.actor->attachShape(*newShape);
            } else {
              collidable.rigidStatic->detachShape(*collidable.shape);
              collidable.rigidStatic->attachShape(*newShape);
            }

            collidable.shape->release();
            collidable.shape = newShape;
          }

          // Create rigid static if no rigid body
          if (!mEntityContext.hasComponent<RigidBodyComponent>(entity) &&
              !collidable.rigidStatic) {
            collidable.rigidStatic = mImpl->getPhysics()->createRigidStatic(
                PhysxMapping::getPhysxTransform(transform.worldTransform));
            collidable.rigidStatic->attachShape(*collidable.shape);
            collidable.rigidStatic->userData =
                reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

            mImpl->getScene()->addActor(*collidable.rigidStatic);
          } else if (collidable.rigidStatic) {
            // Update transform of rigid static if exists
            collidable.rigidStatic->setGlobalPose(
                PhysxMapping::getPhysxTransform(transform.worldTransform));
          }
        });
  }

  {
    LIQUID_PROFILE_EVENT("Synchronize rigid body components");
    mEntityContext.iterateEntities<RigidBodyComponent, TransformComponent>(
        [this](auto entity, RigidBodyComponent &rigidBody,
               TransformComponent &transform) {
          if (!rigidBody.actor) {
            rigidBody.actor = mImpl->getPhysics()->createRigidDynamic(
                PhysxMapping::getPhysxTransform(transform.worldTransform));
            rigidBody.actor->userData =
                reinterpret_cast<void *>(static_cast<uintptr_t>(entity));

            mImpl->getScene()->addActor(*rigidBody.actor);

            // Clear collidable's rigid body if exists
            if (mEntityContext.hasComponent<CollidableComponent>(entity)) {
              CollidableComponent &collidable =
                  mEntityContext.getComponent<CollidableComponent>(entity);
              mImpl->getScene()->removeActor(*collidable.rigidStatic, false);
              collidable.rigidStatic->release();
              collidable.rigidStatic = nullptr;
            }
          }

          if (mEntityContext.hasComponent<CollidableComponent>(entity) &&
              rigidBody.actor->getNbShapes() == 0) {
            rigidBody.actor->attachShape(
                *mEntityContext.getComponent<CollidableComponent>(entity)
                     .shape);
          }

          rigidBody.actor->setMass(rigidBody.dynamicDesc.mass);
          rigidBody.actor->setMassSpaceInertiaTensor(
              {rigidBody.dynamicDesc.inertia.x, rigidBody.dynamicDesc.inertia.y,
               rigidBody.dynamicDesc.inertia.z});
          rigidBody.actor->setGlobalPose(
              PhysxMapping::getPhysxTransform(transform.worldTransform));
        });
  }
}

void PhysicsSystem::synchronizeTransforms() {
  LIQUID_PROFILE_EVENT("PhysicsSystem::synchronizeTransforms");
  uint32_t count = 0;
  auto **actors = mImpl->getScene()->getActiveActors(count);

  {
    LIQUID_PROFILE_EVENT("Synchronize world transforms");
    // calculate world transforms first so that local
    // transforms are calculated from the most recent
    // values
    for (uint32_t i = 0; i < count; ++i) {
      if (actors[i]->getType() != PxActorType::eRIGID_DYNAMIC) {
        continue;
      }
      auto *actor = static_cast<PxRigidActor *>(actors[i]);
      const auto &globalTransform = actor->getGlobalPose();

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                         globalTransform.p.z);
      glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                         globalTransform.q.y, globalTransform.q.z);

      if (mEntityContext.hasComponent<TransformComponent>(entity)) {
        auto &transform =
            mEntityContext.getComponent<TransformComponent>(entity);

        glm::quat emptyQuat;
        glm::vec3 scale;
        glm::vec3 empty3;
        glm::vec4 empty4;

        glm::decompose(transform.worldTransform, scale, emptyQuat, empty3,
                       empty3, empty4);

        transform.worldTransform = glm::translate(glm::mat4{1.0f}, position) *
                                   glm::toMat4(rotation) *
                                   glm::scale(glm::mat4{1.0f}, scale);
      }
    }
  }

  {
    LIQUID_PROFILE_EVENT("Synchronize local transforms");
    // Calculate local transforms from parent world transforms
    for (uint32_t i = 0; i < count; ++i) {
      if (actors[i]->getType() != PxActorType::eRIGID_DYNAMIC) {
        continue;
      }
      auto *actor = static_cast<PxRigidActor *>(actors[i]);
      const auto &globalTransform = actor->getGlobalPose();

      Entity entity =
          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(actor->userData));

      glm::vec3 position(globalTransform.p.x, globalTransform.p.y,
                         globalTransform.p.z);
      glm::quat rotation(globalTransform.q.w, globalTransform.q.x,
                         globalTransform.q.y, globalTransform.q.z);

      if (mEntityContext.hasComponent<TransformComponent>(entity)) {
        auto &transform =
            mEntityContext.getComponent<TransformComponent>(entity);

        if (mEntityContext.hasComponent<TransformComponent>(transform.parent)) {
          const auto &parentTransform =
              mEntityContext.getComponent<TransformComponent>(transform.parent);

          const auto &invParentTransform =
              glm::inverse(parentTransform.worldTransform);

          transform.localPosition =
              glm::vec3(invParentTransform * glm::vec4(position, 1.0));

          transform.localRotation =
              glm::toQuat(invParentTransform * glm::toMat4(rotation));
        } else {
          transform.localPosition = position;
          transform.localRotation = rotation;
        }
      }
    }
  }
}

} // namespace liquid
