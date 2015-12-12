#include "GaRollingBallComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaRollingBallComponent);

void GaRollingBallComponent::StaticRegisterClass() {
  ReField* Fields[] = {
    new ReField("InitialSize_", &GaRollingBallComponent::InitialSize_,
    bcRFF_IMPORTER),
  };

  ReRegisterClass<GaRollingBallComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor(
          {ScnComponentProcessFuncEntry::Update<GaRollingBallComponent>()}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaRollingBallComponent::GaRollingBallComponent() {
}

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

  Size_ = InitialSize_;
  TravelDir_ = MaVec3d(1.0f, 0.0f, 0.0f);
  
  Rot_ = MaVec3d(0, 0, 0);
  Pos_ = MaVec3d(0, 0, 0);
  Vel_ = TravelDir_;
  RotVel_ = MaVec3d(0, -1.0f, 0.0f).cross(TravelDir_) * 1.0f;
  
  UpdateMatrix();

}

void GaRollingBallComponent::UpdateMatrix()
{
  MaMat4d m = ParentEntity_->getLocalMatrix();
  MaMat4d mTrans;
  MaMat4d mRot;
  mTrans.translation(Pos_);
  mRot.rotation(Rot_);

  m.identity();
  //m.scale(MaVec3d(Size_, Size_, Size_));

  m = m * mRot * mTrans;

  ParentEntity_->setLocalMatrix(m);
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
  Pos_ += Vel_ * Tick;
  Rot_ += RotVel_ * Tick;

  UpdateMatrix();
}