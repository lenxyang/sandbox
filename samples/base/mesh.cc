#include "sandbox/base/mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/anim.h"

#include "azer/render/render.h"

void LoadVertex(const aiMesh* paiMesh, MeshData::Group* group) {
  const aiVector3D zero3d(0.0f, 0.0f, 0.0f);
  for (uint32 i = 0; i < paiMesh->mNumVertices; ++i) {
    const aiVector3D& pos = paiMesh->mVertices[i];
    const aiVector3D& normal = paiMesh->mNormals[i];
    const aiVector3D& texcoord =
        paiMesh->HasTextureCoords(0) ? (paiMesh->mTextureCoords[0][i]) : zero3d;

    MeshData::Vertex vertex(azer::Vector4(pos.x, pos.y, pos.z, 1.0),
                            azer::Vector2(texcoord.x, texcoord.y),
                            azer::Vector4(normal.x, normal.y, normal.z, 0.0));
    group->vertices.push_back(vertex);
  }

  for (uint32 i = 0; i < paiMesh->mNumFaces; ++i) {
    const aiFace& face = paiMesh->mFaces[i];
    group->indices.push_back(face.mIndices[0]);
    group->indices.push_back(face.mIndices[1]);
    group->indices.push_back(face.mIndices[2]);
  }

  group->mtrl_idx = paiMesh->mMaterialIndex;
}

bool LoadMeshData(const ::base::FilePath& filepath, MeshData* mesh,
                  azer::RenderSystem* rs) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      ::base::WideToUTF8(filepath.value()),
      aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
  if (scene == NULL) {
    LOG(ERROR) << "Failed to load file: " << filepath.value();
    return false;
  }

  for (uint32 i = 0; i < scene->mNumMeshes; ++i) {
    MeshData::Group group;
    LoadVertex(scene->mMeshes[i], &group);
    mesh->mutable_groups()->push_back(group);
  }

  for (uint32 i = 0; i < scene->mNumMaterials; ++i) {
    const aiMaterial* material = scene->mMaterials[i];
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString path;
      if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path,
                               NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
        ::base::FilePath texpath(::base::UTF8ToWide(path.C_Str()));
        ::base::FilePath realpath;
        realpath = filepath.DirName();
        realpath = realpath.Append(texpath.BaseName());
        azer::TexturePtr texptr(azer::Texture::LoadShaderTexture(realpath, rs));
        MeshData::Material mtrl;
        mtrl.tex = texptr;
        mesh->mutable_materials()->push_back(mtrl);
      }
    }
  }

  return true;
}

bool Mesh::InitPhyBuf(const MeshData* mesh, azer::VertexDescPtr& desc,
                      azer::RenderSystem* rs) {
  for (size_t i = 0; i < mesh->groups().size(); ++i) {
    const MeshData::Group& dgroup = mesh->groups()[i];
    azer::VertexData vdata(desc, dgroup.vertices.size());
    uint8* ptr = vdata.pointer();
    for (int j = 0; j < dgroup.vertices.size(); ++j) {
      UpdateVertex((void*)ptr, dgroup.vertices[j]);
      ptr += desc->stride();
    }

    azer::IndicesData idata(dgroup.indices.size(), azer::IndicesData::kUint32);
    memcpy(idata.pointer(), &(dgroup.indices[0]),
           sizeof(uint32) * dgroup.indices.size());

    Group group;
    group.vb.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), &vdata));
    group.ib.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), &idata));
    group.mtrl_index = dgroup.mtrl_idx;
    groups_.push_back(group);
  }

  for (size_t i = 0; i < mesh->materials().size(); ++i) {
    mtrl_.push_back(mesh->materials()[i]);
  }
  
  return true;
}

void Mesh::Render(azer::Renderer* renderer) {
  for (auto iter = groups_.begin(); iter != groups_.end(); ++iter) {
    UseEffect(*iter, renderer);
    renderer->DrawIndex(iter->vb.get(), iter->ib.get(), azer::kTriangleList);
  }
}
