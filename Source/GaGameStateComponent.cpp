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
 auto cam = 
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
    "CameraEntity_0", "default", "CameraEntity", MaMat4d(), Parent));
  
  if (cam != nullptr) {
    Cam_ = cam->getComponentByType<GaCameraComponent>();

    Cam_->CameraTarget_ = MaVec3d(0.0f, -10.0f, 0.0f);
    Cam_->CameraRotation_ = MaVec3d(0.6f, 0.1f, 0.0);
    Cam_->CameraDistance_ = 25.0f;
  }

  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
    "CubeEntity_0", "game", "CubeEntity", MaMat4d(), Parent));

  {
    MaMat4d Transform;
    Transform.translation(MaVec3d(0, +3.0f, 0));

    ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "CubeEntity_0", "game", "CubeEntity", Transform, Parent));
  }
  /*
  MaMat4d Transform;
  Transform.translation(MaVec3d(0, -10.0f, 0));
  ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
      "FloorGrid", "game", "FloorEntity", Transform, Parent));
      */
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

void GaGameStateComponent::returnToMenu()
{
  //
  auto sc = ScnCore::pImpl();
  sc->removeEntity(getParentEntity());
  sc->spawnEntity(ScnEntitySpawnParams("StartStateE", "start", "StartEntity",
    MaMat4d(), nullptr));
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

    case OsEventInputKeyboard::KEYCODE_ESCAPE:
      returnToMenu();
      break;
  }

  return evtRET_PASS;
}
