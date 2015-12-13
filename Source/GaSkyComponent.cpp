#include "GaSkyComponent.h"

#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

#include "System/Content/CsPackage.h"
#include "System/Content/CsCore.h"

#include "System/Debug/DsCore.h"

#include "System/SysKernel.h"

#include "Base/BcHalf.h"
#include "Base/BcProfiler.h"
#include "Base/BcRandom.h"

//////////////////////////////////////////////////////////////////////////
// GaSkyVertex
struct GaSkyVertex {
  GaSkyVertex() {}

  GaSkyVertex(MaVec4d Position, MaVec4d Normal, MaVec4d Tangent, MaVec4d Colour,
              MaVec2d TexCoord)
      : Position_(Position),
        Normal_(Normal),
        Tangent_(Tangent),
        Colour_(Colour),
        TexCoord_(TexCoord) {}

  MaVec4d Position_;
  MaVec4d Normal_;
  MaVec4d Tangent_;
  MaVec4d Colour_;
  MaVec2d TexCoord_;
};

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaSkyComponent);

void GaSkyComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("Material_", &GaSkyComponent::Material_,
                  bcRFF_SHALLOW_COPY | bcRFF_IMPORTER),
      new ReField("MaterialComponent_", &GaSkyComponent::MaterialComponent_,
                  bcRFF_TRANSIENT),
  };

  ReRegisterClass<GaSkyComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor({
          // ScnComponentProcessFuncEntry::PreUpdate< GaSkyComponent >()
      }));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaSkyComponent::GaSkyComponent() {}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaSkyComponent::~GaSkyComponent() {}

