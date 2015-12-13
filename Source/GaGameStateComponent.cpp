

#include "GaGameStateComponent.h"
#include "GaJunkComponent.h"
#include "GaGameTimer.h"
#include "System/Scene/Rendering/ScnDebugRenderComponent.h"

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

  // Physics World
  World_ = getComponentByType<ScnPhysicsWorldComponent>();

  IsGameStarted_ = false;
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaGameStateComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);

  OsCore::pImpl()->unsubscribeAll(this);
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
    ScnCore::pImpl()->removeEntity(j);
    // ParentEntity_->detach(j);
  }

  // Reset Ball
  Ball_->ResetBall();
  auto v = Ball_->vel();
  auto p = Ball_->pos();
  auto ax = v.cross(MaVec3d(0.0f, 1.0f, 0.0f)).normal();

  // Spawn First Wave of Junk
  auto vDir = v.normal();
  while (JunkVector_.size() < NoofJunk_) {
    MaVec3d spawnPos = p +
                       vDir * RandObj_.randRealRange(+2.0f, +150.0f) +  // Ahead
                       ax * RandObj_.randRealRange(-60.0f, +60.0f);
    MaVec3d spawnDir = MaVec3d(RandObj_.randRealRange(-1.5f, -0.5f), 0.0f,
                               RandObj_.randRealRange(-0.3f, 0.3f));
    BcF32 spawnSize = RandObj_.randRealRange(1.0f, 2.5f);

    SpawnJunk(spawnPos, spawnDir, spawnSize);
  }

  // Setup Camera
  Cam_->CameraTarget_ = p + v;
  Cam_->CameraRotation_ = MaVec3d(1.3f, 1.61f, 0.0f);
  Cam_->CameraDistance_ = 75.0f;

  // Setup Floor
  updateFloorPosition(p, v);
  IsGameStarted_ = true;
}

void GaGameStateComponent::SpawnJunk(MaVec3d spawnPos, MaVec3d spawnDir,
                                     BcF32 spawnSize) {
  MaMat4d m;
  m.translation(spawnPos);  // To the side

  static BcU32 jCounter = 0;
  BcChar buf[130];
  BcSPrintf(buf, 128, "JunkPiece_%d", jCounter++);
  auto j = ScnCore::pImpl()->spawnEntity(ScnEntitySpawnParams(

      buf, "game", "JunkEntity", m, ParentEntity_));

  GaJunkComponent* jnkComp = j->getComponentByType<GaJunkComponent>();

  jnkComp->Rot_ = MaVec3d(RandObj_.randRealRange(0, BcPIMUL2),
                          RandObj_.randRealRange(0, BcPIMUL2),
                          RandObj_.randRealRange(0, BcPIMUL2));
  jnkComp->TravelDir_ = spawnDir.normal();
  jnkComp->Size_ = spawnSize;

  JunkVector_.push_back(j);
}

void GaGameStateComponent::updateFloorPosition(MaVec3d p, MaVec3d v) {
  MaVec3d gridMid = p + v * 10;
  MaMat4d m;
  m.translation(MaVec3d(gridMid.x() - fmodf(gridMid.x(), 16.0f), 0,
                        gridMid.z() - fmodf(gridMid.z(), 16.0f)));
  FloorGrid_->setWorldMatrix(m);
}

