#include "azer/util/sampleapp.h"
#include "azer/base/appinit.h"
#include "base/at_exit.h"
#include "diffuse.afx.h"

#define SHADER_NAME "/diffuse.afx"
using base::FilePath;

class MainDelegate : public SampleApp::Delegate {
 public:
  virtual bool OnInit();
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    renderer->Use();
    renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    renderer->ClearDepthAndStencil();
    effect_->Use(renderer);
    renderer->Draw(vb_.get(), azer::kTriangleList, 3, 0);
  }
  virtual void OnQuit() {}
 private:
  azer::VertexBufferPtr vb_;
  std::unique_ptr<DiffuseEffect> effect_;
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
  return true;
}

int main(int argc, char* argv[]) {
  ::azer::InitApp(&argc, &argv, "");

  MainDelegate delegate;
  SampleApp app(&delegate);
  app.Init();

  app.MainLoop();

  return 0;
}
