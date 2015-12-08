#pragma once

#include "Psybrus.h"

#include "System/Scene/ScnEntity.h"
#include "System/Os/OsCore.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

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

  eEvtReturn onMouseDown(EvtID ID, const EvtBaseEvent& Event);
  eEvtReturn onMouseUp(EvtID ID, const EvtBaseEvent& Event);
  eEvtReturn onMouseMove(EvtID ID, const EvtBaseEvent& Event);

  eEvtReturn onKeyDown(EvtID ID, const EvtBaseEvent& Event);
  eEvtReturn onKeyUp(EvtID ID, const EvtBaseEvent& Event);

  void advanceToGame();
  void update(BcF32 Tick);

 private:
  BcU32 LevelNum_;
  OsEventInputMouse LastMouseEvent_;
  ScnViewComponent* ViewComp_;
};
