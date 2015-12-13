#include "GaJunkComponent.h"

#include "System/Scene/Rendering/ScnDebugRenderComponent.h"

#include "GaGameTimer.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaJunkComponent);

void GaJunkComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("InitialSize_", &GaJunkComponent::InitialSize_,
                  bcRFF_IMPORTER),
      new ReField("InitialSpeed_", &GaJunkComponent::InitialSpeed_,
                  bcRFF_IMPORTER),

  };

  using namespace std::placeholders;
  ReRegisterClass<GaJunkComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor({ScnComponentProcessFuncEntry(
          "Update", ScnComponentPriority::DEFAULT_PRE_UPDATE,
          std::bind(&GaJunkComponent::updateJunk, _1))}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaJunkComponent::GaJunkComponent() {}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaJunkComponent::~GaJunkComponent() {}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaJunkComponent::onAttach(ScnEntityWeakRef Parent) {
  Super::onAttach(Parent);

  if (InitialSize_ < 2.0f) {
    InitialSize_ = 2.0f;
  }

  // Spawn Junk Ball
  JunkModel_ = nullptr;
  auto ballSpawn = ScnEntitySpawnParams("jmodel_00", "game", "JunkModelEntity",
                                        MaMat4d(), Parent);
  ballSpawn.OnSpawn_ = [this](ScnEntity* NewEntity) { JunkModel_ = NewEntity; };
  ScnCore::pImpl()->spawnEntity(ballSpawn);

  // Get Details
  auto mat = Parent->getLocalMatrix();
  Size_ = mat.row3().w();

  TravelDir_ = MaVec3d(-1.0f, 0.0f, 0.0f);
  Rot_ = MaVec3d(0, 0, 0);
  Vel_ = TravelDir_ * InitialSpeed_;
  RotVel_ = MaVec3d(0, -1.0f, 0.0f).cross(TravelDir_).normal() * 1.0f;

  RBody_ = Parent->getComponentByType<ScnPhysicsRigidBodyComponent>();
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaJunkComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);
}

void GaJunkComponent::updateJunk(const ScnComponentList& Components) {
  BcF32 Tick = GaGameTimer::pImpl()->Tick();

  for (auto Component : Components) {
    BcAssert(Component->isTypeOf<GaJunkComponent>());
    GaJunkComponent* junkComp = static_cast<GaJunkComponent*>(Component.get());

    MaMat4d trans = junkComp->ParentEntity_->getLocalMatrix();
    trans.row3(
        MaVec4d(trans.row3().xyz() + junkComp->Vel_ * Tick, junkComp->Size_));
    junkComp->ParentEntity_->setLocalMatrix(trans);

    if (junkComp->RBody_) {
      junkComp->RBody_->setTransform(trans);
    }

    junkComp->Rot_ += junkComp->RotVel_ *
                      (junkComp->Vel_.magnitude() / junkComp->Size_) * Tick;

    MaMat4d mRot;
    mRot.rotation(junkComp->Rot_);
    junkComp->JunkModel_->setLocalMatrix(mRot);
  }
}
