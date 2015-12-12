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
void GaRollingBallComponent::update(BcF32 Tick) {}