#include "azer/sandbox/base/sampleapp.h"
#include "azer/base/appinit.h"
#include "base/at_exit.h"
#include "diffuse.afx.h"

#define SHADER_NAME "/diffuse.afx"
using base::FilePath;

class MainDelegate : public SampleApp::Delegate {
 public:
  MainDelegate() {}
  virtual bool OnInit();
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  azer::VertexBufferPtr vb_;
  azer::OverlayPtr overlay_;
  azer::RendererPtr texrenderer_;
  std::unique_ptr<DiffuseEffect> effect_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

bool MainDelegate::OnInit() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_.reset(new DiffuseEffect(shaders.GetShaderVec(), rs));
  DiffuseEffect::Vertex v[] = {
    DiffuseEffect::Vertex(azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 1.0f, 0.0f, 0.0f, 1.0f )),
    DiffuseEffect::Vertex(azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 0.0f, 1.0f,  0.0f, 1.0f )),
    DiffuseEffect::Vertex(azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 0.0f, 0.0f, 1.0f, 1.0f )),
  };
  azer::VertexData vdata(effect_->GetVertexDesc(), ARRAYSIZE(v));
  memcpy(vdata.pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), &vdata));

  overlay_.reset(rs->CreateOverlay(
      gfx::RectF(-1.0f, 1.0f - 0.25f, 0.25f, 0.25f)));

  azer::Texture::Options rdopt;
  rdopt.width = 800;
  rdopt.height = 600;
  rdopt.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  texrenderer_.reset(rs->CreateRenderer(rdopt));
  return true;
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = texrenderer_.get();
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  effect_->Use(renderer);
  renderer->Draw(vb_.get(), azer::kTriangleList, 3, 0);


  renderer = rs->GetDefaultRenderer();
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  overlay_->SetTexture(texrenderer_->GetRenderTarget()->GetTexture());
  overlay_->EnableBlending(false);
  overlay_->Render(renderer);

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
