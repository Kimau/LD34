#pragma once

#include "Psybrus.h"

#include "Base/BcGlobal.h"

//////////////////////////////////////////////////////////////////////////
// GaState
class GaGameTimer : public BcGlobal<GaGameTimer> {
 public:
  GaGameTimer();
  virtual ~GaGameTimer();

  BcF32 Tick() const;
  void SetPause(BcBool isPause);
  void SetMulti(BcF32 newMulti);

 private:
  BcF32 TimeMulti_ = 1.0f;
  BcBool IsPaused_ = BcFalse;
};
