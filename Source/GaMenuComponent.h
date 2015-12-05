#pragma once

#include "Psybrus.h"

#include "System/Scene/ScnEntity.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaMenuComponent> GaMenuComponentRef;

//////////////////////////////////////////////////////////////////////////
// GaState
class GaMenuComponent : public ScnComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaMenuComponent, ScnComponent);

  GaMenuComponent();
  virtual ~GaMenuComponent();

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  virtual void update(BcF32 Tick);

 private:
  class ScnCanvasComponent* Canvas_;
  class ScnFontComponent* Font_;
  class ScnMaterialComponent* DefaultMaterial_;
};
