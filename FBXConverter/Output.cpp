#include "Output.h"

Output::Output()
{
}

Output::~Output()
{
}

void Output::setOutPath(char* path)
{
	m_path = path;
}



void Output::writeMesh(const std::vector<X::MESH>& meshes,const std::vector<std::vector<X::VERTEX>>& vertices, const std::vector<std::vector<unsigned int>>& indices)
{
	std::ofstream X;
	X.open(m_path + "MESH" + ".x", std::ios::out | std::ios::binary);
	assert(X.is_open());
	X::HEADER_TYPE type;
	type.type = X::HEADER::SCENE;
	X::SCENE scene;
	scene.meshCount = (unsigned short)meshes.size();
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.write((const char*)&scene, sizeof(X::SCENE));
	type.type = X::HEADER::MESH;
	for (int i = 0; i < meshes.size(); i++)
	{
		X.write((const char*)&type, sizeof(X::HEADER_TYPE));

		X.write((const char*)&meshes[i], sizeof(X::MESH));

		for (int j = 0; j < vertices[i].size(); j++)
		{
			X.write((const char*)&vertices[i][j], sizeof(X::VERTEX));
		}

		for (int j = 0; j < indices[i].size(); j++)
		{
			X.write((const char*)&indices[i][j], sizeof(unsigned int));
		}


	}
	type.type = X::HEADER::DEFAULT;

	X.write((const char*)&type, sizeof(X::HEADER_TYPE));

	X.close();
}

void Output::writeBlendShapes(std::vector<X::MORPH_TARGET>& targets, std::vector<std::vector<X::MORPH_VERTEX>>& vertices,
	std::vector<std::vector<X::MORPH_INDEX>>& indices, std::vector<std::vector<X::MORPH_KEYFRAME>>& keyframes, float length, float fps)
{
	std::ofstream X;
	X.open(m_path + "MORPH" + ".x", std::ios::out | std::ios::binary);
	assert(X.is_open());
	X::HEADER_TYPE type;
	type.type = X::HEADER::SCENE;
	X::SCENE scene;
	scene.fps = fps;
	scene.animationLenght = length;
	scene.morphTargetsCount = (unsigned short)targets.size();
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.write((const char*)&scene, sizeof(X::SCENE));
	for (int m = 0; m < (int)targets.size(); m++)
	{
		X.write((const char*)&targets[m], sizeof(X::MORPH_TARGET));
		for (int v = 0; v < (int)vertices[m].size(); v++)
		{
			X.write((const char*)&vertices[m][v], sizeof(X::MORPH_VERTEX));
		}
		for (int i = 0; i < (int)indices[m].size(); i++)
		{
			X.write((const char*)&indices[m][i], sizeof(X::MORPH_INDEX));
		}
		if (keyframes.size() > m)
		{
			for (int k = 0; k < (int)keyframes[m].size(); k++)
			{
				X.write((const char*)&keyframes[m][k], sizeof(X::MORPH_KEYFRAME));
			}
		}
		
	}
	type.type = X::HEADER::DEFAULT;
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.close();
}

void Output::writeSkeleton(Skeleton* skeleton)
{
	std::ofstream X;
	X.open(m_path + "SKELETON" + ".x", std::ios::out | std::ios::binary);
	assert(X.is_open());
	X::HEADER_TYPE type;
	type.type = X::HEADER::SCENE;
	X::SCENE scene;
	scene.fps = skeleton->getFPS();
	scene.animationLenght = skeleton->getAniLength();
	scene.skeletonJointCount = (unsigned short)skeleton->getOffsets().size();
	scene.skeletonKeyframeCount = (unsigned int)skeleton->getkeys(0).size();
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.write((const char*)&scene, sizeof(X::SCENE));
	
	for (unsigned short i = 0; i < scene.skeletonJointCount; i++)
	{
		X.write((const char*)&skeleton->getOffsets()[i], sizeof(X::SkeletonOffset));
	}
	for (unsigned short i = 0; i < scene.skeletonJointCount; i++)
	{
		for (unsigned int j = 0; j < scene.skeletonKeyframeCount; j++)
		{
			X.write((const char*)&skeleton->getkeys(i)[j], sizeof(X::SkeletonKeyFrame));
		}
	}

	type.type = X::HEADER::DEFAULT;
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.close();
}

void Output::writeMaterial(const std::vector<X::MATERIAL>& materials)
{
	std::ofstream X;
	X.open(m_path + "MATERIAL" + ".x", std::ios::out | std::ios::binary);
	assert(X.is_open());
	X::HEADER_TYPE type;
	type.type = X::HEADER::SCENE;
	X::SCENE scene;
	
	scene.materialCount = (unsigned short)materials.size();
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.write((const char*)&scene, sizeof(X::SCENE));

	for (unsigned short i = 0; i < scene.materialCount; i++)
	{
		X.write((const char*)&materials[i], sizeof(X::MATERIAL));
	}
	

	type.type = X::HEADER::DEFAULT;
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.close();
}

void Output::writeLight(const std::vector<X::LIGHT>& lights)
{
	std::ofstream X;
	X.open(m_path + "LIGHT" + ".x", std::ios::out | std::ios::binary);
	assert(X.is_open());
	X::HEADER_TYPE type;
	type.type = X::HEADER::SCENE;
	X::SCENE scene;

	scene.lightCount = (unsigned short)lights.size();
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.write((const char*)&scene, sizeof(X::SCENE));

	for (unsigned short i = 0; i < scene.lightCount; i++)
	{
		X.write((const char*)&lights[i], sizeof(X::LIGHT));
	}


	type.type = X::HEADER::DEFAULT;
	X.write((const char*)&type, sizeof(X::HEADER_TYPE));
	X.close();
}





