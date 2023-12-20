#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "ewMath/vec3.h"

namespace ew {
	ew::Mesh ProcessAiMesh(aiMesh* aiMesh);

	Model::Model(const std::string& filePath)
	{
		Assimp::Importer importer;
		const aiScene* aiScene = importer.ReadFile(filePath, aiProcess_Triangulate);
		for (size_t i = 0; i < aiScene->mNumMeshes; i++)
		{
			aiMesh* aiMesh = aiScene->mMeshes[i];
			m_meshes.push_back(ProcessAiMesh(aiMesh));
		}
	}

	void Model::Draw(const ew::Shader& shader)
	{
		shader.use();
		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].Draw();
		}
	}

	ew::Vec3 AiVec3(const aiVector3D& v) {
		return ew::Vec3(v.x, v.y, v.z);
	}

	//Utility functions local to this file
	ew::Mesh ProcessAiMesh(aiMesh* aiMesh) {
		ew::MeshData meshData;
		for (size_t i = 0; i < aiMesh->mNumVertices; i++)
		{
			ew::Vertex vertex;
			vertex.pos = AiVec3(aiMesh->mVertices[i]);
			if (aiMesh->HasNormals()) {
				vertex.normal = AiVec3(aiMesh->mNormals[i]);
			}
			if (aiMesh->HasTextureCoords(0)) {
				vertex.uv = AiVec3(aiMesh->mTextureCoords[0][i]).ToVec2();
			}
			meshData.vertices.push_back(vertex);
		}
		//Convert faces to indices
		for (size_t i = 0; i < aiMesh->mNumFaces; i++)
		{
			for (size_t j = 0; j < aiMesh->mFaces[i].mNumIndices; j++)
			{
				meshData.indices.push_back(aiMesh->mFaces[i].mIndices[j]);
			}
		}
		return ew::Mesh(meshData);
	}

}
