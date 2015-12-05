
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
          // ScnComponentProcessFuncEntry::PreUpdate<GaStartStateComponent>()
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

  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "FloorEntity_0", "start", "FloorEntity", MaMat4d(), Parent));

  using namespace std::placeholders;
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
  // HACK :: Fucking horrid hack because destroy/detach isn't working
  // TODO :: Poke Neil for correct way
	// Moved to 

  // Seems related to deleting the FONT
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "GameStateE", "game", "GameEntity", MaMat4d(), nullptr));
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
    case OsEventInputKeyboard::KEYCODE_RETURN:
      advanceToGame();
      break;
  }

  return evtRET_PASS;
}
