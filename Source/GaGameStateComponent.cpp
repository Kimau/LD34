#include "Base/BcRandom.h"

#include "GaGameStateComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaGameStateComponent);

void GaGameStateComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("LevelNum_", &GaGameStateComponent::LevelNum_,
                  bcRFF_IMPORTER),
  };

  ReRegisterClass<GaGameStateComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor({
          // ScnComponentProcessFuncEntry::PreUpdate<GaGameStateComponent>()
      }));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaGameStateComponent::GaGameStateComponent() {}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaGameStateComponent::~GaGameStateComponent() {}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaGameStateComponent::onAttach(ScnEntityWeakRef Parent) {
  Super::onAttach(Parent);

  // Spawn Game Bits
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "CameraEntity_0", "default", "CameraEntity", MaMat4d(), Parent));

  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "FloorGrid", "game", "FloorEntity", MaMat4d(), Parent));

  using namespace std::placeholders;
  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYDOWN, this,
      std::bind(&GaGameStateComponent::onKeyDown, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYUP, this,
      std::bind(&GaGameStateComponent::onKeyUp, this, _1, _2));
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaGameStateComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);
  Cube_ = nullptr;

  OsCore::pImpl()->unsubscribeAll(this);
}

//////////////////////////////////////////////////////////////////////////
// onKeyDown
eEvtReturn GaGameStateComponent::onKeyDown(EvtID ID,
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
  }

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onKeyUp
eEvtReturn GaGameStateComponent::onKeyUp(EvtID ID, const EvtBaseEvent& Event) {
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
  }

  return evtRET_PASS;
}
