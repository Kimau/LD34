#include "Base/BcRandom.h"

#include "GaRollingBallRenderComponent.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaRollingBallRenderComponent);

void GaRollingBallRenderComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("Material_", &GaRollingBallRenderComponent::Material_,
                  bcRFF_SHALLOW_COPY | bcRFF_IMPORTER),
      new ReField("MaterialComponent_",
                  &GaRollingBallRenderComponent::MaterialComponent_,
                  bcRFF_TRANSIENT),
  };

  ReRegisterClass<GaRollingBallRenderComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor({}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaRollingBallRenderComponent::GaRollingBallRenderComponent() {}

//////////////////////////////////////////////////////////////////////////
// Dtor
// virtual
GaRollingBallRenderComponent::~GaRollingBallRenderComponent() {}

//////////////////////////////////////////////////////////////////////////
// onAttach
// virtual
void GaRollingBallRenderComponent::onAttach(ScnEntityWeakRef Parent) {
  BcAssert(Material_ != nullptr);

  Super::onAttach(Parent);

  // Allocate our own vertex buffer data.
  NoofVertices_ = NOOF_VERTS_ROLLING_BALL;
  NoofIndex_ = NOOF_INDEXES_ROLLING_BALL;
  pVertices_ = new GaRollingBallVertex[NoofVertices_];
  pIndexes_ = new BcU16[NoofIndex_];

  // Setup Streams
  pVertexDeclaration_ = RsCore::pImpl()->createVertexDeclaration(
      RsVertexDeclarationDesc(3)
          .addElement(RsVertexElement(0, 0, 4, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::POSITION, 0))
          .addElement(RsVertexElement(0, 16, 2, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::TEXCOORD, 0))
          .addElement(RsVertexElement(0, 24, 4, RsVertexDataType::UBYTE_NORM,
                                      RsVertexUsage::COLOUR, 0)),
      getFullName().c_str());

  // Allocate render side vertex buffer.
  pVertexBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::VERTEX, RsResourceCreationFlags::STREAM,
                   NoofVertices_ * sizeof(GaRollingBallVertex)),
      getFullName().c_str());

  pIndexBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::INDEX, RsResourceCreationFlags::STREAM,
                   NoofIndex_ * sizeof(BcU16)),
      getFullName().c_str());

  // Allocate uniform buffer object.
  pUniformBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::UNIFORM, RsResourceCreationFlags::STREAM,
                   sizeof(ScnShaderObjectUniformBlockData)),
      getFullName().c_str());

  MaterialComponent_ = Parent->attach<ScnMaterialComponent>(
      BcName::INVALID, Material_, ScnShaderPermutationFlags::MESH_STATIC_3D |
                                      ScnShaderPermutationFlags::LIGHTING_NONE);

  generateBallMesh(0.5f);

  // Upload.
  BcU32 VertexDataSize = NoofVertices_ * sizeof(GaRollingBallVertex);
  if (VertexDataSize > 0) {
    UploadFence_.increment();
    RsCore::pImpl()->updateBuffer(
        pVertexBuffer_.get(), 0, VertexDataSize, RsResourceUpdateFlags::ASYNC,
        [this, VertexDataSize](RsBuffer* Buffer,
                               const RsBufferLock& BufferLock) {
          BcAssert(VertexDataSize <= Buffer->getDesc().SizeBytes_);
          BcMemCopy(BufferLock.Buffer_, pVertices_, VertexDataSize);
          UploadFence_.decrement();
        });
  }

  size_t IndexDataSize = NoofIndex_ * sizeof(BcU16);
  if (IndexDataSize > 0) {
    UploadFence_.increment();
    RsCore::pImpl()->updateBuffer(
        pIndexBuffer_.get(), 0, IndexDataSize, RsResourceUpdateFlags::ASYNC,
        [this, IndexDataSize](RsBuffer* Buffer,
                              const RsBufferLock& BufferLock) {
          BcAssert(IndexDataSize <= Buffer->getDesc().SizeBytes_);
          BcMemCopy(BufferLock.Buffer_, pIndexes_, IndexDataSize);
          UploadFence_.decrement();
        });
  }

  RsGeometryBindingDesc GeometryBindingDesc;
  GeometryBindingDesc.setIndexBuffer(pIndexBuffer_.get());
  GeometryBindingDesc.setVertexBuffer(0, pVertexBuffer_.get(),
                                      sizeof(GaRollingBallVertex));
  GeometryBindingDesc.setVertexDeclaration(pVertexDeclaration_.get());
  GeometryBinding_ = RsCore::pImpl()->createGeometryBinding(
      GeometryBindingDesc, getFullName().c_str());
}

//////////////////////////////////////////////////////////////////////////
// onDetach
// virtual
void GaRollingBallRenderComponent::onDetach(ScnEntityWeakRef Parent) {
  getParentEntity()->detach(MaterialComponent_);

  UploadFence_.wait();
  RenderFence_.wait();

  GeometryBinding_.reset();
  pVertexDeclaration_.reset();
  pVertexBuffer_.reset();
  pIndexBuffer_.reset();
  pUniformBuffer_.reset();

  delete[] pVertices_;

  Super::onDetach(Parent);
}

