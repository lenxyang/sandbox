#pragma once

#include "azer/render/render.h"
#include "azer/render/ui/render_loop.h"
#include "azer/render/ui/uienv.h"

class MainDelegate : public azer::RenderLoop::Delegate {
 public:
  MainDelegate() {}
  virtual bool Initialize(azer::RenderLoop* rl) override;
  virtual void OnUpdate(azer::FrameArgs* args) override {}
  virtual void OnRenderc(azer::FrameArgs* args) override;
 private:
  azer::VertexBufferPtr vb_;
  std::unique_ptr<azer::Effect> effect_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};
