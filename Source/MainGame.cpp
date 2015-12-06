#include "Psybrus.h"

#include "System/Scene/ScnCore.h"

//////////////////////////////////////////////////////////////////////////
// PsyGameInit
void PsyGameInit() {}

//////////////////////////////////////////////////////////////////////////
// PsyLaunchGame
void PsyLaunchGame() {
  PSY_LOG("Game Started!  \n");

  ScnCore::pImpl()->spawnEntity(
      ScnEntitySpawnParams("DummyE", "default", "Dummy", MaMat4d(), nullptr));

#ifdef DEBUGPOO
  // Spawn Game
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "GameStateE", "game", "GameEntity", MaMat4d(), nullptr));
#else
  // Spawn Start Menu
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "StartStateE", "start", "StartEntity", MaMat4d(), nullptr));
#endif
}
