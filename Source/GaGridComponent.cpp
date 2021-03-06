#include "GaGridComponent.h"

#include "System/Scene/Rendering/ScnViewComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED(GaGridComponent);

void GaGridComponent::StaticRegisterClass() {
  ReField* Fields[] = {
      new ReField("GridSize_", &GaGridComponent::GridSize_, bcRFF_IMPORTER),
      new ReField("Material_", &GaGridComponent::Material_,
                  bcRFF_SHALLOW_COPY | bcRFF_IMPORTER),
      new ReField("MaterialComponent_", &GaGridComponent::MaterialComponent_,
                  bcRFF_TRANSIENT),
  };

  ReRegisterClass<GaGridComponent, Super>(Fields)
      .addAttribute(new ScnComponentProcessor({
          // ScnComponentProcessFuncEntry::PreUpdate< GaGridComponent >()
      }));
}

GaGridComponent::GaGridComponent() {}

GaGridComponent::~GaGridComponent() {}

void GaGridComponent::render(ScnRenderContext& RenderContext) {
  // AABB
  MaMat4d mat = ParentEntity_->getWorldMatrix();
  MaVec4d a = mat.row3();
  BcF32 scale = 10000.0f;

  MaVec3d minZZ = MaVec3d(a.x() - scale, a.y() - scale, a.z() - scale);

  MaVec3d maxZZ = MaVec3d(a.x() + scale, a.y() + scale, a.z() + scale);

  AABB_ = MaAABB(minZZ, maxZZ);

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

    Context->drawPrimitives(GeometryBinding, DrawProgramBinding, RenderState,
                            FrameBuffer, Viewport, nullptr,
                            RsTopologyType::LINE_LIST, 0, NoofVertices_);

    RenderFence_.decrement();
  });
}

MaAABB GaGridComponent::getAABB() const { return AABB_; }

void GaGridComponent::onAttach(ScnEntityWeakRef Parent) {
  BcAssert(Material_ != nullptr);
  BcAssert(NoofGridLines_ >= 0);

  Super::onAttach(Parent);

  // Allocate our own vertex buffer data.
  NoofVertices_ = NoofGridLines_ * 4;
  pVertices_ = new GaGridComponentVertex[NoofVertices_];

  // Setup Streams
  pVertexDeclaration_ = RsCore::pImpl()->createVertexDeclaration(
      RsVertexDeclarationDesc(2)
          .addElement(RsVertexElement(0, 0, 4, RsVertexDataType::FLOAT32,
                                      RsVertexUsage::POSITION, 0))
          .addElement(RsVertexElement(0, 16, 4, RsVertexDataType::UBYTE_NORM,
                                      RsVertexUsage::COLOUR, 0)),
      getFullName().c_str());

  // Allocate render side vertex buffer.
  pVertexBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::VERTEX, RsResourceCreationFlags::STREAM,
                   NoofVertices_ * sizeof(GaGridComponentVertex)),
      getFullName().c_str());

  // Allocate uniform buffer object.
  pUniformBuffer_ = RsCore::pImpl()->createBuffer(
      RsBufferDesc(RsBufferType::UNIFORM, RsResourceCreationFlags::STREAM,
                   sizeof(ScnShaderObjectUniformBlockData)),
      getFullName().c_str());

  MaterialComponent_ = Parent->attach<ScnMaterialComponent>(
      BcName::INVALID, Material_, ScnShaderPermutationFlags::MESH_STATIC_3D |
                                      ScnShaderPermutationFlags::LIGHTING_NONE);

  generateGrid();

  // Upload.
  BcU32 VertexDataSize = NoofVertices_ * sizeof(GaGridComponentVertex);
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

  RsGeometryBindingDesc GeometryBindingDesc;
  GeometryBindingDesc.setVertexBuffer(0, pVertexBuffer_.get(),
                                      sizeof(GaGridComponentVertex));
  GeometryBindingDesc.setVertexDeclaration(pVertexDeclaration_.get());
  GeometryBinding_ = RsCore::pImpl()->createGeometryBinding(
      GeometryBindingDesc, getFullName().c_str());
}

void GaGridComponent::generateGrid() {
  UploadFence_.wait();

  // Generate Grid
  BcF32 minPoint = (NoofGridLines_ * 0.5f) * -GridSize_;
  BcF32 maxPoint = (NoofGridLines_ * 0.5f + 1) * +GridSize_;
  auto pCurr = pVertices_;

  BcF32 x = minPoint;
  for (BcU32 iX = 0; iX < NoofGridLines_; ++iX) {
    BcU32 col = 0xFFFFFFFF;

    x += GridSize_;
    *pCurr = GaGridComponentVertex{x, 0, minPoint + GridSize_, 1.0f, col};
    ++pCurr;
    *pCurr = GaGridComponentVertex{x, 0, maxPoint - GridSize_, 1.0f, col};
    ++pCurr;
  }

  BcF32 y = minPoint;
  for (BcU32 iY = 0; iY < NoofGridLines_; ++iY) {
    BcU32 col = 0xFFFFFFFF;

    y += GridSize_;
    *pCurr = GaGridComponentVertex{minPoint + GridSize_, 0, y, 1.0f, col};
    ++pCurr;
    *pCurr = GaGridComponentVertex{maxPoint - GridSize_, 0, y, 1.0f, col};
    ++pCurr;
  }
}

void GaGridComponent::onDetach(ScnEntityWeakRef Parent) {
  getParentEntity()->detach(MaterialComponent_);

  UploadFence_.wait();
  RenderFence_.wait();

  GeometryBinding_.reset();
  pVertexDeclaration_.reset();
  pVertexBuffer_.reset();
  pUniformBuffer_.reset();

  delete[] pVertices_;

  Super::onDetach(Parent);
}
