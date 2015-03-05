#pragma once

#include "azer/render/render.h"
#include "azer/render/ui/render_loop.h"
#include "azer/render/ui/uienv.h"

class MainDelegate : public azer::RenderLoop::Delegate {
 public:
  MainDelegate() {}
  virtual bool Initialize(azer::RenderLoop* rl) override;
  virtual void OnUpdate(const ::base::Time& Time,
                        const ::base::TimeDelta& delta) override {}
  virtual void OnRender(const ::base::Time& Time,
                        const ::base::TimeDelta& delta) override;
 private:
  azer::VertexBufferPtr vb_;
  std::unique_ptr<azer::Effect> effect_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};
