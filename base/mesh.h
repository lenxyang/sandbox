#pragma once

#include <vector>
#include "azer/render/render.h"
#include "base/files/file_path.h"

class MeshData {
 public:
  MeshData() {}
  struct Vertex {
    azer::Vector4 position;
    azer::Vector2 tex0;
    azer::Vector4 normal;
    Vertex(const azer::Vector4 p0, const azer::Vector2 p1, const azer::Vector4 p2)
      : position(p0)
      , tex0(p1)
      , normal(p2)
      {}
  };

  struct Material {
    azer::TexturePtr tex;
    azer::TexturePtr bumpmap;
  };

  struct Group {
    std::vector<Vertex> vertices;
    std::vector<int32> indices;
    int mtrl_idx;
  };

  const std::vector<Group>& groups() const { return groups_;}
  std::vector<Group>* mutable_groups() { return &groups_;}

  const std::vector<Material>& materials() const { return materials_;}
  std::vector<Material>* mutable_materials() { return &materials_;}
 private:
  std::vector<Group> groups_;
  std::vector<Material> materials_;

  DISALLOW_COPY_AND_ASSIGN(MeshData);
};

class Mesh {
 public:
  Mesh() {}
  struct Group {
    azer::VertexBufferPtr vb;
    azer::IndicesBufferPtr ib;
    int mtrl_index;
  };

  const std::vector<Group>& groups() const { return groups_;}
  std::vector<Group>* mutable_groups() { return &groups_;}
  
  virtual void UpdateVertex(void* desc, const MeshData::Vertex& vertex) = 0;
  virtual void UpdateAll(azer::VertexData* vdata, azer::IndicesData* idata) = 0;

  virtual void UseEffect(const Group& group, azer::Renderer* r) = 0;
  
  void Render(azer::Renderer* renderer);
 protected:
  bool InitPhyBuf(const MeshData* mesh, azer::VertexDescPtr& desc,
                  azer::RenderSystem* rs);
  std::vector<Group> groups_;
  std::vector<MeshData::Material> mtrl_;
  azer::VertexDescPtr desc_;
  DISALLOW_COPY_AND_ASSIGN(Mesh);
};

bool LoadMeshData(const ::base::FilePath& filepath, MeshData* mesh,
                  azer::RenderSystem* rs);
inline bool LoadMeshData(const ::base::FilePath::StringType& filepath,
                         MeshData* mesh, azer::RenderSystem* rs) {
  return LoadMeshData(::base::FilePath(filepath), mesh, rs);
}
