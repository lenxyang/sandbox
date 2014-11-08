#include "azer/util/sampleapp.h"
#include "diffuse.afx.h"
#include "azer/base/appinit.h"
#include "azer/ui/widget/all.h"
#include "base/files/file_path.h"

#define SHADER_NAME "diffuse.afx"
using base::FilePath;

class MainDelegate : public SampleApp::Delegate {
 public:
  virtual bool OnInit();
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    context_->Draw();
    renderer->Use();
    renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    renderer->ClearDepthAndStencil();
    effect_->Use(renderer);
    renderer->Draw(vb_.get(), azer::kTriangleList, 3, 0);

    context_->Render(renderer);
  }
  virtual void OnQuit() {}
  void InitUI(azer::RenderSystem* rs);
 private:
  azer::VertexBufferPtr vb_;
  std::unique_ptr<DiffuseEffect> effect_;
  std::unique_ptr<azer::ui::Context> context_;
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

  InitUI(rs);
  return true;
}

void MainDelegate::InitUI(azer::RenderSystem* rs) {
  static azer::ui::DefaultTheme dtheme;
  dtheme.Init();
  azer::ui::Theme::SetTheme(&dtheme);
  context_.reset(azer::ui::Context::Create(rs));
  azer::ui::WidgetHost* root = context_->GetWidgetHost();
  azer::ui::Label* label = new azer::ui::Label(gfx::Rect(200, 400, 300, 30), root);
  dtheme.control_group()->AddItem(label);
  label->SetText(AZER_LITERAL("hello, this is label"));
}

int main(int argc, char* argv[]) {
  ::azer::InitApp(&argc, &argv, "");

  MainDelegate delegate;
  SampleApp app(&delegate);
  app.Init();
  app.MainLoop();

  return 0;
}