void GaGameStateComponent::update(BcF32 Tick) {
  // Check we have bits we need
  if ((Cam_ == nullptr) || (Ball_ == nullptr)) return;

  if (IsGameStarted_ == false) gameStart();

  Tick = GaGameTimer::pImpl()->Tick();

  // Get Ball Details
  auto v = Ball_->vel();
  auto vDir = v.normal();
  auto p = Ball_->pos();
  auto sz = Ball_->sz();
  auto ax = v.cross(MaVec3d(0.0f, 1.0f, 0.0f)).normal();

  //
  if (Ball_->isShooting()) {
    GaGameTimer::pImpl()->SetMulti(0.3f);

    ScnPhysicsLineCastResult result;
    if (World_->lineCast(p, p + Ball_->getRay() * 100.0f, &result) == BcTrue) {
      ScnDebugRenderComponent::pImpl()->drawCircle(
          result.Intersection_, MaVec3d(1.0f, 1.0f, 1.0f),
          RsColour(1.0f, 0.0f, 0.0f, 1.0f));

      GaJunkComponent* j =
          result.Entity_->getComponentByType<GaJunkComponent>();
      Ball_->eatJunk(j);
      JunkVector_.remove(result.Entity_);
      ParentEntity_->detach(result.Entity_);
    }
  } else if (Ball_->IsAiming()) {
    GaGameTimer::pImpl()->SetMulti(0.1f);
  } else {
    GaGameTimer::pImpl()->SetMulti(1.0f);
  }

  // Junk Update
  {
    std::list<ScnEntity*> DelVector;
    for (auto j : JunkVector_) {
      auto jp = j->getWorldPosition();
      auto jdir = jp - p;

      if (jdir.dot(vDir) < -100.0f) {
        DelVector.push_back(j);
      }
    }

    for (auto j : DelVector) {
      JunkVector_.remove(j);
      ParentEntity_->detach(j);
    }

    while (JunkVector_.size() < NoofJunk_) {
      MaVec3d spawnPos = p +
                         v * RandObj_.randRealRange(+80.0f, +120.0f) +  // Ahead
                         ax * RandObj_.randRealRange(-60.0f, +60.0f);
      MaVec3d spawnDir = MaVec3d(RandObj_.randRealRange(-1.5f, -0.5f), 0.0f,
                                 RandObj_.randRealRange(-0.3f, 0.3f));
      BcF32 spawnSize = RandObj_.randRealRange(sz * 0.5f, sz * 2.0f);

      SpawnJunk(spawnPos, spawnDir, spawnSize);
    }
  }

  // Camera Update
  UpdateCameraLogic();

  updateFloorPosition(p, v);
}

void GaGameStateComponent::UpdateCameraLogic() {
  auto v = Ball_->vel();
  auto p = Ball_->pos();

  auto ax = v.cross(MaVec3d(0.0f, 1.0f, 0.0f)).normal();
  auto sz = Ball_->sz();

  if (Ball_->isShooting()) {
    // Aiming Camera
    Cam_->NextCameraState_ = Cam_->CameraState_ = GaCameraComponent::STATE_IDLE;

    Cam_->CameraTarget_ = p + v;
    Cam_->CameraRotation_ = MaVec3d(1.3f, 1.61f, 0.0f);
    Cam_->CameraDistance_ = 50.0f;
  } else if (Ball_->IsAiming()) {
    // Aiming Camera
    Cam_->NextCameraState_ = Cam_->CameraState_ = GaCameraComponent::STATE_IDLE;

    Cam_->CameraTarget_ = p + v;
    Cam_->CameraRotation_ = MaVec3d(1.3f, 1.61f, 0.0f);
    Cam_->CameraDistance_ = 75.0f;
  } else {
    // Normal Camera
    Cam_->NextCameraState_ = Cam_->CameraState_ =
        GaCameraComponent::STATE_FORCED;
    Cam_->CameraForcedPosition_ = p + MaVec3d(0.0f, sz * 3.0f, 0.0f) - v * 2;
    Cam_->CameraTarget_ = p + MaVec3d(0, 1.0f, 0) * sz + v * 5.0f;
  }
}

//////////////////////////////////////////////////////////////////////////
// onKeyDown
eEvtReturn GaGameStateComponent::onKeyDown(EvtID ID,
                                           const EvtBaseEvent& Event) {
  const auto& KeyboardEvent = Event.get<OsEventInputKeyboard>();

  switch (KeyboardEvent.KeyCode_) {
    case 'A':
    case OsEventInputKeyboard::KEYCODE_LEFT:
      Ball_->leftOn();
      break;
    case 'D':
    case OsEventInputKeyboard::KEYCODE_RIGHT:
      Ball_->rightOn();
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
      Ball_->leftOff();
      break;
    case 'D':
    case OsEventInputKeyboard::KEYCODE_RIGHT:
      Ball_->rightOff();
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
