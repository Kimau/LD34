#pragma once

#include "Psybrus.h"

#include "System/Scene/ScnEntity.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaRollingBallComponent> GaRollingBallComponentRef;

//////////////////////////////////////////////////////////////////////////
// GaState
class GaRollingBallComponent : public ScnComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaRollingBallComponent, ScnComponent);

  GaRollingBallComponent();
  virtual ~GaRollingBallComponent();

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  virtual void update(BcF32 Tick);

 private:

   BcF32 InitialSize_;
};
