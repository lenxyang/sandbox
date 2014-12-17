#include "azer/util/sampleapp.h"
#include "azer/base/appinit.h"
#include "azer/sandbox/base/base.h"
#include "azer/sandbox/base/camera_control.h"
#include "azer/sandbox/envmap/skybox/skybox.h"

#define CUBEMAP_PATH L"azer/sandbox/media/cubemap/snowcube1024.dds"

class MainDelegate : public SampleApp::Delegate {
 public:
  MainDelegate() {}
  virtual bool OnInit();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  azer::Camera camera_;
  std::unique_ptr<Skybox> skybox_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

bool MainDelegate::OnInit() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 5.0f));
  skybox_.reset(new Skybox(::base::FilePath(CUBEMAP_PATH), rs));
  CHECK(skybox_->Init());
  return true; 
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  azer::Radians camera_speed(azer::kPI / 2.0f);
  UpdatedownCamera(&camera_, camera_speed, delta_time);
}


void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  DCHECK(NULL != rs);
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();

  // renderer->SetFillMode(azer::kWireFrame);
  skybox_->Render(camera_, renderer);
}

int main(int argc, char* argv[]) {
  ::azer::InitApp(&argc, &argv, "");
  MainDelegate delegate;
  SampleApp app(&delegate);
  app.Init();
  app.MainLoop();
  return 0;
}
