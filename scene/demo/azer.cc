#include "azer/uisbox/azerui/controls/azer.h"
#include "diffuse.afx.h"

#define SHADER_NAME "/diffuse.afx"
using base::FilePath;


void MainDelegate::OnRender(const ::base::Time& Time,
                            const ::base::TimeDelta& delta) {
  azer::RendererPtr& renderer = GetRenderer();
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  effect_->Use(renderer.get());
  renderer->Draw(vb_.get(), azer::kTriangleList, 3, 0);
}

bool MainDelegate::Initialize(azer::RenderLoop* rl) {
  azer::RenderSystem* rs = rl->GetRenderSystem();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  DiffuseEffect* effect = new DiffuseEffect(shaders.GetShaderVec(), rs);
  effect_.reset(effect);
  DiffuseEffect::Vertex v[] = {
    DiffuseEffect::Vertex(azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 1.0f, 0.0f, 0.0f, 1.0f )),
    DiffuseEffect::Vertex(azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 0.0f, 1.0f,  0.0f, 1.0f )),
    DiffuseEffect::Vertex(azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 0.0f, 0.0f, 1.0f, 1.0f )),
  };
  azer::VertexData vdata(effect->GetVertexDesc(), ARRAYSIZE(v));
  memcpy(vdata.pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), &vdata));

  return true;
}
