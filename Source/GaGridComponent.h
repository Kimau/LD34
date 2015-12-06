#pragma once

#include "Psybrus.h"

#include "System/Scene/Rendering/ScnMaterial.h"
#include "System/Scene/Rendering/ScnRenderableComponent.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaGridComponent> GaGridComponentRef;

struct GaGridComponentVertex {
  BcF32 X_, Y_, Z_, W_;
  BcU32 ABGR_;
};

//////////////////////////////////////////////////////////////////////////
// GaGridComponent
class GaGridComponent : public ScnRenderableComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaGridComponent, ScnRenderableComponent);

  GaGridComponent();
  virtual ~GaGridComponent();

  virtual void render(ScnRenderContext& RenderContext);

  virtual MaAABB getAABB() const;

  virtual void onAttach(ScnEntityWeakRef Parent);

  void generateGrid();

  virtual void onDetach(ScnEntityWeakRef Parent);

 private:
  BcF32 GridSize_ = 1.0f;
  BcU32 NoofGridLines_ = 32;

  // Psybrus Bits
  RsVertexDeclarationUPtr pVertexDeclaration_ = nullptr;
  RsBufferUPtr pUniformBuffer_;
  RsBufferUPtr pVertexBuffer_;
  RsGeometryBindingUPtr GeometryBinding_;
  ScnShaderObjectUniformBlockData ObjectUniforms_;

  // Materials.
  ScnMaterialRef Material_;
  ScnMaterialComponentRef MaterialComponent_;

  // Submission data.
  GaGridComponentVertex* pVertices_ = nullptr;
  GaGridComponentVertex* pVerticesEnd_ = nullptr;
  BcU32 NoofVertices_ = 0;
  SysFence UploadFence_;
  SysFence RenderFence_;
};
