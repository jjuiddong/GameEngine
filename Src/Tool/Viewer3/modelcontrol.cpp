
#include "stdafx.h"
#include "modelcontrol.h"

using namespace graphic;


cModelControl::cModelControl()
{
}

cModelControl::~cModelControl()
{
}


void cModelControl::Update(const float deltaSeconds)
{

}


void cModelControl::Render(cRenderer &renderer, const float deltaSeconds)
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
	if (ImGui::CollapsingHeader("Model Information"))
	{
		ImGui::Text("FileName : %s", g_root.m_model.m_fileName.GetFileName().c_str());
		ImGui::Text("Path : %s", g_root.m_model.m_fileName.GetFilePathExceptFileName().c_str());
		ImGui::Spacing();

		if (ImGui::TreeNode("Mesh List"))
		{
			for (u_int i = 0; i < g_root.m_model.m_meshes.size(); ++i)
			{
				cMesh2 *mesh = g_root.m_model.m_meshes[i];
				if (ImGui::TreeNode(mesh->m_name.c_str()))
				{
					ImGui::Text("Vertex Count : %d", mesh->m_buffers->GetVertexBuffer().GetVertexCount());
					ImGui::Text("Face Count : %d", mesh->m_buffers->GetIndexBuffer().GetFaceCount());

					if (ImGui::TreeNode("Material"))
					{
						for (u_int k = 0; k < mesh->m_mtrls.size(); ++k)
						{
							cMaterial &mtrl = mesh->m_mtrls[k];
							const float speed = 0.01f;
							const float _min = 0;
							const float _max = 1.f;
							ImGui::DragFloat4("Diffuse", (float*)&mtrl.m_mtrl.Diffuse, speed, _min, _max);
							ImGui::DragFloat4("Ambient", (float*)&mtrl.m_mtrl.Ambient, speed, _min, _max);
							ImGui::DragFloat4("Specular", (float*)&mtrl.m_mtrl.Specular, speed, _min, _max);
							ImGui::DragFloat4("Emissive", (float*)&mtrl.m_mtrl.Emissive, speed, _min, _max);
						}
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("ColorMap"))
					{
						for (u_int k = 0; k < mesh->m_colorMap.size(); ++k)
						{
							cTexture *tex = mesh->m_colorMap[k];
							ImGui::Text(tex->m_fileName.c_str());
						}
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("NormalMap"))
					{
						for (u_int k = 0; k < mesh->m_normalMap.size(); ++k)
						{
							cTexture *tex = mesh->m_normalMap[k];
							ImGui::Text(tex->m_fileName.c_str());
						}
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("SpecularMap"))
					{
						for (u_int k = 0; k < mesh->m_specularMap.size(); ++k)
						{
							cTexture *tex = mesh->m_specularMap[k];
							ImGui::Text(tex->m_fileName.c_str());
						}
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("SelfIllumMap"))
					{
						for (u_int k = 0; k < mesh->m_selfIllumMap.size(); ++k)
						{
							cTexture *tex = mesh->m_selfIllumMap[k];
							ImGui::Text(tex->m_fileName.c_str());
						}
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}
}


void cModelControl::Clear()
{

}
