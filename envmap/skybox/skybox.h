#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/render/render.h"

#include "skybox.afx.h"

class Skybox {
 public:
  Skybox(const ::base::FilePath& texpath, azer::RenderSystem* rs);

  bool Init();
  void Render(const azer::Camera& camera, azer::Renderer* renderer);
 private:
  ::base::FilePath texpath_;
  std::unique_ptr<SkyboxEffect> effect_;
  azer::TexturePtr cubemap_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  azer::RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(Skybox);
};
