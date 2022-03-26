#include "liquid/core/Base.h"
#include "liquid/scripting/LuaInterpreter.h"
#include "liquid/scripting/ScriptingUtils.h"

#include <gtest/gtest.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

class LuaInterpreterTest : public ::testing::Test {
public:
  liquid::LuaInterpreter interpreter;
};

TEST_F(LuaInterpreterTest, EvaluateScript) {
  auto *scope = interpreter.createScope();

  auto buffer = liquid::utils::readFileIntoBuffer("component-script.lua");

  interpreter.evaluate(buffer, scope);

  auto *luaScope = static_cast<lua_State *>(scope);

  lua_getglobal(luaScope, "state");
  EXPECT_TRUE(lua_isstring(luaScope, -1));
  EXPECT_EQ(liquid::String(lua_tostring(luaScope, -1)), "none");
  lua_pop(luaScope, -1);

  lua_getglobal(luaScope, "start");
  EXPECT_TRUE(lua_isfunction(luaScope, -1));
  EXPECT_EQ(lua_pcall(luaScope, 0, 1, 0), LUA_OK);
  lua_pop(luaScope, lua_gettop(luaScope));

  lua_getglobal(luaScope, "state");
  EXPECT_TRUE(lua_isstring(luaScope, -1));
  EXPECT_EQ(liquid::String(lua_tostring(luaScope, -1)), "start");
  lua_pop(luaScope, -1);

  lua_getglobal(luaScope, "update");
  EXPECT_TRUE(lua_isfunction(luaScope, -1));
  EXPECT_EQ(lua_pcall(luaScope, 0, 1, 0), LUA_OK);
  lua_pop(luaScope, lua_gettop(luaScope));

  lua_getglobal(luaScope, "state");
  EXPECT_TRUE(lua_isstring(luaScope, -1));
  EXPECT_EQ(liquid::String(lua_tostring(luaScope, -1)), "update");
  lua_pop(luaScope, -1);

  interpreter.destroyScope(scope);
}
