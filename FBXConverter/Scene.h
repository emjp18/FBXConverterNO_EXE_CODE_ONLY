#pragma once
#include "Output.h"
class Scene
{
private:
	FbxManager* m_sdkManager = nullptr;
	FbxScene* m_scene = nullptr;
	void getNodes(FbxNode* node);
	SelectedOptions m_options;
	Mesh* m_mesh = nullptr;
	Skeleton* m_skeleton = nullptr;
	std::vector<X::MESH> m_meshes;
	std::vector<std::vector<X::VERTEX>> m_vertices;
	std::vector<std::vector<unsigned int>> m_indices;
	char* m_outPath;
	std::vector<X::VERTEX> m_vertexVector;
	std::vector<unsigned int> m_indexVector;
	std::vector<X::MORPH_TARGET> m_targets;
	std::vector<std::vector<X::MORPH_VERTEX>> m_morphVertices;
	std::vector<std::vector<X::MORPH_INDEX>> m_morphIndices;
	std::vector<std::vector<X::MORPH_KEYFRAME>> m_morphKeyframes; 
	std::vector<X::MATERIAL> m_materials;
	std::vector<X::LIGHT> m_lights;
	float m_animationlength;
	float m_fps;
	void getMaterial();
	void getLight(FbxNode* lightNode);
public:
	Scene(const FbxString& path, float fps);
	~Scene();
	FbxScene* getScene();
	void getRootNode();
	void setOptions(const SelectedOptions& options) { m_options = options; }
	void setOutPath(char* path);
	void write();
	
};

