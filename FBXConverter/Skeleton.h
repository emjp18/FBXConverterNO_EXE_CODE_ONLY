#pragma once
#include "Include.h"
class Skeleton
{
private:
	std::vector<X::SkeletonOffset> m_offsetJoints;
	std::vector<std::vector<X::SkeletonKeyFrame>> m_jointKeyData;
	float m_aniLength;
	float m_fps;
	FbxAxisSystem m_currentAxisSystem;
public:
	Skeleton(const FbxAxisSystem& axisSystem, float fps);
	void setData(fbxsdk::FbxNode* node, fbxsdk::FbxScene* scene, const SelectedOptions& options);
	const std::vector<X::SkeletonKeyFrame>& getkeys(unsigned short jointNr) { return m_jointKeyData[jointNr]; }
	const std::vector<X::SkeletonOffset>& getOffsets() { return m_offsetJoints; }
	float getAniLength() { return m_aniLength; }
	float getFPS() { return m_fps; }
};

