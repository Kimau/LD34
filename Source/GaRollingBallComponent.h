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

  void ResetBall();

  void UpdateMatrix();

  virtual void onDetach(ScnEntityWeakRef Parent);

  virtual void update(BcF32 Tick);

  void leftOn() { Left_ = BcTrue; }
  void leftOff() { Left_ = BcFalse; }
  void rightOn() { Right_ = BcTrue; }
  void rightOff() { Right_ = BcFalse; }

  const MaVec3d& pos() const { return Pos_; };
  const MaVec3d& vel() const { return Vel_; };
  const BcF32& sz() const { return Size_; };

 private:
  ScnEntity* CraneArm_;
  ScnEntity* JunkBall_;

  BcBool Left_ = BcFalse;
  BcBool Right_ = BcFalse;
  BcF32 CraneArmRot_ = 0.0f;
  BcF32 CraneArmRotCurrSpeed_ = 0.0f;
  BcF32 CraneArmRotSpeed_ = 5.0f;

  BcF32 InitialSize_ = 2.0f;
  BcF32 InitialSpeed_ = 10.0f;

  BcF32 Size_;
  MaVec3d TravelDir_;

  MaVec3d Rot_;
  MaVec3d Pos_;
  MaVec3d RotVel_;
  MaVec3d Vel_;
};