//////////////////////////////////////////////////////////////////////////
// render
// virtual
void GaSkyComponent::render(ScnRenderContext& RenderContext) {
  RsRenderSort mySort = RenderContext.Sort_;

  // Material
  {
    // Set model parameters on material.

    RsCore::pImpl()->updateBuffer(
        pUniformBuffer_.get(), 0, sizeof(ObjectUniforms_),
        RsResourceUpdateFlags::ASYNC,
        [this](RsBuffer* Buffer, const RsBufferLock& Lock) {
          BcMemCopy(Lock.Buffer_, &ObjectUniforms_, sizeof(ObjectUniforms_));
        });

    MaterialComponent_->setObjectUniformBlock(pUniformBuffer_.get());
    RenderContext.pViewComponent_->setMaterialParameters(MaterialComponent_);
    MaterialComponent_->bind(RenderContext.pFrame_, RenderContext.Sort_);
  }

  // Queue Render
  RenderContext.pFrame_->queueRenderNode(RenderContext.Sort_, [
    this,
    GeometryBinding = GeometryBinding_.get(),
    DrawProgramBinding = MaterialComponent_->getProgramBinding(),
    RenderState = MaterialComponent_->getRenderState(),
    FrameBuffer = RenderContext.pViewComponent_->getFrameBuffer(),
    Viewport = &RenderContext.pViewComponent_->getViewport()
  ](RsContext * Context) {

    Context->drawPrimitives(GeometryBinding, DrawProgramBinding, RenderState,
                            FrameBuffer, Viewport, nullptr,
                            RsTopologyType::TRIANGLE_STRIP, 0, 4);

  });
}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaSkyComponent::onAttach(ScnEntityWeakRef Parent) {
  BcAssert(Material_ != nullptr);
  Super::onAttach(Parent);

  // -------------- Vertex -----------------
  // Setup Streams
  pVertexDeclaration_ = RsCore::pImpl()->createVertexDeclaration(
      RsVertexDeclarationDesc(5)
          .addElement(RsVertexElement(0, 0, 4, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::POSITION, 0))
          .addElement(RsVertexElement(0, 16, 4, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::NORMAL, 0))
          .addElement(RsVertexElement(0, 32, 4, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::TANGENT, 0))
          .addElement(RsVertexElement(0, 48, 4, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::COLOUR, 0))
          .addElement(RsVertexElement(0, 64, 2, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::TEXCOORD, 0)),
      getFullName().c_str());

  // Allocate render side vertex buffer.
  pVertexBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::VERTEX, RsResourceCreationFlags::STATIC,
                   4 * sizeof(GaSkyVertex)),
      getFullName().c_str());

  RsCore::pImpl()->updateBuffer(
      pVertexBuffer_.get(), 0, 4 * sizeof(GaSkyVertex),
      RsResourceUpdateFlags::ASYNC,
      [](RsBuffer* Buffer, const RsBufferLock& Lock) {
        auto Vertices = reinterpret_cast<GaSkyVertex*>(Lock.Buffer_);
        *Vertices++ = GaSkyVertex(
            MaVec4d(-1.0f, -1.0f, 1.0f, 1.0f) * 10.0f,
            MaVec4d(-1.0f, -1.0f, 1.0f, 0.0f), MaVec4d(1.0f, 0.0f, 0.0f, 1.0f),
            MaVec4d(1.0f, 1.0f, 1.0f, 1.0f), MaVec2d(0.0f, 0.0f));
        *Vertices++ = GaSkyVertex(
            MaVec4d(1.0f, -1.0f, 1.0f, 1.0f) * 10.0f,
            MaVec4d(1.0f, -1.0f, 1.0f, 0.0f), MaVec4d(1.0f, 0.0f, 0.0f, 1.0f),
            MaVec4d(1.0f, 1.0f, 1.0f, 1.0f), MaVec2d(1.0f, 0.0f));
        *Vertices++ = GaSkyVertex(
            MaVec4d(-1.0f, 1.0f, 1.0f, 1.0f) * 10.0f,
            MaVec4d(-1.0f, 1.0f, 1.0f, 0.0f), MaVec4d(1.0f, 0.0f, 0.0f, 1.0f),
            MaVec4d(1.0f, 1.0f, 1.0f, 1.0f), MaVec2d(0.0f, 1.0f));
        *Vertices++ = GaSkyVertex(
            MaVec4d(1.0f, 1.0f, 1.0f, 1.0f) * 10.0f,
            MaVec4d(1.0f, 1.0f, 1.0f, 0.0f), MaVec4d(1.0f, 0.0f, 0.0f, 1.0f),
            MaVec4d(1.0f, 1.0f, 1.0f, 1.0f), MaVec2d(1.0f, 1.0f));
      });

  // -------------- Uniform -----------------
  // Allocate uniform buffer object.
  pUniformBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::UNIFORM, RsResourceCreationFlags::STREAM,
                   sizeof(ScnShaderObjectUniformBlockData)),
      getFullName().c_str());

  MaterialComponent_ = Parent->attach<ScnMaterialComponent>(
      BcName::INVALID, Material_,
      ScnShaderPermutationFlags::MESH_STATIC_3D |
          ScnShaderPermutationFlags::RENDER_FORWARD |
          ScnShaderPermutationFlags::LIGHTING_NONE);

  // -------------- Geometry -----------------
  RsGeometryBindingDesc GeometryBindingDesc;
  GeometryBindingDesc.setVertexBuffer(0, pVertexBuffer_.get(),
                                      sizeof(GaSkyVertex));
  GeometryBindingDesc.setVertexDeclaration(pVertexDeclaration_.get());
  GeometryBinding_ = RsCore::pImpl()->createGeometryBinding(
      GeometryBindingDesc, getFullName().c_str());
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaSkyComponent::onDetach(ScnEntityWeakRef Parent) {
  Super::onDetach(Parent);

  getParentEntity()->detach(MaterialComponent_);

  GeometryBinding_.reset();
  pVertexDeclaration_.reset();
  pVertexBuffer_.reset();
  pUniformBuffer_.reset();
}

//////////////////////////////////////////////////////////////////////////
// getAABB
// virtual
MaAABB GaSkyComponent::getAABB() const { return MaAABB(); }