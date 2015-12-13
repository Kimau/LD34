#pragma once

#include "Psybrus.h"
#include "System/Scene/Rendering/ScnMaterial.h"
#include "System/Scene/Rendering/ScnRenderableComponent.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaRollingBallRenderComponent>
    GaRollingBallRenderComponentRef;

struct GaRollingBallVertex {
  BcF32 X_, Y_, Z_, W_;
  BcF32 U_, V_;
  BcU32 ABGR_;
};

//////////////////////////////////////////////////////////////////////////
// GaState
class GaRollingBallRenderComponent : public ScnRenderableComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaRollingBallRenderComponent,
                             ScnRenderableComponent);

  GaRollingBallRenderComponent();
  virtual ~GaRollingBallRenderComponent();

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  virtual void render(ScnRenderContext& RenderContext);
  virtual MaAABB getAABB() const;

  void generateBallMesh(BcF32 radius);

 private:
  // Psybrus Bits
  RsVertexDeclarationUPtr pVertexDeclaration_ = nullptr;
  RsBufferUPtr pUniformBuffer_;
  RsBufferUPtr pVertexBuffer_;
  RsBufferUPtr pIndexBuffer_;
  RsGeometryBindingUPtr GeometryBinding_;
  ScnShaderObjectUniformBlockData ObjectUniforms_;

  // Materials.
  ScnMaterialRef Material_;
  ScnMaterialComponentRef MaterialComponent_;

  // Submission data.
  GaRollingBallVertex* pVertices_ = nullptr;
  BcU16* pIndexes_ = nullptr;
  BcU32 NoofIndex_ = 0;
  BcU32 NoofVertices_ = 0;
  SysFence UploadFence_;
  SysFence RenderFence_;

  MaAABB AABB_;

  const BcU16 NOOF_VERTS_ROLLING_BALL = 4 * 6;
  const BcU16 NOOF_INDEXES_ROLLING_BALL = 6 * 6;
};
