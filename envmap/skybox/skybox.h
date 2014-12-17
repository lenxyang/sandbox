#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/render/render.h"

#include "skybox.afx.h"

class Skybox {
 public:
  Skybox(const ::base::FilePath& texpath, azer::RenderSystem* rs);

  bool Init();
  void Render(azer::Renderer* renderer);
 private:
  std::unique_ptr<SkyboxEffect> effect_;
  azer::TexturePtr cubemap_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  DISALLOW_COPY_AND_ASSIGN(Skybox);
};
