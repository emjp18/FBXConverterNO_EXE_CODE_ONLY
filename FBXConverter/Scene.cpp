#include "Scene.h"

Scene::Scene(const FbxString& path, float fps)
{
	m_sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(m_sdkManager, IOSROOT);
	m_sdkManager->SetIOSettings(ios);
	FbxImporter* importer = FbxImporter::Create(m_sdkManager, "");
	m_fps = fps;
	bool result;

	result = importer->Initialize(path, -1, m_sdkManager->GetIOSettings());
	assert(result);

	m_scene = FbxScene::Create(m_sdkManager, "scene");

	importer->Import(m_scene);
	importer->Destroy();

	m_mesh = new Mesh(m_scene->GetGlobalSettings().GetAxisSystem());

	m_skeleton = new Skeleton(m_scene->GetGlobalSettings().GetAxisSystem(), fps);
}

Scene::~Scene()
{
	
	m_sdkManager->Destroy();
	
	if (m_mesh)
	{
		delete m_mesh;
		m_mesh = nullptr;
	}
	if (m_skeleton)
	{
		delete m_skeleton;
		m_skeleton = nullptr;
	}
}


void Scene::getMaterial()
{
	int matCount = m_scene->GetMaterialCount();
	
	m_materials.resize(matCount);

	for (int i = 0; i < matCount; i++)
	{
		FbxSurfaceMaterial* material = m_scene->GetMaterial(i);

		FbxClassId type = material->GetClassId();

		const char* name = material->GetName();

		strcpy_s(m_materials[i].name, name);

		if (type.Is(FbxSurfaceLambert::ClassId))
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)material;



			FbxDouble3 ka = lambert->Ambient;
			FbxDouble3 kd = lambert->Diffuse;
			

			m_materials[i].ka[0] = (float)ka[0];
			m_materials[i].ka[1] = (float)ka[1];
			m_materials[i].ka[2] = (float)ka[2];

			m_materials[i].kd[0] = (float)kd[0];
			m_materials[i].kd[1] = (float)kd[1];
			m_materials[i].kd[2] = (float)kd[2];



		}
		if (type.Is(FbxSurfacePhong::ClassId))
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong*)material;

			FbxDouble3 ka = phong->Ambient;
			FbxDouble3 kd = phong->Diffuse;
			FbxDouble3 ks = phong->Specular;
			FbxDouble exponent = phong->SpecularFactor;
			
			

			m_materials[i].ka[0] = (float)ka[0];
			m_materials[i].ka[1] = (float)ka[1];
			m_materials[i].ka[2] = (float)ka[2];

			m_materials[i].kd[0] = (float)kd[0];
			m_materials[i].kd[1] = (float)kd[1];
			m_materials[i].kd[2] = (float)kd[2];

			m_materials[i].ks[0] = (float)ks[0];
			m_materials[i].ks[1] = (float)ks[1];
			m_materials[i].ks[2] = (float)ks[2];

			m_materials[i].ks[3] = (float)exponent;

		
		}
	}
}

FbxScene* Scene::getScene()
{
	return m_scene;
}

void Scene::getNodes(FbxNode* node)
{
	if (node->GetNodeAttribute())
	{
		FbxNodeAttribute::EType attributeType = (node->GetNodeAttribute()->GetAttributeType());

		switch (attributeType)
		{
		case FbxNodeAttribute::eLight:
			getLight(node);
			break;
		case FbxNodeAttribute::eMesh:
			
			if ((int)(m_options.fileOptions & Options::FileOptions::MESH) != 0)
			{
				m_vertices.push_back(m_vertexVector);
				m_indices.push_back(m_indexVector);
				m_meshes.push_back(m_mesh->getMesh(node, m_scene, m_options, m_vertices[m_vertices.size() - 1], m_indices[m_indices.size() - 1]));
				if ((int)(m_options.fileOptions & Options::FileOptions::MORPH) != 0)
				{
					for (unsigned short i = 0; i < m_mesh->getTargetCount(); i++)
					{
						std::vector<X::MORPH_VERTEX> mVertices;
						std::vector<X::MORPH_INDEX> mIndices;
						std::vector<X::MORPH_KEYFRAME> mKeys;
						m_targets.push_back(m_mesh->getBlendShapes(mKeys, mVertices, mIndices, i));
						m_morphIndices.push_back(mIndices);
						if (mKeys.size() > 0)
						{
							m_morphKeyframes.push_back(mKeys);
						}
						m_morphVertices.push_back(mVertices);

					}
				}
				
			}
		
			m_mesh->clearTargets();
			if ((int)(m_options.fileOptions & Options::FileOptions::SKELETON) != 0)
			{
				m_skeleton->setData(node, m_scene, m_options);
			}
			
			break;
		
		default:
			break;
		}

	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		getNodes(node->GetChild(i));
	}
}

