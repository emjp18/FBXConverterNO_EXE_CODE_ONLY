#include "Skeleton.h"

Skeleton::Skeleton(const FbxAxisSystem& axisSystem, float fps)
{
	m_currentAxisSystem = axisSystem;
	m_fps = fps;
}

void Skeleton::setData(fbxsdk::FbxNode* node, fbxsdk::FbxScene* scene, const SelectedOptions& options)
{

	FbxGeometry* geometry = node->GetGeometry();
	FbxSkin* skin = (FbxSkin*)geometry->GetDeformer(0, FbxDeformer::eSkin);

	if (!skin)
	{
		return;
	}
	bool shouldSwitchAxisSystem = false;
	if (options.coordinateOptions==Options::CoordinateOptions::LEFTHANDED)
	{
		FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);

		if (m_currentAxisSystem.GetCoorSystem() != leftAxis.GetCoorSystem())
		{
			shouldSwitchAxisSystem = true;
		}
	}
	else
	{
		FbxAxisSystem rightAxis(FbxAxisSystem::OpenGL);

		if (m_currentAxisSystem.GetCoorSystem() != rightAxis.GetCoorSystem())
		{
			shouldSwitchAxisSystem = true;
		}
	}
	FbxTime currTime;
	FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxTakeInfo* take = scene->GetTakeInfo(stack->GetName());
	FbxTime timeInfo = take->mLocalTimeSpan.GetDuration();
	FbxTime start = take->mLocalTimeSpan.GetStart();
	FbxTime stop = take->mLocalTimeSpan.GetStop();
	scene->GetGlobalSettings().SetTimeMode(FbxTime::ConvertFrameRateToTimeMode((double)m_fps));
	auto timeMode = scene->GetGlobalSettings().GetTimeMode();
	int boneCount = skin->GetClusterCount();
	m_aniLength = (float)timeInfo.GetFrameCountPrecise();
	m_offsetJoints.resize(boneCount);
	m_jointKeyData.resize(boneCount);
	for (int bone = 0; bone < boneCount; bone++)
	{
		FbxString name = skin->GetCluster(bone)->GetLink()->GetName();
		FbxNode* boneNode = skin->GetCluster(bone)->GetLink();
		FbxString parentName = skin->GetCluster(bone)->GetLink()->GetParent()->GetName();
		FbxAMatrix offsetM;
		skin->GetCluster(bone)->GetTransformLinkMatrix(offsetM);
		offsetM = offsetM.Inverse();
		if (shouldSwitchAxisSystem)
		{
			FbxQuaternion offsetQ = offsetM.GetQ();
			FbxVector4 offsetT = offsetM.GetT();
			FbxVector4 offsetS = offsetM.GetS();
			offsetS.mData[2] = /*-*/offsetS.mData[2];
			offsetT.mData[2] = -offsetT.mData[2];
			offsetQ.mData[0] = -offsetQ.mData[0];
			offsetQ.mData[1] = -offsetQ.mData[1];
			FbxAMatrix switchedOffsetM;
			switchedOffsetM.SetS(offsetS);
			switchedOffsetM.SetQ(offsetQ);
			switchedOffsetM.SetT(offsetT);
			
			m_offsetJoints[bone].m[0] = (float)switchedOffsetM.Get(0, 0);
			m_offsetJoints[bone].m[1] = (float)switchedOffsetM.Get(0, 1);
			m_offsetJoints[bone].m[2] = (float)switchedOffsetM.Get(0, 2);
			m_offsetJoints[bone].m[3] = (float)switchedOffsetM.Get(0, 3);
			m_offsetJoints[bone].m[4] = (float)switchedOffsetM.Get(1, 0);
			m_offsetJoints[bone].m[5] = (float)switchedOffsetM.Get(1, 1);
			m_offsetJoints[bone].m[6] = (float)switchedOffsetM.Get(1, 2);
			m_offsetJoints[bone].m[7] = (float)switchedOffsetM.Get(1, 3);
			m_offsetJoints[bone].m[8] = (float)switchedOffsetM.Get(2, 0);
			m_offsetJoints[bone].m[9] = (float)switchedOffsetM.Get(2, 1);
			m_offsetJoints[bone].m[10] = (float)switchedOffsetM.Get(2, 2);
			m_offsetJoints[bone].m[11] = (float)switchedOffsetM.Get(2, 3);
			m_offsetJoints[bone].m[12] = (float)switchedOffsetM.Get(3, 0);
			m_offsetJoints[bone].m[13] = (float)switchedOffsetM.Get(3, 1);
			m_offsetJoints[bone].m[14] = (float)switchedOffsetM.Get(3, 2);
			m_offsetJoints[bone].m[15] = (float)switchedOffsetM.Get(3, 3);
		}
		else
		{
			m_offsetJoints[bone].m[0] = (float)offsetM.Get(0, 0);
			m_offsetJoints[bone].m[1] = (float)offsetM.Get(0, 1);
			m_offsetJoints[bone].m[2] = (float)offsetM.Get(0, 2);
			m_offsetJoints[bone].m[3] = (float)offsetM.Get(0, 3);
			m_offsetJoints[bone].m[4] = (float)offsetM.Get(1, 0);
			m_offsetJoints[bone].m[5] = (float)offsetM.Get(1, 1);
			m_offsetJoints[bone].m[6] = (float)offsetM.Get(1, 2);
			m_offsetJoints[bone].m[7] = (float)offsetM.Get(1, 3);
			m_offsetJoints[bone].m[8] = (float)offsetM.Get(2, 0);
			m_offsetJoints[bone].m[9] = (float)offsetM.Get(2, 1);
			m_offsetJoints[bone].m[10] = (float)offsetM.Get(2, 2);
			m_offsetJoints[bone].m[11] = (float)offsetM.Get(2, 3);
			m_offsetJoints[bone].m[12] = (float)offsetM.Get(3, 0);
			m_offsetJoints[bone].m[13] = (float)offsetM.Get(3, 1);
			m_offsetJoints[bone].m[14] = (float)offsetM.Get(3, 2);
			m_offsetJoints[bone].m[15] = (float)offsetM.Get(3, 3);
		}

		

		strcpy_s(m_offsetJoints[bone].jointName, name);
		strcpy_s(m_offsetJoints[bone].jointParentName, parentName);
		m_offsetJoints[bone].jointChildCount = (unsigned short)skin->GetCluster(bone)->GetLink()->GetChildCount();
		m_offsetJoints[bone].jointNr = (unsigned short)bone;
		
		for (FbxLongLong frame = start.GetFrameCount(timeMode); frame <= stop.GetFrameCount(timeMode); frame++)
		{
			currTime.SetFrame(frame, timeMode);
			FbxAMatrix localTransform = boneNode->EvaluateLocalTransform(currTime);
			FbxVector4 s = localTransform.GetS();
			FbxQuaternion r = localTransform.GetQ();
			FbxVector4 t = localTransform.GetT();
			X::SkeletonKeyFrame skeletonKeyFrame;
			//r.Normalize();
			if (shouldSwitchAxisSystem)
			{
				skeletonKeyFrame.s[0] = (float)s.mData[0];
				skeletonKeyFrame.s[1] = (float)s.mData[1];
				skeletonKeyFrame.s[2] = (float)s.mData[2];
				skeletonKeyFrame.s[3] = (float)s.mData[3];
				skeletonKeyFrame.r[0] = -(float)r.mData[0];
				skeletonKeyFrame.r[1] = -(float)r.mData[1];
				skeletonKeyFrame.r[2] = (float)r.mData[2];
				skeletonKeyFrame.r[3] = (float)r.mData[3];
				skeletonKeyFrame.t[0] = (float)t.mData[0];
				skeletonKeyFrame.t[1] = (float)t.mData[1];
				skeletonKeyFrame.t[2] = -(float)t.mData[2];
				skeletonKeyFrame.t[3] = (float)t.mData[3];
			}
			else
			{
				skeletonKeyFrame.s[0] = (float)s.mData[0];
				skeletonKeyFrame.s[1] = (float)s.mData[1];
				skeletonKeyFrame.s[2] = (float)s.mData[2];
				skeletonKeyFrame.s[3] = (float)s.mData[3];
				skeletonKeyFrame.r[0] = (float)r.mData[0];
				skeletonKeyFrame.r[1] = (float)r.mData[1];
				skeletonKeyFrame.r[2] = (float)r.mData[2];
				skeletonKeyFrame.r[3] = (float)r.mData[3];
				skeletonKeyFrame.t[0] = (float)t.mData[0];
				skeletonKeyFrame.t[1] = (float)t.mData[1];
				skeletonKeyFrame.t[2] = (float)t.mData[2];
				skeletonKeyFrame.t[3] = (float)t.mData[3];
			}
			skeletonKeyFrame.jointNr = (unsigned short)bone;
			
			m_jointKeyData[bone].push_back(skeletonKeyFrame);
		}
	}
	
}
