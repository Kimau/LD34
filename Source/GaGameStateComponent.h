#pragma once

#include "Psybrus.h"

#include "Base/BcRandom.h"
#include "System/Scene/ScnEntity.h"
#include "System/Os/OsCore.h"

#include "GaCameraComponent.h"
#include "GaRollingBallComponent.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaGameStateComponent> GaGameStateComponentRef;

//////////////////////////////////////////////////////////////////////////
// GaState
class GaGameStateComponent : public ScnComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaGameStateComponent, ScnComponent);

  GaGameStateComponent();
  virtual ~GaGameStateComponent();

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  eEvtReturn onKeyUp(EvtID ID, const EvtBaseEvent& Event);
  eEvtReturn onKeyDown(EvtID ID, const EvtBaseEvent& Event);

  void returnToMenu();

  void gameStart();

  virtual void update(BcF32 Tick);

 private:
  BcU32 LevelNum_;
  std::vector<ScnEntity*> JunkVector_;
  GaCameraComponent* Cam_;
  GaRollingBallComponent* Ball_;

  BcU32 NoofJunk_ = 32;
  BcBool IsGameStarted_ = false;
  BcRandom RandObj_;
};
