#include "azer/sandbox/envmap/skybox/skybox.h"

#include "skybox.afx.h"
#define SKYBOX_SHADER_NAME "/skybox.afx"

using azer::Vector4;
using azer::VertexData;
using azer::IndicesData;

Skybox::Skybox(const ::base::FilePath& texpath, azer::RenderSystem* rs) 
    : texpath_(texpath)
    , render_system_(rs) {
}

bool Skybox::Init() {
  azer::RenderSystem* rs = render_system_;
  cubemap_.reset(azer::Texture::Load(azer::Texture::kCubemap, texpath_, rs));
  if (!cubemap_.get()) {
    return false;
  }

  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SKYBOX_SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SKYBOX_SHADER_NAME ".ps", &shaders));
  effect_.reset(new SkyboxEffect(shaders.GetShaderVec(), rs));
  azer::VertexDescPtr desc = effect_->GetVertexDesc();

  const int kVertexNum = 8;
  VertexData data(desc, kVertexNum);
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

void Skybox::Render(const azer::Camera& camera, azer::Renderer* renderer) {
  azer::Matrix4 world = std::move(azer::Translate(camera.position())
                                  * azer::Scale(100.0f, 100.0f, 100.0));
  azer::FrontFace backup_frontface = renderer->GetFrontFace();
  renderer->SetFrontFace(azer::kClockwise);
  effect_->SetPVW(std::move(camera.GetProjViewMatrix() * world));
  effect_->SetWorld(world);
  effect_->SetCubemap(cubemap_);
  effect_->Use(renderer);
  renderer->DrawIndex(vb_.get(), ib_.get(), azer::kTriangleList);
  renderer->SetFrontFace(backup_frontface);
}
