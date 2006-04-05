//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RCLua.h"
#include "RigidChip.h"
#include "Unit1.h"
extern "C" {
  #include "lua/lua.h"
  #include "lua/lualib.h"
  #include "lua/lauxlib.h"
  char *initlocale(char *);
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
int lua_nop(lua_State *L)
{
  lua_pushnumber(L, 0);
  return 1;
}
//---------------------------------------------------------------------------
int lua_out(lua_State *L)
{
  int n = lua_gettop(L);
  int i = lua_tonumber(L, 1);
  AnsiString s;
  for (int i = 2; i <= n; i ++)
    s += lua_tostring(L, i);
  if (0 <= i && i < 100)
  {
    while (Form1->LuaOut->Lines->Count <= i)
      Form1->LuaOut->Lines->Add("");
    Form1->LuaOut->Lines->Strings[i] = s;
  }
  return 0;
}
int lua_DT(lua_State *L)
{
  lua_pushnumber(L, 0.03);
  return 1;
}
int lua_FPS(lua_State *L)
{
  lua_pushnumber(L, 30);
  return 1;
}
int lua_TICKS(lua_State *L)
{
  lua_pushnumber(L, TRCLua::RCLua->Tick);
  return 1;
}
int lua_SETTICKS(lua_State *L)
{
  TRCLua::RCLua->Tick = lua_tonumber(L, 1);
  return 0;
}
int lua_WIDTH(lua_State *L)
{
  lua_pushnumber(L, Form1->PaintPanel->Width);
  return 1;
}
int lua_HEIGHT(lua_State *L)
{
  lua_pushnumber(L, Form1->PaintPanel->Height);
  return 1;
}
int lua_KEY(lua_State *L)
{
  int key = lua_tonumber(L, 1);
  lua_pushnumber(L, Form1->Keys[key] ? 1 : 0);
  return 1;
}
int lua_KEYUP(lua_State *L)
{
  int key = lua_tonumber(L, 1);
  lua_pushnumber(L, Form1->KeyUp[key] ? 0 : 1);
  return 1;
}
int lua_KEYDOWN(lua_State *L)
{
  int key = lua_tonumber(L, 1);
  lua_pushnumber(L, Form1->KeyDown[key] ? 1 : 0);
  return 1;
}
int lua_MX(lua_State *L)
{
  lua_pushnumber(L, Form1->MouseX);
  return 1;
}
int lua_MY(lua_State *L)
{
  lua_pushnumber(L, Form1->MouseY);
  return 1;
}
int lua_ML(lua_State *L)
{
  lua_pushnumber(L, Form1->MouseLeft ? 1 : 0);
  return 1;
}
int lua_MM(lua_State *L)
{
  lua_pushnumber(L, Form1->MouseMiddle ? 1 : 0);
  return 1;
}
int lua_MR(lua_State *L)
{
  lua_pushnumber(L, Form1->MouseRight ? 1 : 0);
  return 1;
}
int lua_H(lua_State *L)
{
  lua_pushnumber(L, -100000);
  return 1;
}
//---------------------------------------------------------------------------
TRCLua *TRCLua::RCLua = NULL;
//---------------------------------------------------------------------------
TRCLua::TRCLua()
{
  RCLua = this;
  Script = NULL;
  ScriptString = "";
  L = NULL;
  initlocale("");
}
//---------------------------------------------------------------------------
TRCLua::~TRCLua()
{
  RCLua = NULL;
  if (L)
    lua_close(L);
  delete[] Script;
}
//---------------------------------------------------------------------------
void TRCLua::SetScript(AnsiString s)
{
  RCLua = this;
  ScriptString = s;
  Tick = 1;

  if (Script) delete[] Script;
  int len = s.Length();
  Script = new char[len+1];
  CopyMemory(Script, s.c_str(), len);
  Script[len] = 0;

  if (L)
    lua_close(L);
  L = lua_open();

  luaopen_base(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_table(L);

  #define LUAREG(func) lua_register(L, #func, lua ## func)
  #define LUANOP(func) lua_register(L, #func, lua_nop)
  lua_register(L, "out", lua_out);
  LUAREG(_DT);
  LUAREG(_FPS);
  LUAREG(_TICKS);
  LUAREG(_SETTICKS);
  LUANOP(_CHIPS);
  LUANOP(_WEIGHT);
  LUAREG(_WIDTH);
  LUAREG(_HEIGHT);
  LUANOP(_FACE);
  LUAREG(_KEY);
  LUAREG(_KEYUP);
  LUAREG(_KEYDOWN);
  LUANOP(_ANALOG);
  LUANOP(_HAT);
  LUAREG(_MX);
  LUAREG(_MY);
  LUAREG(_ML);
  LUAREG(_MM);
  LUAREG(_MR);
  LUANOP(_SETCOLOR);
  LUANOP(_MOVE2D);
  LUANOP(_LINE2D);
  LUANOP(_MOVE3D);
  LUANOP(_LINE3D);
  LUANOP(_X); LUANOP(_Y); LUANOP(_Z);
  LUANOP(_EX); LUANOP(_EY); LUANOP(_EZ);
  LUANOP(_AX); LUANOP(_AY); LUANOP(_AZ);
  LUANOP(_LX); LUANOP(_LY); LUANOP(_LZ);
  LUANOP(_WX); LUANOP(_WY); LUANOP(_WZ);
  LUANOP(_VX); LUANOP(_VY); LUANOP(_VZ); LUANOP(_VEL);
  LUANOP(_FX); LUANOP(_FY); LUANOP(_FZ);
  LUANOP(_GX); LUANOP(_GY); LUANOP(_GZ);
  LUANOP(_XX); LUANOP(_XY); LUANOP(_XZ);
  LUANOP(_YX); LUANOP(_YY); LUANOP(_YZ);
  LUANOP(_ZX); LUANOP(_ZY); LUANOP(_ZZ);
  LUANOP(_QX); LUANOP(_QY); LUANOP(_QZ);
  LUANOP(_RX); LUANOP(_RY); LUANOP(_RZ);
  LUAREG(_H);
  LUANOP(_I);
  LUANOP(_TYPE);
  LUANOP(_PARENT);
  LUANOP(_TOP);
  LUANOP(_DIR);
  LUANOP(_ANGLE);
  LUANOP(_POWER);
  LUANOP(_SPRING);
  LUANOP(_DAMPER);
  LUANOP(_BRAKE);
  LUANOP(_COLOR);
  LUANOP(_OPTION);
  LUANOP(_EFFECT);
  LUANOP(_USER1);
  LUANOP(_USER2);
  LUANOP(_M);
  LUANOP(_E);
  LUANOP(_T);
  LUANOP(_BYE);
  LUANOP(_SPLIT);
  LUANOP(_OX); LUANOP(_OY); LUANOP(_OZ);
  LUANOP(_MOBJ);
  LUANOP(_IOBJ);
  LUANOP(_CCD);
  LUANOP(_RED); LUANOP(_GREEN); LUANOP(_BLUE);
  LUANOP(_ZOOM);
  LUANOP(_PLAYERS);
  LUANOP(_PLAYERHOSTID);
  LUANOP(_PLAYERMYID);
  LUANOP(_PLAYERID);
  LUANOP(_PLAYERCHIPS);
  LUANOP(_PLAYERARMS);
  LUANOP(_PLAYERCOLOR);
  LUANOP(_PLAYERCRUSHES);
  LUANOP(_PLAYERRESETS);
  LUANOP(_PLAYERINITS);
  LUANOP(_PLAYERX); LUANOP(_PLAYERY); LUANOP(_PLAYERZ);
  LUANOP(_PLAYERNAME);
  LUANOP(_PLAYERNAME2);

  lua_settop(L, 0);
  
  if (luaL_loadbuffer(L, Script, len, "") != 0)
  {
    ErrorMessage = lua_tostring(L, -1);
    lua_pop(L, 1);
    return;
  }
  if (lua_pcall(L, 0, 0, 0) != 0)
  {
    ErrorMessage = lua_tostring(L, -1);
    lua_pop(L, 1);
    return;
  }

  lua_pushstring(L, "OnInit");
  lua_gettable(L, LUA_GLOBALSINDEX);
  if (lua_isfunction(L, 1) != 1)
    lua_pop(L, 1);
  else if (lua_pcall(L, 0, 0, 0) != 0)
  {
    ErrorMessage = lua_tostring(L, -1);
    lua_pop(L, 1);
    return;
  }
}
//---------------------------------------------------------------------------
bool TRCLua::Run(TRigidChipCore *core)
{
  RCLua = this;
  if (!L) return false;
  if (ErrorMessage != "") return false;

  lua_settop(L, 0);

  for (TRigidChipsVariableMap::iterator i = core->Variables.begin(); i != core->Variables.end(); i ++)
  {
    lua_pushstring(L, i->first.c_str());
    lua_pushnumber(L, i->second->Value);
    lua_settable(L, LUA_GLOBALSINDEX);
  }
  
  lua_pushstring(L, "OnFrame");
  lua_gettable(L, LUA_GLOBALSINDEX);
  if (lua_isfunction(L, -1) != 1)
  {
    lua_pop(L, 1);
    lua_pushstring(L, "main");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (lua_isfunction(L, -1) != 1)
    {
      lua_pop(L, 1);
      ErrorMessage = "OnFrame/main is not found";
      return false;
    }
  }
  if (lua_pcall(L, 0, 0, 0) != 0)
  {
    ErrorMessage = lua_tostring(L, -1);
    lua_pop(L, 1);
    return false;
  }

  for (TRigidChipsVariableMap::iterator i = core->Variables.begin(); i != core->Variables.end(); i ++)
  {
    lua_pushstring(L, i->first.c_str());
    lua_gettable(L, LUA_GLOBALSINDEX);
    i->second->Value = lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
  
  Tick ++;
  return true;
}
//---------------------------------------------------------------------------