void Scene::getRootNode()
{
	FbxNode* node = m_scene->GetRootNode();
	if (node)
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			getNodes(node->GetChild(i));
		}
	}
	if ((int)(m_options.fileOptions & Options::FileOptions::MATERIAL) != 0)
	{
		getMaterial();
	}
}

void Scene::getLight(FbxNode* lightNode)
{
	
	FbxVector4 forward;
	bool shouldSwitchAxisSystem = false;
	if (m_options.coordinateOptions==Options::CoordinateOptions::LEFTHANDED)
	{
		FbxAxisSystem leftAxis(FbxAxisSystem::DirectX);

		if (m_scene->GetGlobalSettings().GetAxisSystem().GetCoorSystem() != leftAxis.GetCoorSystem())
		{
			shouldSwitchAxisSystem = true;
			forward.Set(0, 0, -1, 0);
		}
	}
	else
	{
		FbxAxisSystem rightAxis(FbxAxisSystem::OpenGL);

		if (m_scene->GetGlobalSettings().GetAxisSystem().GetCoorSystem() != rightAxis.GetCoorSystem())
		{
			shouldSwitchAxisSystem = true;
			forward.Set(0, 0, 1, 0);
		}
	}
	bool global = false;
	if (m_options.spaceOptions == Options::SpaceOptions::GLOBAL)
	{
		global = true;
	}

	X::LIGHT lightStruct;

	FbxLight* light = nullptr;
	
	light = (FbxLight*)lightNode->GetNodeAttribute();
	
	
	
	//
	FbxLight::EType lightType = light->LightType.Get();

	if (lightType == FbxLight::EType::ePoint)
	{
		lightStruct.type = X::LIGHT_TYPE::Point;
		FbxDouble3 t_color = light->Color.Get();
		lightStruct.r = (float)t_color[0];
		lightStruct.g = (float)t_color[1];
		lightStruct.b = (float)t_color[2];

		FbxDouble t_intensity = light->Intensity.Get();
		lightStruct.intensity = (float)t_intensity/100.0f;

		FbxDouble3 t_pos = lightNode->LclTranslation.Get();
		
		
		if (shouldSwitchAxisSystem)
		{
			lightStruct.posX = (float)t_pos.mData[0];
			lightStruct.posY = (float)t_pos.mData[1];
			lightStruct.posZ = -(float)t_pos.mData[2];
		}
		else
		{
			lightStruct.posX = (float)t_pos.mData[0];
			lightStruct.posY = (float)t_pos.mData[1];
			lightStruct.posZ = (float)t_pos.mData[2];
		}
		

		m_lights.push_back(lightStruct);
		return;
	}
	else if (lightType == FbxLight::EType::eDirectional)
	{
		lightStruct.type = X::LIGHT_TYPE::Dir;
		auto eulerRot = lightNode->LclRotation.Get();
		FbxDouble3 t_color = light->Color.Get();
		lightStruct.r = (float)t_color[0];
		lightStruct.g = (float)t_color[1];
		lightStruct.b = (float)t_color[2];

		FbxDouble t_intensity = light->Intensity.Get();
		lightStruct.intensity = (float)t_intensity / 100.0f;
		
		FbxAMatrix rotM;
		rotM.SetIdentity();
		rotM.SetR({ eulerRot }, FbxEuler::EOrder::eOrderXYZ);
		
		forward = rotM.MultT(forward);

		if (shouldSwitchAxisSystem)
		{
			lightStruct.dirX = (float)forward.mData[0];
			lightStruct.dirY = (float)forward.mData[1];
			lightStruct.dirZ = -(float)forward.mData[2];
		}
		else
		{
			lightStruct.dirX = (float)forward.mData[0];
			lightStruct.dirY = (float)forward.mData[1];
			lightStruct.dirZ = (float)forward.mData[2];
		}
		
		
		


		
		m_lights.push_back(lightStruct);
		return;
	}
	else if (lightType == FbxLight::EType::eSpot)
	{
		lightStruct.type = X::LIGHT_TYPE::Spot;

		FbxDouble3 t_color = light->Color.Get();
		lightStruct.r = (float)t_color[0];
		lightStruct.g = (float)t_color[1];
		lightStruct.b = (float)t_color[2];

		FbxDouble t_intensity = light->Intensity.Get();
		lightStruct.intensity = (float)t_intensity / 100.0f;

		FbxDouble3 t_pos = lightNode->LclTranslation.Get();

		

		FbxDouble3 t_rot = lightNode->LclRotation.Get();

		

		//Cutoff Angle
		FbxDouble t_angle = light->InnerAngle.Get();
		
		t_angle = FBXSDK_DEG_TO_RAD * t_angle;

		lightStruct.fov = (float)t_angle;
		auto eulerRot = lightNode->LclRotation.Get();
		
		if (shouldSwitchAxisSystem)
		{
			lightStruct.posX = (float)t_pos.mData[0];
			lightStruct.posY = (float)t_pos.mData[1];
			lightStruct.posZ = -(float)t_pos.mData[2];
			
			FbxAMatrix rotM;
			rotM.SetIdentity();
			rotM.SetR({ eulerRot }, FbxEuler::EOrder::eOrderXYZ);

			forward = rotM.MultT(forward);
			lightStruct.dirX = (float)forward.mData[0];
			lightStruct.dirY = (float)forward.mData[1];
			lightStruct.dirZ = -(float)forward.mData[2];
		}
		else
		{
			FbxAMatrix rotM;
			rotM.SetIdentity();
			rotM.SetR({ eulerRot }, FbxEuler::EOrder::eOrderXYZ);

			forward = rotM.MultT(forward);
			lightStruct.posX = (float)t_pos.mData[0];
			lightStruct.posY = (float)t_pos.mData[1];
			lightStruct.posZ = (float)t_pos.mData[2];
			lightStruct.dirX = (float)forward.mData[0];
			lightStruct.dirY = (float)forward.mData[1];
			lightStruct.dirZ = (float)forward.mData[2];
			
		}


		m_lights.push_back(lightStruct);
		return;
	}
	else 
	{
		return;
	}






}

void Scene::setOutPath(char* path)
{
	m_outPath = path;
}



void Scene::write()
{
	if (m_targets.size() > 0)
	{
		m_animationlength = m_mesh->getAniLength();
	}
	Output output;
	output.setOutPath(m_outPath);
	auto meshFlag = m_options.fileOptions;
	if ((int)(meshFlag & Options::FileOptions::MESH) != 0)
	{
		
		output.writeMesh(m_meshes, m_vertices, m_indices);
		
	}
	if ((int)(meshFlag & Options::FileOptions::MORPH) != 0)
	{

		output.writeBlendShapes(m_targets,m_morphVertices,m_morphIndices,m_morphKeyframes, m_animationlength, m_fps);

	}
	if ((int)(meshFlag & Options::FileOptions::SKELETON) != 0)
	{
		output.writeSkeleton(m_skeleton);
	}
	if ((int)(meshFlag & Options::FileOptions::MATERIAL) != 0)
	{

		output.writeMaterial(m_materials);

	}
	if ((int)(meshFlag & Options::FileOptions::LIGHT) != 0)
	{
		output.writeLight(m_lights);
	}
}


