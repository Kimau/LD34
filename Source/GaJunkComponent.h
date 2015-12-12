#pragma once

#include "Psybrus.h"

#include "System/Scene/ScnEntity.h"
#include "System/Scene/Physics/ScnPhysicsRigidBodyComponent.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaJunkComponent> GaJunkComponentRef;

//////////////////////////////////////////////////////////////////////////
// GaState
class GaJunkComponent : public ScnComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaJunkComponent, ScnComponent);

  GaJunkComponent();
  virtual ~GaJunkComponent();

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  const MaVec3d& vel() const { return Vel_; };
  const BcF32& sz() const { return Size_; };

 private:
  static void updateJunk(const ScnComponentList& Components);

  ScnEntity* JunkModel_;
  ScnPhysicsRigidBodyComponent* RBody_;

  BcF32 InitialSize_ = 2.0f;
  BcF32 InitialSpeed_ = 10.0f;

  BcF32 Size_;
  MaVec3d TravelDir_;

  MaVec3d Rot_;
  MaVec3d RotVel_;
  MaVec3d Vel_;
};
