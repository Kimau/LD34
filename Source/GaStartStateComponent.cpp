
#include "GaStartStateComponent.h"

#include "System/Os/OsCore.h"

#include "GaMenuComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaStartStateComponent);

void GaStartStateComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("LevelNum_", &GaStartStateComponent::LevelNum_,
                  bcRFF_IMPORTER),
  };

  ReRegisterClass<GaStartStateComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor({
          ScnComponentProcessFuncEntry::Update<GaStartStateComponent>(),
      }));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaStartStateComponent::GaStartStateComponent() {}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaStartStateComponent::~GaStartStateComponent() {}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaStartStateComponent::onAttach(ScnEntityWeakRef Parent) {
  Super::onAttach(Parent);

  // Spawn Bits
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "CameraEntity_0", "default", "CameraEntity", MaMat4d(), Parent));

  MaMat4d Transform;
  Transform.translation(MaVec3d(0, -10.0f, 0));
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "FloorGrid", "start", "FloorEntity", Transform, Parent));

  using namespace std::placeholders;

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEDOWN, this,
      std::bind(&GaStartStateComponent::onMouseDown, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEUP, this,
      std::bind(&GaStartStateComponent::onMouseUp, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEMOVE, this,
      std::bind(&GaStartStateComponent::onMouseMove, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYDOWN, this,
      std::bind(&GaStartStateComponent::onKeyDown, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYUP, this,
      std::bind(&GaStartStateComponent::onKeyUp, this, _1, _2));
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaStartStateComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);

  OsCore::pImpl()->unsubscribeAll(this);
  ViewComp_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// onMouseDown
eEvtReturn GaStartStateComponent::onMouseDown(EvtID ID,
                                              const EvtBaseEvent& Event) {
  const auto& MouseEvent = Event.get<OsEventInputMouse>();

  // MouseEvent.ButtonCode_
  LastMouseEvent_ = MouseEvent;

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onMouseUp
eEvtReturn GaStartStateComponent::onMouseUp(EvtID ID,
                                            const EvtBaseEvent& Event) {
  // const auto& MouseEvent = Event.get<OsEventInputMouse>();

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onMouseMove
eEvtReturn GaStartStateComponent::onMouseMove(EvtID ID,
                                              const EvtBaseEvent& Event) {
  const auto& MouseEvent = Event.get<OsEventInputMouse>();
  LastMouseEvent_ = MouseEvent;

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onKeyDown
eEvtReturn GaStartStateComponent::onKeyDown(EvtID ID,
                                            const EvtBaseEvent& Event) {
  const auto& KeyboardEvent = Event.get<OsEventInputKeyboard>();

  switch (KeyboardEvent.KeyCode_) {
    case 'A':
    case OsEventInputKeyboard::KEYCODE_LEFT:
      break;
    case 'D':
    case OsEventInputKeyboard::KEYCODE_RIGHT:
      break;
    case 'W':
    case OsEventInputKeyboard::KEYCODE_UP:
      break;
    case 'S':
    case OsEventInputKeyboard::KEYCODE_DOWN:
      break;
    case OsEventInputKeyboard::KEYCODE_RETURN:
      break;
  }

  return evtRET_PASS;
}

void GaStartStateComponent::advanceToGame() {
  //
  auto sc = ScnCore::pImpl();
  sc->removeEntity(getParentEntity());
  sc->spawnEntity(ScnEntitySpawnParams("GameStateE", "game", "GameEntity",
                                       MaMat4d(), nullptr));
}

void GaStartStateComponent::update(BcF32 Tick) {
  if (ViewComp_ == nullptr) {
    ViewComp_ = ParentEntity_->getComponentByType<ScnViewComponent>();
  }
}

//////////////////////////////////////////////////////////////////////////
// onKeyUp
eEvtReturn GaStartStateComponent::onKeyUp(EvtID ID, const EvtBaseEvent& Event) {
  const auto& KeyboardEvent = Event.get<OsEventInputKeyboard>();

  switch (KeyboardEvent.KeyCode_) {
    case 'A':
    case OsEventInputKeyboard::KEYCODE_LEFT:
      break;
    case 'D':
    case OsEventInputKeyboard::KEYCODE_RIGHT:
      break;
    case 'W':
    case OsEventInputKeyboard::KEYCODE_UP:
      break;
    case 'S':
    case OsEventInputKeyboard::KEYCODE_DOWN:
      break;
    case OsEventInputKeyboard::KEYCODE_PGUP:
      {
        // Hack for now
        GaMenuComponent* menu = getComponentByType<GaMenuComponent>();
        if (menu != nullptr) {
          menu->SelectedItem_ = (menu->SelectedItem_ + 4 - 1) % 4;
        }
      }
        break;
    case OsEventInputKeyboard::KEYCODE_PGDN:
      {
        // Hack for now
        GaMenuComponent* menu = getComponentByType<GaMenuComponent>();
        if (menu != nullptr) {
          menu->SelectedItem_ = (menu->SelectedItem_ + 4 +1) % 4;
        }
      }
        break;
    case OsEventInputKeyboard::KEYCODE_RETURN:
      advanceToGame();
      break;
  }

  return evtRET_PASS;
}
