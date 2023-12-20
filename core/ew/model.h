#pragma once
#include "mesh.h"
#include "shader.h"
#include <vector>

namespace ew {
	class Model {
	public:
		Model(const std::string& filePath);
		void Draw(const ew::Shader& shader);
	private:
		std::vector<ew::Mesh> m_meshes;
	};
}