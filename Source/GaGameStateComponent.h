#pragma once

#include "Psybrus.h"

#include "System/Scene/ScnEntity.h"
#include "System/Os/OsCore.h"

#include "GaCameraComponent.h"

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

 private:
  BcU32 LevelNum_;
  ScnEntity* Cube_ = nullptr;
  GaCameraComponent* Cam_;
};
