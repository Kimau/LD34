
#include "GaGameTimer.h"
#include "System/SysKernel.h"

GaGameTimer::GaGameTimer() {}

GaGameTimer::~GaGameTimer() {}

BcF32 GaGameTimer::Tick() const {
  if (IsPaused_) return 0;

  return SysKernel::pImpl()->getFrameTime() * TimeMulti_;
}

void GaGameTimer::SetPause(BcBool isPause) { IsPaused_ = isPause; }
void GaGameTimer::SetMulti(BcF32 newMulti) { TimeMulti_ = newMulti; }
