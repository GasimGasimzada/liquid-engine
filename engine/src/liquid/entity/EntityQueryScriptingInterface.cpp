#include "liquid/core/Base.h"
#include "liquid/scripting/EntityDecorator.h"

#include "EntityQuery.h"
#include "EntityQueryScriptingInterface.h"

namespace liquid {

int EntityQueryScriptingInterface::LuaInterface::getFirstEntityByName(
    void *state) {
  LuaScope scope(state);
  scope.stackDump();

  if (!scope.is<String>(1)) {
    scope.set(nullptr);
    // TODO: Show logs here
    return 1;
  }

  auto arg = scope.get<StringView>(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  EntityQuery query(entityDatabase);

  auto entity = query.getFirstEntityByName(arg);
  if (entity == EntityNull) {
    scope.set(nullptr);
  } else {
    EntityDecorator::createEntityTable(scope, entity);
  }

  return 1;
}

} // namespace liquid
