#pragma once
#include "Include.h"
struct ControlPoint
{
	
	int index;
	std::vector<float> skinWeight;
	std::vector<short> jointNr;
	std::vector<FbxString> jointName;
	bool operator==(const ControlPoint& other)
	{
		if (index == other.index)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
struct BSC
{
	unsigned int controlPointIndex;
	unsigned int sourceIndex;
	unsigned int targetIndex;
	unsigned int count = 0;
	unsigned int otherControlPointIndex;
};
class Mesh
{
private:
	
	void getVerticesAndIndices(std::vector<X::VERTEX>& vertices, std::vector<unsigned int>& indices, FbxMesh* meshNode, bool global, bool leftHanded, bool averageN,
		ControlPoint* skinningData);
	ControlPoint* getSkinningInformation(FbxSkin* skinNode);
	FbxAxisSystem m_currentAxisSystem;
	void getBlendShapes(FbxGeometry* geometry, fbxsdk::FbxScene* scene, bool leftHanded, FbxMesh* meshNode, bool global);
	void averageNormals(std::vector<X::VERTEX>& vertices, std::vector<unsigned int>& indices);
	void getTangentsAndBiNormals(std::vector<X::VERTEX>& vertices, std::vector<unsigned int>& indices);
	void normalize(std::vector<X::VERTEX>& vertices, bool n, bool t, bool b);
	X::MESH m_mesh;
	std::vector<std::vector<X::MORPH_KEYFRAME>> m_morphKeyframes;
	std::vector<std::vector<X::MORPH_VERTEX>> m_morphVertices;
	std::vector<std::vector<X::MORPH_INDEX>> m_morphIndices;
	std::vector<X::MORPH_TARGET> m_morphTargets;
	std::map<unsigned int, unsigned int> m_meshMorphIndices;
	float m_length = 0.0f;
	void test(std::vector<X::VERTEX>& vertices, FbxMesh* meshNode);
	std::vector<BSC> m_blendShapeC;
public:
	Mesh(const FbxAxisSystem& axisSystem);
	~Mesh();
	const X::MESH& getMesh(fbxsdk::FbxNode* node, fbxsdk::FbxScene* scene, const SelectedOptions& options,  std::vector<X::VERTEX>& vertices,
		 std::vector<unsigned int>& indices);
	unsigned short getTargetCount() { return (unsigned short)m_morphTargets.size(); }
	const X::MORPH_TARGET& getBlendShapes(std::vector<X::MORPH_KEYFRAME>& keys, std::vector<X::MORPH_VERTEX>& vertices, std::vector<X::MORPH_INDEX>& indices,
		unsigned short targetNr);
	float getAniLength() { return m_length; }
	void clearTargets();
};

