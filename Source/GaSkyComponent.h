#pragma once

#include "Psybrus.h"
#include "System/Scene/Rendering/ScnRenderableComponent.h"

#include "System/Scene/Rendering/ScnMaterial.h"

//////////////////////////////////////////////////////////////////////////
// GaExampleComponentRef
typedef ReObjectRef<class GaSkyComponent> GaSkyComponentRef;

//////////////////////////////////////////////////////////////////////////
// GaSkyComponent
class GaSkyComponent : public ScnRenderableComponent {
 public:
  REFLECTION_DECLARE_DERIVED(GaSkyComponent, ScnRenderableComponent);

  GaSkyComponent();
  virtual ~GaSkyComponent();

  virtual void render(ScnRenderContext& RenderContext);

  virtual void onAttach(ScnEntityWeakRef Parent);
  virtual void onDetach(ScnEntityWeakRef Parent);

  virtual MaAABB getAABB() const;

 private:
  RsVertexDeclarationUPtr pVertexDeclaration_ = nullptr;
  RsBufferUPtr pUniformBuffer_;
  RsBufferUPtr pVertexBuffer_;
  RsGeometryBindingUPtr GeometryBinding_;
  ScnShaderObjectUniformBlockData ObjectUniforms_;

  ScnMaterialRef Material_;
  ScnMaterialComponentRef MaterialComponent_;
};
