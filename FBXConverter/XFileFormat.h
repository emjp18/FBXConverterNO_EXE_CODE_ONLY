#pragma once
namespace X
{
	enum class HEADER
	{
		MESH,
		MATERIAL,
		SKELETON,
		MORPH_TARGET,
		SCENE,
		LIGHT,
		DEFAULT
	};
	enum class SCENE_TYPE
	{
		MESH,
		MORPH_TARGET,
		SKELETON,
		LIGHT,
		MATERIAL
	};
	enum class LIGHT_TYPE
	{
		Point,
		Spot,
		Dir,
		DEFAULT
	};
	struct HEADER_TYPE
	{
		HEADER type = HEADER::DEFAULT;
	};
	struct SCENE
	{
		unsigned short meshCount = 0;
		unsigned short materialCount = 0;
		unsigned short lightCount = 0;
		unsigned int skeletonKeyframeCount = 0;
		unsigned short skeletonJointCount = 0;
		unsigned short morphTargetsCount = 0;
		float fps = 0.0f;
		float animationLenght = 0.0f;
	};
	struct MESH
	{
		char meshName[100] = { ' ' };
		unsigned int vertexCount = 0;
		unsigned int indexCount = 0;
		unsigned short bonePerVertexCount = 0;
		unsigned short blendShapeCount = 0;
		//only one material per mesh
		char materialName[100] = { ' ' };
	};

	struct VERTEX
	{
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float u = 0.0f;
		float v = 0.0f;
		float normalX = 0.0f;
		float normalY = 0.0f;
		float normalZ = 0.0f;
		float tangentX = 0.0f;
		float tangentY = 0.0f;
		float tangentZ = 0.0f;
		float biNormalX = 0.0f;
		float biNormalY = 0.0f;
		float biNormalZ = 0.0f;
		float skinWeight[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		short jointNr[4] = { -1,-1,-1,-1 };
		char jointName[4][100] = { {' '},{' '},{' '},{' '} };
	};

	struct MORPH_TARGET
	{
		char name[100] = { ' ' };
		char sourceName[100] = { ' ' };
		unsigned int vertexCount = 0;
		unsigned int indexCount = 0;
		unsigned int morphKeyframeCount = 0;
	};

	struct MORPH_VERTEX
	{
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float normalX = 0.0f;
		float normalY = 0.0f;
		float normalZ = 0.0f;
		float tangentX = 0.0f;
		float tangentY = 0.0f;
		float tangentZ = 0.0f;
		float biNormalX = 0.0f;
		float biNormalY = 0.0f;
		float biNormalZ = 0.0f;
	};

	struct MORPH_KEYFRAME
	{
		float weight = 0.0f;
		float timeStamp = 0.0f;
	};

	struct MORPH_INDEX
	{

		unsigned int sourceIndex;
		unsigned int targetIndex;
	};

	struct SkeletonOffset
	{
		unsigned short jointNr;
		char jointName[100] = { ' ' };
		char jointParentName[100] = { ' ' };
		//not recursive
		unsigned short jointChildCount = 0;
		
		float m[16] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f ,0.0f,0.0f,0.0f,0.0f };

	};
	struct SkeletonKeyFrame
	{
		unsigned short jointNr;
		float s[4] = {0.0f,0.0f,0.0f,0.0f };
		//rotationquaternion
		float r[4]= {0.0f,0.0f,0.0f,0.0f };
		float t[4]= {0.0f,0.0f,0.0f,0.0f };
	};

	struct MATERIAL
	{
		char name[100] = { ' ' };
		float ka[3];
		float kd[3];
		//w is shininess
		float ks[4];
	};

	struct LIGHT
	{
		LIGHT_TYPE type = LIGHT_TYPE::DEFAULT;
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float dirX = 0.0f;
		float dirY = 0.0f;
		float dirZ = 0.0f;
		//in radians
		float fov = 0.0f;
		float intensity = 0.0f;
	};
}