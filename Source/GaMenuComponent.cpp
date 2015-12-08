#include "GaMenuComponent.h"

#include "System/Scene/Rendering/ScnCanvasComponent.h"
#include "System/Scene/Rendering/ScnFont.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaMenuComponent);

void GaMenuComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("Canvas_", &GaMenuComponent::Canvas_, bcRFF_TRANSIENT),
      new ReField("Font_", &GaMenuComponent::Font_, bcRFF_TRANSIENT),
      new ReField("DefaultMaterial_", &GaMenuComponent::DefaultMaterial_,
                  bcRFF_TRANSIENT),
  };

  ReRegisterClass<GaMenuComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor(
          {ScnComponentProcessFuncEntry::Update<GaMenuComponent>()}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaMenuComponent::GaMenuComponent()
    : Canvas_(nullptr), Font_(nullptr), DefaultMaterial_(nullptr) {
  SelectedItem_ = 0;
}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaMenuComponent::~GaMenuComponent() {}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaMenuComponent::onAttach(ScnEntityWeakRef Parent) {
  Super::onAttach(Parent);

  Canvas_ =
      getParentEntity()->getComponentAnyParentByType<ScnCanvasComponent>();
  Font_ = getParentEntity()->getComponentAnyParentByType<ScnFontComponent>();
  DefaultMaterial_ =
      getParentEntity()->getComponentByType<ScnMaterialComponent>("default2d");
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaMenuComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);
}

//////////////////////////////////////////////////////////////////////////
// update
// virtual
void GaMenuComponent::update(BcF32 Tick) {
  Canvas_->clear();

  Canvas_->setMaterialComponent(DefaultMaterial_);

  // HACK :: Font flip shouldn't be needed
  MaMat4d fontFlipMat;
  fontFlipMat.scale(MaVec3d(1.0f, -1.0f, 1.0f));
  Canvas_->pushMatrix(fontFlipMat);

  // Hack Define
  BcU16 numBoxes = 5;
  MaVec2d tl = MaVec2d(-0.5f, -0.7f);
  MaVec2d br = MaVec2d(+0.5f, +0.7f);
  BcF32 step = (br.y() - tl.y()) / numBoxes;
  BcF32 padNum = 0.05f;
  BcU32 baseLayer = 10;

  Canvas_->drawBox(tl, br, RsColour::GRAY, baseLayer);
  Canvas_->drawLineBox(tl, br, RsColour::WHITE, baseLayer + 1);

  // Draw Boxes
  MaMat4d mat;
  mat.translation(MaVec3d(0.0f, tl.y(), 0.0f));
  Canvas_->pushMatrix(mat);
  for (BcU16 i = 0; i < numBoxes; ++i) {
    MaVec2d tlBut = MaVec2d(tl.x() + padNum, step * i + padNum);
    MaVec2d brBut = MaVec2d(br.x() - padNum, step * (i + 1) - padNum);

    if (i == SelectedItem_) {
      Canvas_->drawBox(tlBut, brBut, RsColour::BLUE, baseLayer + 2);
      Canvas_->drawLineBox(tlBut, brBut, RsColour::WHITE, baseLayer + 3);
    } else {
      Canvas_->drawBox(tlBut, brBut, RsColour::BLACK, baseLayer + 2);
      Canvas_->drawLineBox(tlBut, brBut, RsColour::GRAY, baseLayer + 3);
    }
  }

  // Draw Text
  // TODO :: Draws upside down?? Look into
  ScnFontDrawParams DrawParams =
      ScnFontDrawParams()
          .setSize(step - padNum * 2)
          .setMargin(padNum)
          .setAlignment(ScnFontAlignment::HCENTRE | ScnFontAlignment::VCENTRE)
          .setTextColour(RsColour::WHITE)
          .setLayer(baseLayer + 4);

  for (BcU16 i = 0; i < numBoxes; ++i) {
    MaVec2d tlBut = MaVec2d(tl.x() + padNum, step * i - padNum);
    MaVec2d brBut = MaVec2d(br.x() - padNum, step * (i + 1) + padNum);

    if (i == SelectedItem_) {
      DrawParams.setTextColour(RsColour::WHITE);
      Font_->drawText(Canvas_, DrawParams, tlBut, brBut - tlBut, "Button");
    } else {
      DrawParams.setTextColour(RsColour::GRAY);
      Font_->drawText(Canvas_, DrawParams, tlBut, brBut - tlBut, "Button");
    }
  }
  Canvas_->popMatrix();

  Canvas_->popMatrix();
}