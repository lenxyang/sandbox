
#include "azer/sandbox/base/base.h"
#include "azer/sandbox/base/camera_control.h"

class MainDelegate : public SampleApp::Delegate {
 public:
  MainDelegate() {}
  virtual bool OnInit();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  azer::Camera camera_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

bool MainDelegate::OnInit() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  return true; 
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  azer::Radians camera_speed(azer::kPI / 2.0f);
  mesh_->OnUpdateScene(camera_);
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
}

int main(int argc, char* argv[]) {
  ::azer::InitApp(&argc, &argv, "");
  MainDelegate delegate;
  SampleApp app(&delegate);
  app.Init();
  app.MainLoop();
  return 0;
}
