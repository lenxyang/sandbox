#include "azer/sandbox/envmap/skybox.h"

#define CUBEMAP_PATH L"azer/sandbox/media/cubemap/snowcube1024.dds"

Skybox::Skybox(const ::base::FilePath& texpath, azer::RenderSystem* rs) 
    : texpath_(texpath)
    , render_system_(rs) {
}

bool Skybox::Init() {
  cubemap_.reset(azer::Texture::Load(azer::Texture::kCubemap, texpath_, 
                                     render_system_));
  if (!cubemap_.get()) {
    return false;
  }

  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_.reset(new DiffuseEffect(shaders.GetShaderVec(), rs));
  desc_ = effect_->GetVertexDesc();

  const int kVertexNum = 8;
  VertexData data(effect_->GetVertexDesc(), kVertexNum);
  Vector4* begin_vptr = (Vector4*)data.pointer();
  Vector4* vptr = begin_vptr;
  *vptr++ = azer::Vector4(-0.5f,  0.5f,  0.5f, 1.0f);
  *vptr++ = azer::Vector4(-0.5f,  0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f,  0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f,  0.5f,  0.5f, 1.0f);

  *vptr++ = azer::Vector4(-0.5f, -0.5f,  0.5f, 1.0f);
  *vptr++ = azer::Vector4(-0.5f, -0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f, -0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f, -0.5f,  0.5f, 1.0f);
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), &data));

  const int kTriangleNum = 12;
  IndicesData idata(kTriangleNum * 3, IndicesData::kUint32);
  int32 indices[kTriangleNum * 3] = {
    0,3,1, 1,3,2,  // top
    4,5,7, 7,5,6,  // bottom
    1,2,5, 5,2,6,  // front
    0,4,3, 3,4,7,  // back
    0,1,4, 4,1,5,  // left
    2,3,7, 2,7,6,  // right
  };
  int32* cur = (int32*)idata.pointer();
  memcpy(cur, indices, sizeof(int32) * kTriangleNum * 3);
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), &idata));
  return true;
}

void Skybox::Render(azer::Renderer* renderer) {
  effect_->SetCubemap(cubemap_);
  effect_->Use(renderer);
  renderer->DrawIndex();
}
