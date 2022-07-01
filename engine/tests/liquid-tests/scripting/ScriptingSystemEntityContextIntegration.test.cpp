#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class ScriptingEntityIntegrationTest : public ::testing::Test {
public:
  ScriptingEntityIntegrationTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName) {
    auto handle =
        assetManager
            .loadLuaScriptFromFile(std::filesystem::current_path() /
                                   "scripting-system-component-tester.lua")
            .getData();

    entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

    scriptingSystem.start(entityDatabase);

    auto &scripting =
        entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

    scripting.scope.luaGetGlobal(functionName);
    scripting.scope.call(0);

    return scripting.scope;
  }

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
};

using ScriptingNameComponentIntegrationTest = ScriptingEntityIntegrationTest;

TEST_F(ScriptingNameComponentIntegrationTest,
       ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();

  auto &scope = call(entity, "name_get");

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(ScriptingNameComponentIntegrationTest, ReturnsEmptyStringIfNoSelf) {
  auto entity = entityDatabase.createEntity();

  auto &scope = call(entity, "name_get_invalid");

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(ScriptingNameComponentIntegrationTest,
       ReturnsNameComponentDataIfExists) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::NameComponent>(entity, {"Test name"});

  auto &scope = call(entity, "name_get");

  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "Test name");
}

TEST_F(ScriptingNameComponentIntegrationTest,
       DoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.createEntity();

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  call(entity, "name_set_invalid");
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
}

TEST_F(ScriptingNameComponentIntegrationTest, CreatesNameComponentOnSet) {
  auto entity = entityDatabase.createEntity();

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  call(entity, "name_set");

  EXPECT_TRUE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  EXPECT_EQ(entityDatabase.getComponent<liquid::NameComponent>(entity).name,
            "Hello World");
}

TEST_F(ScriptingNameComponentIntegrationTest,
       UpdatesExistingNameComponentOnSet) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::NameComponent>(entity, {"Test name"});

  call(entity, "name_set");

  EXPECT_EQ(entityDatabase.getComponent<liquid::NameComponent>(entity).name,
            "Hello World");
}

using ScriptingLocalTransformComponentTest = ScriptingEntityIntegrationTest;
using ScriptingLocalTransformComponentDeathTest =
    ScriptingEntityIntegrationTest;

TEST_F(ScriptingLocalTransformComponentDeathTest,
       GetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_position_get"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest,
       GetPositionReturnsNilIfValuesAreInvalid) {
  auto entity = entityDatabase.createEntity();
  auto &scope = call(entity, "local_transform_position_get_invalid");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_position_x"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_position_y"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_position_z"));
}

TEST_F(ScriptingLocalTransformComponentTest, GetsPositionValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {glm::vec3(2.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_get");

  EXPECT_EQ(scope.getGlobal<float>("local_position_x"), 2.5f);
  EXPECT_EQ(scope.getGlobal<float>("local_position_y"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("local_position_z"), 0.5f);
}

TEST_F(ScriptingLocalTransformComponentDeathTest,
       SetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_position_set"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest, SetsPositionValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_set");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(ScriptingLocalTransformComponentTest,
       DoesNothingIfSetPositionArgumentsAreInvalid) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_set_invalid");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(1.5f, 0.2f, 0.5f));
}

TEST_F(ScriptingLocalTransformComponentDeathTest,
       GetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_scale_get"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest,
       GetScaleReturnsNilIfValuesAreInvalid) {
  auto entity = entityDatabase.createEntity();
  auto &scope = call(entity, "local_transform_scale_get_invalid");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_scale_x"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_scale_y"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_scale_z"));
}

TEST_F(ScriptingLocalTransformComponentTest, GetsScaleValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_get");

  EXPECT_EQ(scope.getGlobal<float>("local_scale_x"), 2.5f);
  EXPECT_EQ(scope.getGlobal<float>("local_scale_y"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("local_scale_z"), 0.5f);
}

TEST_F(ScriptingLocalTransformComponentDeathTest,
       SetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_scale_set"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest, SetsScaleValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_set");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(ScriptingLocalTransformComponentTest,
       DoesNothingIfSetScaleArgumentsAreNotNumbers) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_set_invalid");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(1.5f, 0.2f, 0.5f));
}
