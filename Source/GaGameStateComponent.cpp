

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
      .addAttribute(new ScnComponentProcessor(
          {ScnComponentProcessFuncEntry::Update<GaGameStateComponent>()}));
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

  Cam_ = nullptr;
  Ball_ = nullptr;
  FloorGrid_ = nullptr;

  // Spawn Camera
  auto camSpawn = ScnEntitySpawnParams("CameraEntity_0", "default",
                                       "CameraEntity", MaMat4d(), Parent);
  camSpawn.OnSpawn_ = [this](ScnEntity* NewEntity) {
    Cam_ = NewEntity->getComponentByType<GaCameraComponent>();

  };
  ScnCore::pImpl()->spawnEntity(camSpawn);

  // Spawn Ball
  auto ballSpawn = ScnEntitySpawnParams("TheBall", "game", "RollingBallEntity",
                                        MaMat4d(), Parent);
  ballSpawn.OnSpawn_ = [this](ScnEntity* NewEntity) {
    Ball_ = NewEntity->getComponentByType<GaRollingBallComponent>();
  };
  ScnCore::pImpl()->spawnEntity(ballSpawn);

  // Floor
  MaMat4d Transform;
  Transform.translation(MaVec3d(0, 0.0f, 0));
  auto floorSpawn = ScnEntitySpawnParams("FloorGrid", "game", "FloorEntity",
                                         Transform, Parent);
  floorSpawn.OnSpawn_ =
      [this](ScnEntity* NewEntity) { FloorGrid_ = NewEntity; };
  ScnCore::pImpl()->spawnEntity(floorSpawn);

  using namespace std::placeholders;
  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYDOWN, this,
      std::bind(&GaGameStateComponent::onKeyDown, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYUP, this,
      std::bind(&GaGameStateComponent::onKeyUp, this, _1, _2));

  IsGameStarted_ = false;
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaGameStateComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);

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

void GaGameStateComponent::returnToMenu() {
  //
  auto sc = ScnCore::pImpl();
  sc->removeEntity(getParentEntity());
  sc->spawnEntity(ScnEntitySpawnParams("StartStateE", "start", "StartEntity",
                                       MaMat4d(), nullptr));
}

void GaGameStateComponent::gameStart() {
  RandObj_ = BcRandom(BcRandom::Global.rand());

  // Clear Junk
  while (JunkVector_.empty() == false) {
    auto j = JunkVector_.back();
    JunkVector_.pop_back();
    ParentEntity_->detach(j);
  }

  // Reset Ball
  Ball_->ResetBall();
  auto v = Ball_->vel();
  auto p = Ball_->pos();
  auto ax = v.cross(MaVec3d(0.0f, 1.0f, 0.0f)).normal();

  // Setup Camera
  Cam_->CameraTarget_ = p + v;
  Cam_->CameraRotation_ = MaVec3d(1.3f, 1.61f, 0.0f);
  Cam_->CameraDistance_ = 75.0f;

  // Setup Floor
  updateFloorPosition(p, v);
  IsGameStarted_ = true;
}

void GaGameStateComponent::updateFloorPosition(MaVec3d p, MaVec3d v) {
  MaVec3d gridMid = p + v * 10;
  MaMat4d m;
  m.translation(MaVec3d(gridMid.x() - fmodf(gridMid.x(), 16.0f), 0,
                        gridMid.z() - fmodf(gridMid.z(), 16.0f)));
  FloorGrid_->setLocalMatrix(m);
}

void GaGameStateComponent::update(BcF32 Tick) {
  // Check we have bits we need
  if ((Cam_ == nullptr) || (Ball_ == nullptr)) return;

  if (IsGameStarted_ == false) gameStart();

  // Get Ball Details
  auto v = Ball_->vel();
  auto p = Ball_->pos();
  auto ax = v.cross(MaVec3d(0.0f, 1.0f, 0.0f)).normal();

  // Spawn Junk
  {
    while (JunkVector_.size() < NoofJunk_) {
      MaMat4d m;
      m.translation(p + v * RandObj_.randRealRange(+5.0f, +15.0f) +  // Ahead
                    ax *
                        RandObj_.randRealRange(-30.0f, +30.0f));  // To the side

      // auto sz = RandObj_.randRealRange(0.2f, 3.0f);

      auto j = ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(
          "JunkPiece_00", "game", "CubeEntity", m, ParentEntity_));

      JunkVector_.push_back(j);
    }
  }

  // Camera Follow Ball
  Cam_->CameraTarget_ = p + v * 2.0f;

  updateFloorPosition(p, v);
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

    case 'R':
      gameStart();
      break;

    case OsEventInputKeyboard::KEYCODE_ESCAPE:
      returnToMenu();
      break;
  }

  return evtRET_PASS;
}
