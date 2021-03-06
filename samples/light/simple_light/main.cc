
#include "azer/base/appinit.h"
#include "azer/util/sampleapp.h"
#include "azer/sandbox/base/camera_control.h"
#include "azer/sandbox/base/mesh.h"

#include "diffuse.afx.h"
#define MESH_PATH FILE_PATH_LITERAL("azer/sandbox/media/model/Rayguns/Raygun_01.OBJ")
#define SHADER_NAME "diffuse.afx"

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
  std::unique_ptr<DiffuseEffect> effect_;
  DirLight light_;
  Material mtrl_;
  DISALLOW_COPY_AND_ASSIGN(MyMesh);
};

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate() {}
  virtual void OnCreate() override {}
  virtual void OnUpdateScene(double time, float delta_time) override;
  virtual void OnRenderScene(double time, float delta_time) override;
  virtual void OnQuit() override {}
  bool Init();
 private:
  azer::Camera camera_;
  DirLight light_;
  std::unique_ptr<MyMesh> mesh_;
  DISALLOW_COPY_AND_ASSIGN(MainDelegate);
};

bool MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();

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
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  renderer->EnableDepthTest(true);
  return true; 
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  azer::Radians camera_speed(azer::kPI / 2.0f);
  UpdatedownCamera(&camera_, 50.0f, camera_speed, delta_time);
  mesh_->OnUpdateScene(camera_);
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(azer::kCullNone);

  mesh_->Render(renderer);
}

bool MyMesh::Init(MeshData* data, azer::RenderSystem* rs) {
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_.reset(new DiffuseEffect(shaders.GetShaderVec(), rs));
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

int main(int argc, char* argv[]) {
  ::azer::InitApp(&argc, &argv, "");
  
  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::LoadRenderSystem(&win));
  CHECK(delegate.Init());
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  win.Show();
  azer::MainRenderLoop(&win);
  return 0;
}
