#include "Mesh.h"





void Mesh::getVerticesAndIndices(std::vector<X::VERTEX>& vertices, std::vector<unsigned int>& indices, FbxMesh* meshNode, bool global, bool leftHanded, bool averageN,
	ControlPoint* skinningData)
{
	int indexCount = meshNode->GetPolygonVertexCount();
	std::vector<unsigned int> positionIndices(indexCount);
	std::vector<unsigned int> normalIndices;
	std::vector<unsigned int> tangentIndices;
	std::vector<unsigned int> biNormalIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<FbxDouble2> uvVector;
	std::vector<FbxDouble4> posVector;
	std::vector<FbxDouble4> normalVector;
	std::vector<FbxDouble4> tangentVector;
	std::vector<FbxDouble4> binormalVector;
	
	FbxAMatrix localMatrix = meshNode->GetNode()->EvaluateLocalTransform();

	FbxNode* pParentNode = meshNode->GetNode()->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
	}


	FbxAMatrix globalTransform;// = meshNode->GetNode()->EvaluateGlobalTransform();
	globalTransform.SetIdentity();
	FbxAMatrix geoMat;
	geoMat.SetIdentity();
	geoMat.SetS(meshNode->GetNode()->GetGeometricScaling(FbxNode::EPivotSet::eSourcePivot));
	geoMat.SetR(meshNode->GetNode()->GetGeometricRotation(FbxNode::EPivotSet::eSourcePivot));
	geoMat.SetT(meshNode->GetNode()->GetGeometricTranslation(FbxNode::EPivotSet::eSourcePivot));
	
	globalTransform = parentMatrix * localMatrix*geoMat;

	FbxAMatrix inverseT = globalTransform.Inverse();
	inverseT.Transpose();
	

	bool shouldSwitchAxisSystem = false;
	if (leftHanded)
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
	FbxVector4* positions = meshNode->GetControlPoints();

	for (int i = 0; i < (int)meshNode->GetControlPointsCount(); i++)
	{
		if (global)
		{
			positions[i] = globalTransform.MultT(positions[i]);
		}
		FbxDouble4 double4;
		double4.mData[0] = positions[i].mData[0];
		double4.mData[1] = positions[i].mData[1];
		double4.mData[2] = positions[i].mData[2];
		double4.mData[3] = positions[i].mData[3];
		posVector.push_back(double4);

	}
	int faceCount = meshNode->GetPolygonCount();
	
	int indiceUV = 0;
	int indice = 0;
	int indiceNormal = 0;
	int indiceTangent = 0;
	int indiceBinormal = 0;
	bool controlPointUV = false;
	bool controlPointNormal = false;
	bool controlPointTangent = false;
	bool controlPointBiNormal = false;
	for (int face = 0; face < faceCount; face++)
	{
		int polygonSize = meshNode->GetPolygonSize(face);
		for (int polygonVertex = 0; polygonVertex < polygonSize; polygonVertex++)
		{
			int controlPointIndex = meshNode->GetPolygonVertex(face, polygonVertex);
			positionIndices[indice++] = controlPointIndex;
			FbxGeometryElementUV* elementUV = meshNode->GetElementUV();
			FbxGeometryElementNormal* elementNormal = meshNode->GetElementNormal();
			FbxGeometryElementTangent* elementTangent = meshNode->GetElementTangent();
			FbxGeometryElementBinormal* elementBiNormal = meshNode->GetElementBinormal();
			if (elementUV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointUV = true;
				if (elementUV->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector2 UV = elementUV->GetDirectArray().GetAt(controlPointIndex);
					FbxDouble2 double2;
					double2.mData[0] = UV.mData[0];
					double2.mData[1] = UV.mData[1];
					uvVector.push_back(double2);

				}
				else if (elementUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementUV->GetIndexArray().GetAt(controlPointIndex);
					FbxVector2 UV = elementUV->GetDirectArray().GetAt(id);
					FbxDouble2 double2;
					double2.mData[0] = UV.mData[0];
					double2.mData[1] = UV.mData[1];
					uvVector.push_back(double2);

				}
				else
				{
					assert(0);
				}
			}
			else if (elementUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				
				int uvIndex = meshNode->GetTextureUVIndex(face, polygonVertex);
				FbxVector2 UV = elementUV->GetDirectArray().GetAt(uvIndex);
				FbxDouble2 double2;
				double2.mData[0] = UV.mData[0];
				double2.mData[1] = UV.mData[1];
				
				auto it = std::find(uvVector.begin(), uvVector.end(), double2);
				if (it == uvVector.end())
				{
					uvVector.push_back(double2);

					uvIndices.push_back(indiceUV++);
				}
				else
				{
					uvIndices.push_back((unsigned int)std::distance(uvVector.begin(), it));

				}
			}
			else
			{
				assert(0);
			}
			if (elementNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointNormal = true;
				if (elementNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 normal = elementNormal->GetDirectArray().GetAt(controlPointIndex);
					if (global)
					{
						normal = globalTransform.MultT(normal);
					}
					normal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = normal.mData[0];
					double4.mData[1] = normal.mData[1];
					double4.mData[2] = normal.mData[2];
					double4.mData[3] = normal.mData[3];
					normalVector.push_back(double4);

				}
				else if (elementNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementNormal->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 normal = elementNormal->GetDirectArray().GetAt(id);
					if (global)
					{
						
						normal = inverseT.MultR(normal);
					}
					normal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = normal.mData[0];
					double4.mData[1] = normal.mData[1];
					double4.mData[2] = normal.mData[2];
					double4.mData[3] = normal.mData[3];
					normalVector.push_back(double4);

				}
				else
				{
					assert(0);
				}
			}
			else if (elementNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				
				FbxVector4 normal;
				bool result = meshNode->GetPolygonVertexNormal(face, polygonVertex, normal);
				assert(result);
				
				if (global)
				{
					normal = inverseT.MultR(normal);
					
				}
				normal.Normalize();
				FbxDouble4 double4;
				double4.mData[0] = normal.mData[0];
				double4.mData[1] = normal.mData[1];
				double4.mData[2] = normal.mData[2];
				double4.mData[3] = normal.mData[3];
				auto it = std::find(normalVector.begin(), normalVector.end(), double4);
				if (it == normalVector.end())
				{
					normalVector.push_back(double4);

					normalIndices.push_back(indiceNormal++);
				}
				else
				{
					normalIndices.push_back((unsigned int)std::distance(normalVector.begin(), it));

				}
			}
			else
			{
				assert(0);
			}
			if (elementTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointTangent = true;
				if (elementTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(controlPointIndex);
					if (global)
					{
						tangent = inverseT.MultR(tangent);
						
					}
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					tangentVector.push_back(double4);
				}
				else if (elementTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementTangent->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(id);
					if (global)
					{
						tangent = inverseT.MultR(tangent);
					}
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					tangentVector.push_back(double4);
				}
				else
				{
					assert(0);
				}

			}
			else if (elementTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				
				if (elementTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(controlPointIndex);
					if (global)
					{
						tangent = inverseT.MultR(tangent);
						
					}
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					auto it = std::find(tangentVector.begin(), tangentVector.end(), double4);
					if (it == tangentVector.end())
					{
						tangentVector.push_back(double4);

						tangentIndices.push_back(indiceTangent++);
					}
					else
					{
						tangentIndices.push_back((unsigned int)std::distance(tangentVector.begin(), it));

					}
				}
				else if (elementTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementTangent->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 tangent = elementTangent->GetDirectArray().GetAt(id);
					if (global)
					{
						tangent = inverseT.MultR(tangent);
						
					}
					tangent.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = tangent.mData[0];
					double4.mData[1] = tangent.mData[1];
					double4.mData[2] = tangent.mData[2];
					double4.mData[3] = tangent.mData[3];
					auto it = std::find(tangentVector.begin(), tangentVector.end(), double4);
					if (it == tangentVector.end())
					{
						tangentVector.push_back(double4);

						tangentIndices.push_back(indiceTangent++);
					}
					else
					{
						tangentIndices.push_back((unsigned int)std::distance(tangentVector.begin(), it));

					}
				}
				else
				{
					assert(0);
				}
				
			}
			else
			{
				assert(0);
			}
			if (elementBiNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				controlPointBiNormal = true;
				if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(controlPointIndex);
					if (global)
					{
						biNormal = inverseT.MultR(biNormal);
					}
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					binormalVector.push_back(double4);
				}
				else if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementBiNormal->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(id);
					if (global)
					{
						biNormal = globalTransform.MultT(biNormal);
					}
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					binormalVector.push_back(double4);
				}
				else
				{
					assert(0);
				}
			}
			else if (elementBiNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				
				if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(controlPointIndex);
					if (global)
					{
						biNormal = inverseT.MultR(biNormal);
						
					}
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					auto it = std::find(binormalVector.begin(), binormalVector.end(), double4);
					if (it == binormalVector.end())
					{
						binormalVector.push_back(double4);

						biNormalIndices.push_back(indiceBinormal++);
					}
					else
					{
						biNormalIndices.push_back((unsigned int)std::distance(binormalVector.begin(), it));

					}
				}
				else if (elementBiNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int id = elementBiNormal->GetIndexArray().GetAt(controlPointIndex);
					FbxVector4 biNormal = elementBiNormal->GetDirectArray().GetAt(id);
					if (global)
					{
						biNormal = inverseT.MultR(biNormal);
						
					}
					biNormal.Normalize();
					FbxDouble4 double4;
					double4.mData[0] = biNormal.mData[0];
					double4.mData[1] = biNormal.mData[1];
					double4.mData[2] = biNormal.mData[2];
					double4.mData[3] = biNormal.mData[3];
					auto it = std::find(binormalVector.begin(), binormalVector.end(), double4);
					if (it == binormalVector.end())
					{
						binormalVector.push_back(double4);

						biNormalIndices.push_back(indiceBinormal++);
					}
					else
					{
						biNormalIndices.push_back((unsigned int)std::distance(binormalVector.begin(), it));
						
					}
				}
				else
				{
					assert(0);
				}
				
			}
			else
			{
				assert(0);
			}

			

		}
	}
	if (controlPointUV)
	{
		uvIndices = positionIndices;
	}
	if (controlPointNormal)
	{
		normalIndices = positionIndices;
	}
	if (controlPointTangent)
	{
		tangentIndices = positionIndices;
	}
	if (controlPointBiNormal)
	{
		biNormalIndices = positionIndices;
	}

	struct IndexStruct
	{
		FbxDouble4 p;
		FbxDouble2 uv;
		FbxDouble4 n;
		FbxDouble4 t;
		FbxDouble4 b;
		bool operator==(const IndexStruct& other)
		{
			if (p.mData[0] == other.p.mData[0]&&
				p.mData[1] == other.p.mData[1]&&
				p.mData[2] == other.p.mData[2]
				&& uv.mData[0] == other.uv.mData[0]
				&& uv.mData[1] == other.uv.mData[1]
				&& n.mData[0] == other.n.mData[0]
				&& n.mData[1] == other.n.mData[1]
				&& n.mData[2] == other.n.mData[2]
				&& t.mData[0] == other.t.mData[0]
				&& t.mData[1] == other.t.mData[1]
				&& t.mData[2] == other.t.mData[2]
				&& b.mData[0] == other.b.mData[0]
				&& b.mData[1] == other.b.mData[1]
				&& b.mData[2] == other.b.mData[2])
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
	unsigned int indexCounter = 0;
	std::vector<IndexStruct> allIndices;
	for (int i = 0; i < indexCount; i++)
	{
		IndexStruct indexStruct;
		indexStruct.p = posVector[positionIndices[i]];
		indexStruct.uv = uvVector[uvIndices[i]];
		indexStruct.n = normalVector[normalIndices[i]];
		indexStruct.t = tangentVector[tangentIndices[i]];
		indexStruct.b = binormalVector[biNormalIndices[i]];
		

		auto it = std::find(allIndices.begin(), allIndices.end(), indexStruct);
		if (it == allIndices.end())
		{
			
			X::VERTEX vertex;
			if (shouldSwitchAxisSystem)
			{
				vertex.posX = (float)indexStruct.p.mData[0];
				vertex.posY = (float)indexStruct.p.mData[1];
				vertex.posZ = -(float)indexStruct.p.mData[2];

				vertex.u = (float)indexStruct.uv.mData[0];
				vertex.v = -(float)indexStruct.uv.mData[1];

				vertex.normalX = (float)indexStruct.n.mData[0];
				vertex.normalY = (float)indexStruct.n.mData[1];
				vertex.normalZ = -(float)indexStruct.n.mData[2];

				vertex.tangentX = (float)indexStruct.t.mData[0];
				vertex.tangentY = (float)indexStruct.t.mData[1];
				vertex.tangentZ = -(float)indexStruct.t.mData[2];

				vertex.biNormalX = (float)indexStruct.b.mData[0];
				vertex.biNormalY = (float)indexStruct.b.mData[1];
				vertex.biNormalZ = -(float)indexStruct.b.mData[2];
				
			}
			else
			{
				vertex.posX = (float)indexStruct.p.mData[0];
				vertex.posY = (float)indexStruct.p.mData[1];
				vertex.posZ = (float)indexStruct.p.mData[2];

				vertex.u = (float)indexStruct.uv.mData[0];
				vertex.v = (float)indexStruct.uv.mData[1];

				vertex.normalX = (float)indexStruct.n.mData[0];
				vertex.normalY = (float)indexStruct.n.mData[1];
				vertex.normalZ = (float)indexStruct.n.mData[2];

				vertex.tangentX = (float)indexStruct.t.mData[0];
				vertex.tangentY = (float)indexStruct.t.mData[1];
				vertex.tangentZ = (float)indexStruct.t.mData[2];

				vertex.biNormalX = (float)indexStruct.b.mData[0];
				vertex.biNormalY = (float)indexStruct.b.mData[1];
				vertex.biNormalZ = (float)indexStruct.b.mData[2];
				
			}
			if (skinningData)
			{
				assert(m_mesh.bonePerVertexCount < 5);

				for (int j = 0; j < (int)skinningData[positionIndices[i]].jointName.size(); j++)
				{
					if (j > 3)
					{
						break;
					}
					strcpy_s(vertex.jointName[j], skinningData[positionIndices[i]].jointName[j]);
					vertex.jointNr[j] = skinningData[positionIndices[i]].jointNr[j];
					vertex.skinWeight[j] = skinningData[positionIndices[i]].skinWeight[j];
				}


			}
			BSC bsc;
			bsc.sourceIndex = indexCounter;
			bsc.controlPointIndex = positionIndices[i];
			bsc.count = m_blendShapeC.size();
			m_blendShapeC.push_back(bsc);
			/*m_meshMorphIndices.insert({ positionIndices[i],indexCounter });*/ // access the source by control point index.
			vertices.push_back(vertex);
			indices.push_back(indexCounter++);
			allIndices.push_back(indexStruct);
		}
		else
		{
			//m_meshMorphIndices.insert({ positionIndices[i] ,(unsigned int)std::distance(allIndices.begin(), it)}); //need it here?
			BSC bsc;
			bsc.sourceIndex = (unsigned int)std::distance(allIndices.begin(), it);
			bsc.controlPointIndex = positionIndices[i];
			bsc.count = m_blendShapeC.size();
			m_blendShapeC.push_back(bsc);
			indices.push_back((unsigned int)std::distance(allIndices.begin(), it));
			
			
		}
		
	}
	//TEST:::
	/*indices.clear();
	indices = positionIndices;
	vertices.clear();
	X::VERTEX vertex;
	for (int i = 0; i < posVector.size(); i++)
	{
		vertex.posX = (float)posVector[i].mData[0];
		vertex.posY = (float)posVector[i].mData[1];
		vertex.posZ = -(float)posVector[i].mData[2];
		
		
		vertices.push_back(vertex);
	}
	test(vertices, meshNode);*/

	//-................
	if (skinningData)
	{
		delete[] skinningData;
		skinningData = nullptr;
	}
	if (averageN)
	{
		averageNormals(vertices, indices);
	}
}

ControlPoint* Mesh::getSkinningInformation(FbxSkin* skinNode)
{
	int clusterCount = skinNode->GetClusterCount();
	std::vector<FbxString> uniqueClusters;
	std::vector<ControlPoint> controlPoints;
	for (int cluster = 0; cluster < clusterCount; cluster++)
	{
		FbxCluster* clusterNode = skinNode->GetCluster(cluster);
		FbxString jointName = clusterNode->GetLink()->GetName();
		/*auto it0 = std::find(uniqueClusters.begin(), uniqueClusters.end(), jointName);
		if (it0 != uniqueClusters.end())
		{
			continue;
		}
		uniqueClusters.push_back(jointName);*/
		short jointNr = (short)cluster;
		int clusterIndexCount = clusterNode->GetControlPointIndicesCount();
		int* clusterIndices = clusterNode->GetControlPointIndices();
		double* clusterWeights = clusterNode->GetControlPointWeights();
		
		for (int cp = 0; cp < clusterIndexCount; cp++)
		{
			ControlPoint controlpoint;
			controlpoint.index = clusterIndices[cp];
		
			
			auto it = std::find(controlPoints.begin(), controlPoints.end(), controlpoint);
			if (it == controlPoints.end())
			{
				
				controlpoint.jointName.push_back(jointName);
				controlpoint.jointNr.push_back(jointNr);
				controlpoint.skinWeight.push_back((float)clusterWeights[cp]);
				controlPoints.push_back(controlpoint);
			}
			else
			{
				int index = (int)std::distance(controlPoints.begin(), it);
				auto it2 = std::find(controlPoints[index].jointNr.begin(), controlPoints[index].jointNr.end(), jointNr);

				if (it2 == controlPoints[index].jointNr.end())
				{
					controlPoints[index].jointName.push_back(jointName);
					controlPoints[index].jointNr.push_back(jointNr);
					controlPoints[index].skinWeight.push_back((float)clusterWeights[cp]);
				}
				
				
			}
			
			
		}
	}
	ControlPoint* controlpointsPtr = new ControlPoint[controlPoints.size()];
	int lod = -INT_MAX;
	for (int cp = 0; cp < (int)controlPoints.size(); cp++)
	{
		// the one with index n should be at n.
		controlpointsPtr[controlPoints[cp].index] = controlPoints[cp];
		
		lod = std::max(lod, (int)controlPoints[cp].jointNr.size());
	}
	m_mesh.bonePerVertexCount = (unsigned short)lod;
	
	return controlpointsPtr;
}



void Mesh::getBlendShapes(FbxGeometry* geometry, fbxsdk::FbxScene* scene, bool leftHanded, FbxMesh* meshNode, bool global)
{
	struct IndexStruct
	{
		FbxVector4 p;
		FbxVector4 n;
		FbxVector4 t;
		FbxVector4 b;
		
		bool operator==(const IndexStruct& other)
		{
			if (p.mData[0] == other.p.mData[0]
				&& p.mData[1] == other.p.mData[1]
				&& p.mData[2] == other.p.mData[2]
				&& n.mData[0] == other.n.mData[0]
				&& n.mData[1] == other.n.mData[1]
				&& n.mData[2] == other.n.mData[2]
				&& t.mData[0] == other.t.mData[0]
				&& t.mData[1] == other.t.mData[1]
				&& t.mData[2] == other.t.mData[2]
				&& b.mData[0] == other.b.mData[0]
				&& b.mData[1] == other.b.mData[1]
				&& b.mData[2] == other.b.mData[2])
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
	bool shouldSwitchAxisSystem = false;
	if (leftHanded)
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
	
	int blendShapeDeformerCount = geometry->GetDeformerCount(FbxDeformer::eBlendShape);
	FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxTakeInfo* take = scene->GetTakeInfo(stack->GetName());

	FbxTime timeInfo = take->mLocalTimeSpan.GetDuration();

	m_length = (float)timeInfo.GetFrameCountPrecise();
	int layerCount = stack->GetMemberCount<FbxAnimLayer>();
	for (int bsd = 0; bsd < blendShapeDeformerCount; bsd++)
	{
		FbxBlendShape* blendShape = (FbxBlendShape*)geometry->GetDeformer(bsd, FbxDeformer::eBlendShape);
		int blendShapeCount = blendShape->GetBlendShapeChannelCount();
		m_morphTargets.resize(blendShapeCount);
		m_morphKeyframes.resize(blendShapeCount);
		m_morphVertices.resize(blendShapeCount);
		m_morphIndices.resize(blendShapeCount);
		for (int layer = 0; layer < layerCount; layer++)
		{
			FbxAnimLayer* animLayer = stack->GetMember<FbxAnimLayer>(layer);

			for (int bs = 0; bs < blendShapeCount; bs++)
			{
				FbxBlendShapeChannel* channel = blendShape->GetBlendShapeChannel(bs);
				strcpy_s(m_morphTargets[bs].sourceName, m_mesh.meshName);
				strcpy_s(m_morphTargets[bs].name, channel->GetName());
				FbxAnimCurve* animCurve = geometry->GetShapeChannel(bsd, bs, animLayer);
				if (animCurve)
				{
					int keyCount = animCurve->KeyGetCount();
					m_morphKeyframes[bs].resize(keyCount);
					for (int key = 0; key < keyCount; key++)
					{
						m_morphKeyframes[bs][key].weight = animCurve->KeyGetValue(key) / 100.0f;
						m_morphKeyframes[bs][key].timeStamp = (float)animCurve->KeyGetTime(key).GetFrameCountPrecise();
					}

					m_morphTargets[bs].morphKeyframeCount = (unsigned int)keyCount;
				}
				else
				{
					m_morphKeyframes.clear();
				}
				FbxShape* shape = channel->GetTargetShape(0);
				FbxLayerElementArrayTemplate<FbxVector4>* narr = nullptr;
				bool normalBool = shape->GetNormals(&narr);
				FbxLayerElementArrayTemplate<FbxVector4>* tarr = nullptr;
				bool tangentBool = shape->GetTangents(&tarr);
				FbxLayerElementArrayTemplate<FbxVector4>* barr = nullptr;
				bool biNormalsBool = shape->GetBinormals(&barr);
				FbxLayerElementArrayTemplate<int>* nInd = nullptr;
				bool normalIndiceseBool = shape->GetNormalsIndices(&nInd);
				FbxLayerElementArrayTemplate<int>* tInd = nullptr;
				bool tangentIndiceseBool = shape->GetTangentsIndices(&tInd);
				FbxLayerElementArrayTemplate<int>* bInd = nullptr;
				bool biNormalIndiceseBool = shape->GetBinormalsIndices(&bInd);
				int* posIndices = shape->GetControlPointIndices();
				FbxVector4* cp = shape->GetControlPoints();
				
				//
				FbxAMatrix inverseT;
				inverseT.SetIdentity();
				FbxAMatrix globalTransform;
				globalTransform.SetIdentity();
				
				//FbxAMatrix localMatrix = geometry->GetNode()->EvaluateLocalTransform();

				//FbxNode* pParentNode = geometry->GetNode()->GetParent();
				//FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
				//while ((pParentNode = pParentNode->GetParent()) != NULL)
				//{
				//	parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
				//}


				//;// = meshNode->GetNode()->EvaluateGlobalTransform();
				//globalTransform.SetIdentity();
				//FbxAMatrix geoMat;
				//geoMat.SetIdentity();
				//geoMat.SetS(geometry->GetNode()->GetGeometricScaling(FbxNode::EPivotSet::eSourcePivot));
				//geoMat.SetR(geometry->GetNode()->GetGeometricRotation(FbxNode::EPivotSet::eSourcePivot));
				//geoMat.SetT(geometry->GetNode()->GetGeometricTranslation(FbxNode::EPivotSet::eSourcePivot));

				//globalTransform = parentMatrix * localMatrix * geoMat;

				//inverseT = globalTransform.Inverse();
				//inverseT.Transpose();
				
				//
				
				

				std::map< unsigned int, unsigned int> controlTargets;
				std::map< unsigned int, unsigned int> controlTargets2;
				int c = 0;
				std::vector<IndexStruct> totalIndices;
				for (int i = 0; i < shape->GetControlPointIndicesCount(); i++)
				{
					IndexStruct indexStruct;
					indexStruct.p = cp[posIndices[i]];
					if (normalBool&& normalIndiceseBool)
					{
						indexStruct.n = narr->GetAt(nInd->GetAt(i));
					}
					else
					{
						indexStruct.n.mData[0] = 0;
						indexStruct.n.mData[1] = 0;
						indexStruct.n.mData[2] = 0;
					}
					if (tangentBool &&tangentIndiceseBool)
					{
						indexStruct.t = tarr->GetAt(tInd->GetAt(i));
					}
					else
					{
						indexStruct.t.mData[0] = 0;
						indexStruct.t.mData[1] = 0;
						indexStruct.t.mData[2] = 0;
					}
					if (biNormalsBool&&biNormalIndiceseBool)
					{
						indexStruct.b = barr->GetAt(bInd->GetAt(i));
					}
					else
					{
						indexStruct.b.mData[0] = 0;
						indexStruct.b.mData[1] = 0;
						indexStruct.b.mData[2] = 0;
					}
					
					
					/*unsigned int correctIndex = posIndices[i];*/
					auto it = std::find(totalIndices.begin(), totalIndices.end(), indexStruct);
					/*X::MORPH_INDEX morphIndex;*/

					if (it == totalIndices.end())
					{
						totalIndices.push_back(indexStruct);
						X::MORPH_VERTEX vertex;

						if (global)
						{
							FbxDouble4 transformedT = globalTransform.MultT(indexStruct.p.mData);
							indexStruct.p.mData[0] = transformedT[0];
							indexStruct.p.mData[1] = transformedT[1];
							indexStruct.p.mData[2] = transformedT[2];

							FbxDouble4 transformedN = inverseT.MultR(indexStruct.n.mData);
							indexStruct.n.mData[0] = transformedN[0];
							indexStruct.n.mData[1] = transformedN[1];
							indexStruct.n.mData[2] = transformedN[2];

							FbxDouble4 transformedTan = inverseT.MultR(indexStruct.t.mData);
							indexStruct.t.mData[0] = transformedTan[0];
							indexStruct.t.mData[1] = transformedTan[1];
							indexStruct.t.mData[2] = transformedTan[2];

							FbxDouble4 transformedBN = inverseT.MultR(indexStruct.b.mData);
							indexStruct.b.mData[0] = transformedBN[0];
							indexStruct.b.mData[1] = transformedBN[1];
							indexStruct.b.mData[2] = transformedBN[2];
						}


						if (shouldSwitchAxisSystem)
						{
							vertex.posX = (float)indexStruct.p.mData[0];
							vertex.posY = (float)indexStruct.p.mData[1];
							vertex.posZ = -(float)indexStruct.p.mData[2];
							vertex.normalX = (float)indexStruct.n.mData[0];
							vertex.normalY = (float)indexStruct.n.mData[1];
							vertex.normalZ = -(float)indexStruct.n.mData[2];
							vertex.tangentX = (float)indexStruct.t.mData[0];
							vertex.tangentY = (float)indexStruct.t.mData[1];
							vertex.tangentZ = -(float)indexStruct.t.mData[2];
							vertex.biNormalX = (float)indexStruct.b.mData[0];
							vertex.biNormalY = (float)indexStruct.b.mData[1];
							vertex.biNormalZ = -(float)indexStruct.b.mData[2];
						}
						else
						{
							vertex.posX = (float)indexStruct.p.mData[0];
							vertex.posY = (float)indexStruct.p.mData[1];
							vertex.posZ = (float)indexStruct.p.mData[2];
							vertex.normalX = (float)indexStruct.n.mData[0];
							vertex.normalY = (float)indexStruct.n.mData[1];
							vertex.normalZ = (float)indexStruct.n.mData[2];
							vertex.tangentX = (float)indexStruct.t.mData[0];
							vertex.tangentY = (float)indexStruct.t.mData[1];
							vertex.tangentZ = (float)indexStruct.t.mData[2];
							vertex.biNormalX = (float)indexStruct.b.mData[0];
							vertex.biNormalY = (float)indexStruct.b.mData[1];
							vertex.biNormalZ = (float)indexStruct.b.mData[2];
						}
						m_morphVertices[bs].push_back(vertex);
					
						
						controlTargets.insert({ (unsigned int)posIndices[i], c++ });
					}
					else
					{
						
						/*controlTargets2.insert({ (unsigned int)posIndices[i] , std::distance(totalIndices.begin(), it) });*/
					}
				}
				
				
				X::MORPH_INDEX morphIndex;
				std::vector<unsigned int> tempTest;
				for (int i = 0; i < m_blendShapeC.size(); i++)
				{
					auto it = std::find(tempTest.begin(), tempTest.end(), m_blendShapeC[i].controlPointIndex);
					if (it == tempTest.end())
					{
						if (controlTargets.find(m_blendShapeC[i].controlPointIndex) != controlTargets.end())
						{
							morphIndex.targetIndex = controlTargets[m_blendShapeC[i].controlPointIndex];
							morphIndex.sourceIndex = m_blendShapeC[i].sourceIndex;
							m_morphIndices[bs].push_back(morphIndex);
							
						}
						tempTest.push_back(m_blendShapeC[i].controlPointIndex);
					}
					
				}
				m_morphIndices[bs] = m_morphIndices[bs];
				m_morphVertices[bs] = m_morphVertices[bs];
				m_morphTargets[bs].indexCount = (unsigned int)m_morphIndices[bs].size();
				m_morphTargets[bs].vertexCount = (unsigned int)m_morphVertices[bs].size();
			}

		}


	}


}

void Mesh::averageNormals(std::vector<X::VERTEX>& vertices, std::vector<unsigned int>& indices)
{
	
	for (int index = 0; index < (int)indices.size(); index += 3)
	{
		FbxVector4 vertexA;
		FbxVector4 vertexB;
		FbxVector4 vertexC;
		
		vertexA.mData[0] = vertices[indices[index]].posX;
		vertexA.mData[1] = vertices[indices[index]].posY;
		vertexA.mData[2] = vertices[indices[index]].posZ;

		vertexB.mData[0] = vertices[indices[index+1]].posX;
		vertexB.mData[1] = vertices[indices[index + 1]].posY;
		vertexB.mData[2] = vertices[indices[index + 1]].posZ;

		vertexC.mData[0] = vertices[indices[index+2]].posX;
		vertexC.mData[1] = vertices[indices[index + 2]].posY;
		vertexC.mData[2] = vertices[indices[index + 2]].posZ;

		FbxVector4 edgeA = vertexB - vertexA;
		FbxVector4 edgeB = vertexC - vertexA;

		FbxVector4 surfaceNormal = edgeB.CrossProduct(edgeA);
		
		vertices[indices[index]].normalX += (float)surfaceNormal.mData[0];
		vertices[indices[index]].normalY += (float)surfaceNormal.mData[1];
		vertices[indices[index]].normalZ += (float)surfaceNormal.mData[2];

		vertices[indices[index+1]].normalX = (float)surfaceNormal.mData[0];
		vertices[indices[index + 1]].normalY += (float)surfaceNormal.mData[1];
		vertices[indices[index + 1]].normalZ +=(float)surfaceNormal.mData[2];

		vertices[indices[index+2]].normalX += (float)surfaceNormal.mData[0];
		vertices[indices[index + 2]].normalY += (float)surfaceNormal.mData[1];
		vertices[indices[index + 2]].normalZ += (float)surfaceNormal.mData[2];

		normalize(vertices, true, false, false);
	}
	
	getTangentsAndBiNormals(vertices, indices);

	
}

void Mesh::getTangentsAndBiNormals(std::vector<X::VERTEX>& vertices, std::vector<unsigned int>& indices)
{
	FbxVector4 init(0, 0, 0, 0);
	std::vector<FbxVector4> tanA(vertices.size(), init);
	std::vector<FbxVector4> tanB(vertices.size(), init);

	for (int i = 0; i < (int)((float)indices.size() / 3.0f); i++)
	{
		int i1 = indices[i * 3];
		int i2 = indices[i * 3 + 1];
		int i3 = indices[i * 3 + 2];

		FbxVector4 pos1(vertices[i1].posX, vertices[i1].posX, vertices[i1].posX, 0);
		FbxVector4 pos2(vertices[i2].posY, vertices[i2].posY, vertices[i2].posY, 0);
		FbxVector4 pos3(vertices[i3].posZ, vertices[i3].posZ, vertices[i3].posZ, 0);

		FbxVector4 uv1(vertices[i1].u, vertices[i1].v, 0, 0);
		FbxVector4 uv2(vertices[i2].u, vertices[i2].v, 0, 0);
		FbxVector4 uv3(vertices[i3].u, vertices[i3].v, 0, 0);

		FbxVector4 edge1 = pos2 - pos1;
		FbxVector4 edge2 = pos3 - pos1;

		FbxVector4 u = uv2 - uv1;
		FbxVector4 v = uv3 - uv1;
		float r = 1;

		if ((u.mData[0] * v.mData[1]) - (u.mData[1] * v.mData[0]) != 0)
		{
			float r = 1.0f / (((float)u.mData[0] * (float)v.mData[1]) - ((float)u.mData[1] * (float)v.mData[0]));

		}

		FbxVector4 tan((((edge1.mData[0] * v.mData[1]) - (edge2.mData[0] * u.mData[1]))) * (double)r,
			(((edge1.mData[1] * v.mData[1]) - (edge2.mData[1] * u.mData[1]))) * (double)r,
			(((edge1.mData[2]) * v.mData[1]) - (edge2.mData[2] * u.mData[1])) * (double)r, 0.0);

		FbxVector4 bi((((edge1.mData[0] * v.mData[0]) - (edge2.mData[0] * u.mData[0]))) * (double)r,
			(((edge1.mData[1] * v.mData[0]) - (edge2.mData[1] * u.mData[0]))) * (double)r,
			(((edge1.mData[2]) * v.mData[0]) - (edge2.mData[2] * u.mData[0])) * (double)r, 0.0);

	

		tanA[i1].mData[0] += tan[0];
		tanA[i1].mData[1] += tan[1];
		tanA[i1].mData[2] += tan[2];

		tanA[i2].mData[0] += tan[0];
		tanA[i2].mData[1] += tan[1];
		tanA[i2].mData[2] += tan[2];

		tanA[i3].mData[0] += tan[0];
		tanA[i3].mData[1] += tan[1];
		tanA[i3].mData[2] += tan[2];


		tanB[i1].mData[0] += bi[0];
		tanB[i1].mData[1] += bi[1];
		tanB[i1].mData[2] += bi[2];

		tanB[i2].mData[0] += bi[0];
		tanB[i2].mData[1] += bi[1];
		tanB[i2].mData[2] += bi[2];

		tanB[i3].mData[0] += bi[0];
		tanB[i3].mData[1] += bi[1];
		tanB[i3].mData[2] += bi[2];

	}

	for (int i = 0; i < (int)vertices.size(); i++)
	{
		FbxVector4 t0(tanA[i].mData[0], tanA[i].mData[1], tanA[i].mData[2], 0);
		FbxVector4 t1(tanB[i].mData[0], tanB[i].mData[1], tanB[i].mData[2], 0);

		FbxVector4 n(vertices[i].normalX, vertices[i].normalY, vertices[i].normalZ, 0);

		double scalar = n.DotProduct(t0);

		n *= scalar;

		FbxVector4 t = t0 - n;

		t.Normalize();

		vertices[i].tangentX =(float)t[0];
		vertices[i].tangentY =(float)t[1];
		vertices[i].tangentZ =(float)t[2];

		FbxVector4 bi = t.CrossProduct(n);

		bi.Normalize();

		vertices[i].biNormalX = (float)bi[0];
		vertices[i].biNormalY = (float)bi[1];
		vertices[i].biNormalZ = (float)bi[2];


	}
}

void Mesh::normalize(std::vector<X::VERTEX>& vertices, bool n, bool t, bool b)
{
	if (n)
	{
		for (int i = 0; i < (int)vertices.size(); i++)
		{
			FbxVector4 n(vertices[i].normalX, vertices[i].normalY, vertices[i].normalZ, 0);

			n.Normalize();

			vertices[i].normalX = (float)n.mData[0];
			vertices[i].normalY = (float)n.mData[1];
			vertices[i].normalZ = (float)n.mData[2];
		}
	}

	if (t)
	{
		for (int i = 0; i < (int)vertices.size(); i++)
		{
			FbxVector4 t(vertices[i].tangentX, vertices[i].tangentY, vertices[i].tangentZ, 0);

			t.Normalize();

			vertices[i].tangentX = (float)t.mData[0];
			vertices[i].tangentY = (float)t.mData[1];
			vertices[i].tangentZ = (float)t.mData[2];
		}
	}

	if (b)
	{
		for (int i = 0; i < (int)vertices.size(); i++)
		{
			FbxVector4 b(vertices[i].biNormalX, vertices[i].biNormalY, vertices[i].biNormalZ, 0);

			b.Normalize();

			vertices[i].biNormalX = (float)b.mData[0];
			vertices[i].biNormalY = (float)b.mData[1];
			vertices[i].biNormalZ = (float)b.mData[2];
		}
	}
}











void Mesh::test(std::vector<X::VERTEX>& vertices, FbxMesh* meshNode)
{
	FbxGeometry* geometry = meshNode->GetNode()->GetGeometry();
	
	FbxSkin* skinNode = (FbxSkin*)geometry->GetDeformer(0, FbxDeformer::eSkin);

	int clusterCount = skinNode->GetClusterCount();
	
	std::vector<ControlPoint> controlPoints;
	for (int cluster = 0; cluster < clusterCount; cluster++)
	{
		FbxCluster* clusterNode = skinNode->GetCluster(cluster);
		FbxString jointName = clusterNode->GetLink()->GetName();
		
		short jointNr = (short)cluster;
		int clusterIndexCount = clusterNode->GetControlPointIndicesCount();
		int* clusterIndices = clusterNode->GetControlPointIndices();
		double* clusterWeights = clusterNode->GetControlPointWeights();
		for (int cp = 0; cp < clusterIndexCount; cp++)
		{
			
			int index = clusterIndices[cp];
			if (vertices[index].jointNr[0] == -1)
			{
				vertices[index].jointNr[0] = jointNr;
				strcpy_s(vertices[index].jointName[0], jointName);
				vertices[index].skinWeight[0] = (float)clusterWeights[cp];
			}
			else if (vertices[index].jointNr[1] == -1)
			{
				vertices[index].jointNr[1] = jointNr;
				strcpy_s(vertices[index].jointName[1], jointName);
				vertices[index].skinWeight[1] = (float)clusterWeights[cp];
			}
			else if (vertices[index].jointNr[2] == -1)
			{
				vertices[index].jointNr[2] = jointNr;
				strcpy_s(vertices[index].jointName[2], jointName);
				vertices[index].skinWeight[2] = (float)clusterWeights[cp];
			}
		}
	}
	

}

Mesh::Mesh(const FbxAxisSystem& axisSystem)
{
	m_currentAxisSystem = axisSystem;
}

Mesh::~Mesh()
{
}

 const X::MESH& Mesh::getMesh(fbxsdk::FbxNode* node, fbxsdk::FbxScene* scene, const SelectedOptions& options,  std::vector<X::VERTEX>& vertices,
	 std::vector<unsigned int>& indices) 
{
	FbxMesh* meshNode = (FbxMesh*)node->GetNodeAttribute();
	const char* meshName = node->GetName();
	strcpy_s(m_mesh.meshName, meshName);

	FbxSurfaceMaterial* mat = node->GetMaterial(0);

	const char* matName = mat->GetName();

	strcpy_s(m_mesh.materialName, matName);


	bool global = false;
	bool leftHanded = false;
	bool averageN = false;
	if (options.coordinateOptions == Options::CoordinateOptions::LEFTHANDED)
	{
		leftHanded = true;
	}
	if (options.spaceOptions == Options::SpaceOptions::GLOBAL)
	{
		global = true;
	}
	if (options.normalOptions == Options::NormalOptions::AVERAGE)
	{
		averageN = true;
	}
	FbxGeometry* geometry = node->GetGeometry();
	ControlPoint* skinningData = nullptr;
	if (geometry->GetDeformerCount(FbxDeformer::eSkin) > 0)
	{
		FbxSkin* skin = (FbxSkin*)geometry->GetDeformer(0, FbxDeformer::eSkin);
		skinningData = getSkinningInformation(skin);
	}
	getVerticesAndIndices(vertices, indices, meshNode, global, leftHanded,averageN, skinningData);
	m_mesh.indexCount = (unsigned int)indices.size();
	m_mesh.vertexCount = (unsigned int)vertices.size();
	auto meshFlag = options.fileOptions;
	if ((int)(meshFlag & Options::FileOptions::MORPH) != 0&& geometry->GetDeformerCount(FbxDeformer::eBlendShape) > 0)
	{
		getBlendShapes(geometry, scene, leftHanded, meshNode, global);
	}

	return m_mesh;
}

 const X::MORPH_TARGET& Mesh::getBlendShapes(std::vector<X::MORPH_KEYFRAME>& keys, std::vector<X::MORPH_VERTEX>& vertices, std::vector<X::MORPH_INDEX>& indices, unsigned short targetNr)
 {
	 if (m_morphKeyframes.size() > targetNr)
	 {
		 keys = m_morphKeyframes[targetNr];
	 }
	 vertices = m_morphVertices[targetNr];
	 indices = m_morphIndices[targetNr];

	 return m_morphTargets[targetNr];
 }

 void Mesh::clearTargets()
 {
	 m_morphKeyframes.clear();
	  m_morphVertices.clear();
	 m_morphIndices.clear();
	 m_morphTargets.clear();
	 m_meshMorphIndices.clear();
 }

 

 
