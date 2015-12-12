/**************************************************************************
*
* File:   GaCameraComponent.cpp
* Author: Neil Richardson
* Ver/Date: 15/12/12
* Description:
*   Camera component.
*
*
*
*
**************************************************************************/

#include "GaCameraComponent.h"

#include "System/Os/OsCore.h"

#include "Base/BcMath.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaCameraComponent);

void GaCameraComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("CameraTarget_", &GaCameraComponent::CameraTarget_),
      new ReField("CameraRotation_", &GaCameraComponent::CameraRotation_),
      new ReField("CameraWalk_", &GaCameraComponent::CameraWalk_),
      new ReField("CameraDistance_", &GaCameraComponent::CameraDistance_),
      new ReField("CameraZoom_", &GaCameraComponent::CameraZoom_),
      new ReField("CameraState_", &GaCameraComponent::CameraState_),
      new ReField("NextCameraState_", &GaCameraComponent::NextCameraState_),
  };

  ReRegisterClass<GaCameraComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor(
          {ScnComponentProcessFuncEntry::PreUpdate<GaCameraComponent>()}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaCameraComponent::GaCameraComponent() {
  CameraState_ = STATE_IDLE;
  NextCameraState_ = STATE_IDLE;
  CameraDistance_ = 16.0f;
  CameraZoom_ = 0.0f;

#if PLATFORM_ANDROID
  CameraRotation_ = MaVec3d(0.1f, 0.0f, 0.0f);
#endif
  CameraWalk_ = MaVec3d(0.0f, 0.0f, 0.0f);
}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaCameraComponent::~GaCameraComponent() {}

//////////////////////////////////////////////////////////////////////////
// preUpdate
// virtual
void GaCameraComponent::preUpdate(BcF32 Tick) {
#if PLATFORM_ANDROID
  CameraRotation_ += MaVec3d(0.0f, 0.05f, 0.0f) * Tick;
#endif

  // Update state.
  switch (CameraState_) {
    case STATE_IDLE: {
      OsCore::pImpl()->getClient(0)->setMouseLock(BcFalse);
    } break;

    case STATE_ROTATE: {
      OsCore::pImpl()->getClient(0)->setMouseLock(BcTrue);
      BcF32 RotateSpeed = 0.25f;
      CameraRotation_ +=
          MaVec3d(LastMouseEvent_.MouseDY_, LastMouseEvent_.MouseDX_, 0.0f) *
          RotateSpeed * Tick;
    } break;

    case STATE_PAN: {
      OsCore::pImpl()->getClient(0)->setMouseLock(BcTrue);

      BcF32 PanSpeed = 4.0f;
      MaMat4d CameraRotationMatrix = getCameraRotationMatrix();
      MaVec3d OffsetVector =
          MaVec3d(LastMouseEvent_.MouseDX_, LastMouseEvent_.MouseDY_, 0.0f) *
          CameraRotationMatrix;
      CameraTarget_ += OffsetVector * Tick * PanSpeed;
    } break;
  }

  // Keyboard rotation.
  CameraRotation_ += CameraRotationDelta_ * Tick;

  CameraDistance_ += CameraZoom_ * Tick;
  CameraDistance_ = BcClamp(CameraDistance_, 1.0f, 4096.0f);
  CameraZoom_ = 0.0f;

  BcF32 WalkSpeed = 8.0f;
  MaMat4d CameraRotationMatrix = getCameraRotationMatrix();
  MaVec3d OffsetVector = -CameraWalk_ * CameraRotationMatrix;
  CameraTarget_ += OffsetVector * Tick * WalkSpeed;

  MaVec3d ViewDistance = MaVec3d(0.0f, 0.0f, CameraDistance_);
  ViewDistance = ViewDistance * CameraRotationMatrix;
  MaVec3d ViewFromPosition = CameraTarget_ + ViewDistance;

  MaMat4d camMat;
  camMat.lookAt(
      ViewFromPosition, CameraTarget_,
      MaVec3d(CameraRotationMatrix.row1().x(), CameraRotationMatrix.row1().y(),
              CameraRotationMatrix.row1().z()));
  camMat.inverse();
  getParentEntity()->setLocalMatrix(camMat);

  CameraState_ = NextCameraState_;

  // clear event.
  BcMemZero(&LastMouseEvent_, sizeof(LastMouseEvent_));
}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaCameraComponent::onAttach(ScnEntityWeakRef Parent) {
  Super::onAttach(Parent);

  using namespace std::placeholders;

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEDOWN, this,
      std::bind(&GaCameraComponent::onMouseDown, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEUP, this,
      std::bind(&GaCameraComponent::onMouseUp, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEMOVE, this,
      std::bind(&GaCameraComponent::onMouseMove, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_MOUSEWHEEL, this,
      std::bind(&GaCameraComponent::onMouseWheel, this, _1, _2));
  /*
  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYDOWN, this,
      std::bind(&GaCameraComponent::onKeyDown, this, _1, _2));

  OsCore::pImpl()->subscribe(
      osEVT_INPUT_KEYUP, this,
      std::bind(&GaCameraComponent::onKeyUp, this, _1, _2));*/
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaCameraComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);

  OsCore::pImpl()->unsubscribeAll(this);
}

//////////////////////////////////////////////////////////////////////////
// onMouseDown
eEvtReturn GaCameraComponent::onMouseDown(EvtID ID, const EvtBaseEvent& Event) {
  const auto& MouseEvent = Event.get<OsEventInputMouse>();

  if (NextCameraState_ == STATE_IDLE) {
    if (MouseEvent.ButtonCode_ == 2) {
      NextCameraState_ = STATE_ROTATE;
    } else if (MouseEvent.ButtonCode_ == 1) {
      NextCameraState_ = STATE_PAN;
    }

    LastMouseEvent_ = MouseEvent;
  }

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onMouseUp
eEvtReturn GaCameraComponent::onMouseUp(EvtID ID, const EvtBaseEvent& Event) {
  // const auto& MouseEvent = Event.get<OsEventInputMouse>();

  if (NextCameraState_ != STATE_IDLE) {
    NextCameraState_ = STATE_IDLE;
  }

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onMouseMove
eEvtReturn GaCameraComponent::onMouseMove(EvtID ID, const EvtBaseEvent& Event) {
  const auto& MouseEvent = Event.get<OsEventInputMouse>();

  LastMouseEvent_ = MouseEvent;

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onMouseWheel
eEvtReturn GaCameraComponent::onMouseWheel(EvtID ID,
                                           const EvtBaseEvent& Event) {
  const auto& MouseEvent = Event.get<OsEventInputMouseWheel>();

  CameraZoom_ +=
      (CameraDistance_ * CameraDistance_) * -0.01f * MouseEvent.ScrollY_;
  CameraZoom_ = BcClamp(CameraZoom_, -1024.0f, 1024.0f);

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onKeyDown
eEvtReturn GaCameraComponent::onKeyDown(EvtID ID, const EvtBaseEvent& Event) {
  const auto& KeyboardEvent = Event.get<OsEventInputKeyboard>();

  LastKeyboardEvent_ = KeyboardEvent;

  switch (KeyboardEvent.KeyCode_) {
    case OsEventInputKeyboard::KEYCODE_LEFT:
      CameraRotationDelta_.y(-1.0f);
      break;
    case OsEventInputKeyboard::KEYCODE_RIGHT:
      CameraRotationDelta_.y(1.0f);
      break;
    case OsEventInputKeyboard::KEYCODE_UP:
      CameraRotationDelta_.x(-1.0f);
      break;
    case OsEventInputKeyboard::KEYCODE_DOWN:
      CameraRotationDelta_.x(1.0f);
      break;
    case 'W':
      CameraWalk_.z(1.0f);
      break;
    case 'S':
      CameraWalk_.z(-1.0f);
      break;
    case 'A':
      CameraWalk_.x(-1.0f);
      break;
    case 'D':
      CameraWalk_.x(1.0f);
      break;
  }

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// onKeyUp
eEvtReturn GaCameraComponent::onKeyUp(EvtID ID, const EvtBaseEvent& Event) {
  const auto& KeyboardEvent = Event.get<OsEventInputKeyboard>();

  LastKeyboardEvent_ = KeyboardEvent;

  switch (KeyboardEvent.KeyCode_) {
    case OsEventInputKeyboard::KEYCODE_LEFT:
    case OsEventInputKeyboard::KEYCODE_RIGHT:
      CameraRotationDelta_.y(0.0f);
      break;
    case OsEventInputKeyboard::KEYCODE_UP:
    case OsEventInputKeyboard::KEYCODE_DOWN:
      CameraRotationDelta_.x(0.0f);
      break;
    case 'W':
    case 'S':
      CameraWalk_.z(0.0f);
      break;
    case 'A':
    case 'D':
      CameraWalk_.x(0.0f);
      break;
  }

  return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// getCameraRotationMatrix
MaMat4d GaCameraComponent::getCameraRotationMatrix() const {
  MaMat4d CameraPitchMatrix;
  MaMat4d CameraYawMatrix;
  CameraPitchMatrix.rotation(MaVec3d(CameraRotation_.x(), 0.0f, 0.0f));
  CameraYawMatrix.rotation(MaVec3d(0.0f, CameraRotation_.y(), 0.0f));
  return CameraPitchMatrix * CameraYawMatrix;
}