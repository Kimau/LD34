#pragma once

#include "Psybrus.h"

#include "System/Scene/ScnEntity.h"
#include "System/Os/OsCore.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaStartStateComponent> GaStartStateComponentRef;

//////////////////////////////////////////////////////////////////////////
// GaState
class GaStartStateComponent : public ScnComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaStartStateComponent, ScnComponent);

  GaStartStateComponent();
  virtual ~GaStartStateComponent();

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  eEvtReturn onKeyUp(EvtID ID, const EvtBaseEvent& Event);
  eEvtReturn onKeyDown(EvtID ID, const EvtBaseEvent& Event);

  void advanceToGame();

 private:
  BcU32 LevelNum_;
};
