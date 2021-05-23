#pragma once
#include "Mesh.h"
#include "Skeleton.h"
class Output
{
private:
	std::string m_path;
public:
	Output();
	~Output();
	void setOutPath(char* path);
	void writeMesh(const std::vector<X::MESH>& meshes,const std::vector<std::vector<X::VERTEX>>& vertices, const std::vector<std::vector<unsigned int>>& indices);
	void writeBlendShapes(std::vector<X::MORPH_TARGET>& targets, std::vector<std::vector<X::MORPH_VERTEX>>& vertices,
		std::vector<std::vector<X::MORPH_INDEX>>& indices,std::vector<std::vector<X::MORPH_KEYFRAME>>& keyframes, float length, float fps);
	void writeSkeleton(Skeleton* skeleton);
	void writeMaterial(const std::vector<X::MATERIAL>& materials);
	void writeLight(const std::vector<X::LIGHT>& lights);
};

