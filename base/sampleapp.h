#pragma once

#include <memory>
#include "azer/sandbox/window/window_host.h"
#include "base/time/time.h"

namespace azer {
class SwapChain;
class Surface;
class RenderSystem;
}  // namespace azer

class SampleApp : public azer::WindowHost {
 public:
  class Delegate {
   public:
    Delegate() : app_(NULL) {}
    void SetApp(SampleApp* app) { app_ = app;}
    virtual bool OnInit() = 0;
    virtual void OnUpdateScene(double time, float delta_time) = 0;
    virtual void OnRenderScene(double time, float delta_time) = 0;
    virtual void OnQuit() = 0;
   protected:
    SampleApp* app_;
  };

  SampleApp(const Options& opt, Delegate* delegate);
  SampleApp(Delegate* delegate);
  azer::RenderSystem* GetRS() { return render_system_;}

  azer::SwapChain* GetSwapChain() { return swapchain_.get();}
  void MainLoop();
 protected:
  virtual bool OnInit();
  virtual void OnQuit();
  virtual int OnIdle(azer::window::NativeIdleEvent* message);
  Delegate* delegate_;

  std::unique_ptr<azer::Surface> surface_;
  std::unique_ptr<azer::SwapChain> swapchain_;
  ::base::TimeDelta total_ticking_;
  base::TimeTicks prev_frame_;
  azer::RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(SampleApp);
};