void GaRollingBallRenderComponent::render(ScnRenderContext& RenderContext) {
  // Material
  {
    // Set model parameters on material.
    ObjectUniforms_.WorldTransform_ = getParentEntity()->getWorldMatrix();
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
  RenderFence_.increment();
  RenderContext.pFrame_->queueRenderNode(RenderContext.Sort_, [
    this,
    GeometryBinding = GeometryBinding_.get(),
    DrawProgramBinding = MaterialComponent_->getProgramBinding(),
    RenderState = MaterialComponent_->getRenderState(),
    FrameBuffer = RenderContext.pViewComponent_->getFrameBuffer(),
    Viewport = &RenderContext.pViewComponent_->getViewport()
  ](RsContext * Context) {

    Context->drawIndexedPrimitives(
        GeometryBinding, DrawProgramBinding, RenderState, FrameBuffer, Viewport,
        nullptr, RsTopologyType::TRIANGLE_LIST, 0, NoofIndex_, 0);

    RenderFence_.decrement();
  });
}

MaAABB GaRollingBallRenderComponent::getAABB() const {
  return MaAABB(MaVec3d(-100.0f, -100.0f, -100.0f),
                MaVec3d(100.0f, 100.0f, 100.0f));
}

void GaRollingBallRenderComponent::generateBallMesh(BcF32 radius) {
  GaRollingBallVertex* pVert = pVertices_;
  BcU16* pIdx = pIndexes_;

  BcU16 v = 0;
  BcU16 i = 0;
  MaVec3d pos = MaVec3d(0, 0, 0);
  BcU32 col = 0xFFFFFFFF;

  // Draw Top
  /*  pNorm[n++] = MaVec3d(0.0f, 0.0f, 1.0f);*/
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() - radius,
                                   pos.z() + radius, 1.0f, 0.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() - radius,
                                   pos.z() + radius, 1.0f, 1.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() + radius,
                                   pos.z() + radius, 1.0f, 1.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() + radius,
                                   pos.z() + radius, 1.0f, 0.0f, 1.0f, col};
  pIdx[i++] = v - 4;
  pIdx[i++] = v - 3;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 1;
  pIdx[i++] = v - 4;

  // Draw Bottom
  /*  pNorm[n++] = MaVec3d(0.0f, 0.0f, -1.0f);*/
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() - radius,
                                   pos.z() - radius, 1.0f, 0.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() + radius,
                                   pos.z() - radius, 1.0f, 0.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() + radius,
                                   pos.z() - radius, 1.0f, 1.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() - radius,
                                   pos.z() - radius, 1.0f, 1.0f, 0.0f, col};
  pIdx[i++] = v - 4;
  pIdx[i++] = v - 3;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 1;
  pIdx[i++] = v - 4;

  // Draw North
  /*  pNorm[n++] = MaVec3d(0.0f, 1.0f, 0.0f); */
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() + radius,
                                   pos.z() - radius, 1.0f, 0.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() + radius,
                                   pos.z() + radius, 1.0f, 0.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() + radius,
                                   pos.z() + radius, 1.0f, 1.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() + radius,
                                   pos.z() - radius, 1.0f, 1.0f, 0.0f, col};
  pIdx[i++] = v - 4;
  pIdx[i++] = v - 3;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 1;
  pIdx[i++] = v - 4;

  // Draw South
  /*  pNorm[n++] = MaVec3d(0.0f, -1.0f, 0.0f); */
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() - radius,
                                   pos.z() - radius, 1.0f, 0.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() - radius,
                                   pos.z() - radius, 1.0f, 1.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() - radius,
                                   pos.z() + radius, 1.0f, 1.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() - radius,
                                   pos.z() + radius, 1.0f, 0.0f, 1.0f, col};
  pIdx[i++] = v - 4;
  pIdx[i++] = v - 3;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 1;
  pIdx[i++] = v - 4;

  // Draw East
  /*  pNorm[n++] = MaVec3d(1.0f, 0.0f, 0.0f); */
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() - radius,
                                   pos.z() - radius, 1.0f, 0.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() + radius,
                                   pos.z() - radius, 1.0f, 1.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() + radius,
                                   pos.z() + radius, 1.0f, 1.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() + radius, pos.y() - radius,
                                   pos.z() + radius, 1.0f, 0.0f, 1.0f, col};
  pIdx[i++] = v - 4;
  pIdx[i++] = v - 3;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 1;
  pIdx[i++] = v - 4;

  // Draw West
  /*  pNorm[n++] = MaVec3d(-1.0f, 0.0f, 0.0f); */
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() - radius,
                                   pos.z() - radius, 1.0f, 0.0f, 0.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() - radius,
                                   pos.z() + radius, 1.0f, 0.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() + radius,
                                   pos.z() + radius, 1.0f, 1.0f, 1.0f, col};
  pVert[v++] = GaRollingBallVertex{pos.x() - radius, pos.y() + radius,
                                   pos.z() - radius, 1.0f, 1.0f, 0.0f, col};
  pIdx[i++] = v - 4;
  pIdx[i++] = v - 3;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 2;
  pIdx[i++] = v - 1;
  pIdx[i++] = v - 4;
}