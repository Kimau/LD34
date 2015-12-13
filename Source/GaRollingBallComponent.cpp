#include "GaRollingBallComponent.h"
#include "GaRollingBallRenderComponent.h"

#include "System/Scene/Rendering/ScnDebugRenderComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaRollingBallComponent);

void GaRollingBallComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("InitialSize_", &GaRollingBallComponent::InitialSize_,
                  bcRFF_IMPORTER),
      new ReField("InitialSpeed_", &GaRollingBallComponent::InitialSpeed_,
                  bcRFF_IMPORTER),
      new ReField("CraneArmRotSpeed_",
                  &GaRollingBallComponent::CraneArmRotSpeed_, bcRFF_IMPORTER),

  };

  ReRegisterClass<GaRollingBallComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor(
          {ScnComponentProcessFuncEntry::Update<GaRollingBallComponent>()}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaRollingBallComponent::GaRollingBallComponent() {}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaRollingBallComponent::~GaRollingBallComponent() {}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaRollingBallComponent::onAttach(ScnEntityWeakRef Parent) {
  Super::onAttach(Parent);

  if (InitialSize_ < 2.0f) {
    InitialSize_ = 2.0f;
  }

  // Spawn Junk Ball
  JunkBall_ = nullptr;
  auto ballSpawn = ScnEntitySpawnParams("junkball", "game", "JunkBallEntity",
                                        MaMat4d(), Parent);
  ballSpawn.OnSpawn_ = [this](ScnEntity* NewEntity) { JunkBall_ = NewEntity; };
  ScnCore::pImpl()->spawnEntity(ballSpawn);

  // Spawn Crane Arm
  CraneArm_ = nullptr;
  auto armSpawn = ScnEntitySpawnParams("myarm", "game", "CraneArmEntity",
                                       MaMat4d(), Parent);
  armSpawn.OnSpawn_ = [this](ScnEntity* NewEntity) { CraneArm_ = NewEntity; };
  ScnCore::pImpl()->spawnEntity(armSpawn);

  ResetBall();

  UpdateMatrix();
}

void GaRollingBallComponent::ResetBall() {
  Size_ = InitialSize_;
  TravelDir_ = MaVec3d(1.0f, 0.0f, 0.0f);

  Rot_ = MaVec3d(0, 0, 0);
  Pos_ = MaVec3d(0, 0, 0);
  Vel_ = TravelDir_ * InitialSpeed_;
  RotVel_ = MaVec3d(0, -1.0f, 0.0f).cross(TravelDir_).normal() * 1.0f;

  CraneArmRot_ = 0.0f;
}

void GaRollingBallComponent::UpdateMatrix() {
  // Set Main Body
  {
    MaMat4d mTrans;
    mTrans.translation(Pos_);
    ParentEntity_->setLocalMatrix(mTrans);
  }

  // Set Crane Arm
  {
    MaMat4d caMat;
    MaMat4d mTrans;
    mTrans.translation(TravelDir_ * Size_);
    caMat.rotation(MaVec3d(0.0f, -CraneArmRot_, 0.0f));

    caMat = mTrans * caMat;
    CraneArm_->setLocalMatrix(caMat);
  }

  // Set Junk Ball
  {
    MaMat4d jbMat;
    MaMat4d mRot;
    mRot.rotation(Rot_);
    jbMat.scale(MaVec3d(Size_, Size_, Size_));
    jbMat = jbMat * mRot;
    JunkBall_->setLocalMatrix(jbMat);
  }
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaRollingBallComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);
}

//////////////////////////////////////////////////////////////////////////
// update
// virtual
void GaRollingBallComponent::update(BcF32 Tick) {
  // Update Details
  Size_ -= 0.5f * Tick;
  if (Size_ < 0.1f) {
    ResetBall();
  }

  // ScnDebugRenderComponent::pImpl()->drawAABB(JunkBall_->getComponentByType<GaRollingBallRenderComponent>()->getAABB(),
  // RsColour(0.0,1.0f,0.0f,1.0f));

  // Move Crane Arm
  IsShooting_ = BcFalse;
  if (Left_ && Right_) {
    // Shoot Crane Arm
    MaMat4d caMat;
    caMat.rotation(MaVec3d(0.0f, -CraneArmRot_, 0.0f));
    MaVec3d dirVec = MaVec3d(1.0f, 0.0f, 0.0f) * caMat;
    ScnDebugRenderComponent::pImpl()->drawLine(
        ParentEntity_->getWorldPosition(),
        ParentEntity_->getWorldPosition() + dirVec * 100.0f,
        RsColour(1.0f, 0.0f, 0.0f, 1.0f));

    ShootRay_ = dirVec;
    IsShooting_ = BcTrue;
  } else if (Left_) {
    CraneArmRotCurrSpeed_ =
        BcLerp(CraneArmRotCurrSpeed_, CraneArmRotSpeed_, Tick);
    CraneArmRot_ -= CraneArmRotCurrSpeed_ * Tick;
  } else if (Right_) {
    CraneArmRotCurrSpeed_ =
        BcLerp(CraneArmRotCurrSpeed_, CraneArmRotSpeed_, Tick);
    CraneArmRot_ += CraneArmRotCurrSpeed_ * Tick;
  } else {
    CraneArmRotCurrSpeed_ = 0.0f;
  }

  // Move Ball
  Pos_ += Vel_ * Tick;
  Pos_ = MaVec3d(Pos_.x(), Size_ * 0.5f, Pos_.z());
  Rot_ += RotVel_ * (Vel_.magnitude() / Size_) * Tick;

  UpdateMatrix();
}

void GaRollingBallComponent::eatJunk(GaJunkComponent* junk) {
  Size_ += junk->sz();
}
