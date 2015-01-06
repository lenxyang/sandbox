#include "azer/sandbox/base/sampleapp.h"
#include "azer/base/appinit.h"
#include "azer/base/image.h"
#include "rotate.afx.h"

#define SHADER_NAME "rotate.afx"
#define TEXPATH AZER_LITERAL("azer/sandbox/media/textures/seafloor.dds")
using base::FilePath;

class MainDelegate : public SampleApp::Delegate {
 public:
  MainDelegate() {}
  virtual bool OnInit();
  void OnUpdateScene(double time, float delta_time) override;
  void OnRenderScene(double time, float delta_time) override;
  virtual void OnQuit() {}
 private:
  azer::VertexBufferPtr vb_;
  std::unique_ptr<RotateEffect> effect_;
  azer::TexturePtr tex_;
  azer::Matrix4 world_;
  azer::Camera camera_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

bool MainDelegate::OnInit() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_.reset(new RotateEffect(shaders.GetShaderVec(), rs));
  RotateEffect::Vertex v[] = {
    RotateEffect::Vertex(azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
                          azer::Vector2( 0.5f, 0.0f)),
    RotateEffect::Vertex(azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector2( 0.0f, 1.0f)),
    RotateEffect::Vertex(azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector2( 1.0f, 1.0f)),
  };
  azer::VertexData vdata(effect_->GetVertexDesc(), ARRAYSIZE(v));
  memcpy(vdata.pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), &vdata));
  tex_.reset(azer::Texture::Load(azer::Texture::k2D, TEXPATH, rs));

  camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 5.0f));
  return true;
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  world_ = azer::RotateY(azer::Degree(time / 300.0f));
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::Matrix4 pvw = std::move(camera_.GetProjViewMatrix() * world_);
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  effect_->SetPVW(pvw);
  effect_->SetTexture(tex_);
  effect_->Use(renderer);
  renderer->Draw(vb_.get(), azer::kTriangleList, 3, 0);
}

int main(int argc, char* argv[]) {
  ::azer::InitApp(&argc, &argv, "");

  MainDelegate delegate;
  SampleApp app(&delegate);
  app.Init();

  app.MainLoop();

  return 0;
}
