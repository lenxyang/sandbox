#include <memory>

#include "azer/render/render.h"
#include "azer/render/ui/views.h"
#include "azer/render/ui/render_loop.h"
#include "diffuse.afx.h"

#define SHADER_NAME "/diffuse.afx"
using base::FilePath;

class MainDelegate : public azer::RenderLoop::Delegate {
 public:
  MainDelegate() {}
  virtual bool Initialize(azer::RenderLoop* rl) override;
  virtual void OnUpdate(azer::FrameArgs* args) override {}
  virtual void OnRender(azer::FrameArgs* args) override;
 private:
  azer::VertexBufferPtr vb_;
  scoped_refptr<azer::Effect> effect_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

void MainDelegate::OnRender(azer::FrameArgs* args) {
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
  effect_ = effect;
  DiffuseEffect::Vertex v[] = {
    DiffuseEffect::Vertex(azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 1.0f, 0.0f, 0.0f, 1.0f )),
    DiffuseEffect::Vertex(azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 0.0f, 1.0f,  0.0f, 1.0f )),
    DiffuseEffect::Vertex(azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
                          azer::Vector4( 0.0f, 0.0f, 1.0f, 1.0f )),
  };
  azer::VertexDataPtr vdata(new azer::VertexData(effect->GetVertexDesc(), ARRAYSIZE(v)));
  memcpy(vdata->pointer(), (uint8*)v, sizeof(v));
  vb_ = rs->CreateVertexBuffer(azer::VertexBuffer::Options(), vdata.get());

  return true;
}


using base::ASCIIToUTF16;

class MainWindowContent : public views::WidgetDelegateView {
 public:
  MainWindowContent(azer::UIEnvironment* env)
      : env_(env) {
    set_background(views::Background::CreateSolidBackground(0x00000000));
    // set_background(views::Background::CreateStandardPanelBackground());
  }

 private:
  bool CanResize() const override { return true; }
  bool CanMaximize() const override { return false; }
  bool CanMinimize() const override { return false; }
  base::string16 GetWindowTitle() const override {
    return base::ASCIIToUTF16("Hello Views");
  }
  View* GetContentsView() override { return this; }
  void WindowClosing() override {
    base::MessageLoopForUI::current()->Quit();
    env_->GetRenderLoop()->Quit();
  }
 private:
  azer::UIEnvironment* env_;
  DISALLOW_COPY_AND_ASSIGN(MainWindowContent);
};

int main(int argc, char** argv) {
  azer::UIEnvironment env;
  CHECK(env.Init(argc, argv));

  MainDelegate azer_content;
  azer::UIEnvironment::Params params;
  params.width = 800;
  params.height = 600;
  params.view_delegate = new MainWindowContent(&env);
  params.render_delegate = &azer_content;
  CHECK(env.MainLoop(params));
  
  return 0;
}

