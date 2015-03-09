#include <memory>

#include "azer/render/render.h"
#include "azer/render/ui/views.h"
#include "azer/render/ui/render_loop.h"
#include "azer/sandbox/base/mesh.h"
#include "azer/sandbox/base/camera_control.h"
#include "diffuse.afx.h"

#define MESH_PATH FILE_PATH_LITERAL("azer/sandbox/media/model/Rayguns/Raygun_01.OBJ")
#define SHADER_NAME "/diffuse.afx"
using base::FilePath;

class MyMesh : public Mesh {
 public:
  MyMesh() {}
  bool Init(MeshData* data, azer::RenderSystem* rs);
  virtual void UseEffect(const Group& group, azer::Renderer* r);
  virtual void UpdateVertex(void* desc, const MeshData::Vertex& vertex);
  virtual void UpdateAll(azer::VertexData* vdata, azer::IndicesData* idata);

  void OnUpdateScene(const azer::Camera& camera);
  void SetLight(const DirLight& l) { light_ = l;}
 private:
  scoped_refptr<DiffuseEffect> effect_;
  DirLight light_;
  Material mtrl_;
  DISALLOW_COPY_AND_ASSIGN(MyMesh);
};

class MainDelegate : public azer::RenderLoop::Delegate {
 public:
  MainDelegate() {}
  virtual bool Initialize(azer::RenderLoop* rl) override;
  virtual void OnUpdate(azer::FrameArgs* args) override;
  virtual void OnRender(azer::FrameArgs* args) override;
 private:
  azer::VertexBufferPtr vb_;
  scoped_refptr<azer::Effect> effect_;

  azer::Camera camera_;
  DirLight light_;
  std::unique_ptr<MyMesh> mesh_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

void MainDelegate::OnRender(azer::FrameArgs* args) {
  azer::RendererPtr& renderer = GetRenderer();
  renderer->EnableDepthTest(true);
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  effect_->Use(renderer.get());
  mesh_->Render(renderer);
}

void MainDelegate::OnUpdate(azer::FrameArgs* args) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  azer::Radians camera_speed(azer::kPI / 2.0f);
  UpdatedownCamera(&camera_, 50.0f, camera_speed, (float)args->delta().InSecondsF());
  mesh_->OnUpdateScene(camera_);
}

bool MainDelegate::Initialize(azer::RenderLoop* rl) {
  azer::RenderSystem* rs = rl->GetRenderSystem();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  DiffuseEffect* effect = new DiffuseEffect(shaders.GetShaderVec(), rs);
  effect_ = effect;
  
  MeshData data;
  if (!LoadMeshData(MESH_PATH, &data, rs)) {
    return false;
  }
  
  mesh_.reset(new MyMesh);
  if (!mesh_->Init(&data, rs)) {
    return false;
  }

  light_.dir = azer::Vector4(0.0f, -0.3f, 0.75f, 0.0f);
  light_.diffuse = azer::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  light_.ambient = azer::Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  mesh_->SetLight(light_);
  camera_.SetPosition(azer::Vector3(0.0f, 10.0f, 10.0f));
  camera_.SetLookAt(azer::Vector3(0.0f, 0.0f, 0.0f));
  return true;
}

bool MyMesh::Init(MeshData* data, azer::RenderSystem* rs) {
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_ = new DiffuseEffect(shaders.GetShaderVec(), rs);
  desc_ = effect_->GetVertexDesc();

  if (!InitPhyBuf(data, effect_->GetVertexDesc(), rs)) {
    return false;
  }

  mtrl_.diffuse_color = azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  mtrl_.specular_color = azer::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  mtrl_.specular_power = 32.0f;
  mtrl_.specular_intensity = 1.0f;
  return true;
}

void MyMesh::UseEffect(const Group& group, azer::Renderer* renderer) {
  effect_->Use(renderer);
}

void MyMesh::UpdateVertex(void* dest, const MeshData::Vertex& vertex) {
  DiffuseEffect::Vertex* v = (DiffuseEffect::Vertex*)dest;
  v->position = azer::Vector3(vertex.position);
  v->texcoord0= vertex.tex0;
  v->normal = azer::Vector3(vertex.normal);
}

void MyMesh::UpdateAll(azer::VertexData* vdata, azer::IndicesData* idata) {
}

void MyMesh::OnUpdateScene(const azer::Camera& camera) {
  effect_->SetWorld(azer::Matrix4::kIdentity);
  effect_->SetDirLight(light_);
  effect_->SetMaterial(mtrl_);
  effect_->SetPVW(camera.GetProjViewMatrix());
  effect_->SetCameraPos(azer::Vector4(camera.position(), 1.0f));
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

