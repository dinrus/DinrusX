// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/RendElements/AbstractMeshElement.h>
#include <DinrusX/DinrusXRend/D3DRender/DriverD3D.h>
#include <DinrusX/CoreX/Renderer/VertexFormats.h>

AbstractMeshElement::~AbstractMeshElement()
{
	if (m_vertexBuffer != ~0u) gcpRendD3D->m_DevBufMan.Destroy(m_vertexBuffer);
	if (m_indexBuffer  != ~0u) gcpRendD3D->m_DevBufMan.Destroy(m_indexBuffer);
}

void AbstractMeshElement::ValidateMesh()
{
	if (m_meshDirty)
	{
		GenMesh();

		if (m_vertexBuffer != ~0u)
		{
			gcpRendD3D->m_DevBufMan.Destroy(m_vertexBuffer);
			m_vertexBuffer = ~0u;
		}
		if (m_indexBuffer != ~0u)
		{
			gcpRendD3D->m_DevBufMan.Destroy(m_indexBuffer);
			m_indexBuffer = ~0u;
		}

		if (!m_vertices.empty())
		{
			m_vertexBuffer = gcpRendD3D->m_DevBufMan.Create(BBT_VERTEX_BUFFER, BU_STATIC, m_vertices.size() * sizeof(SVF_P3F_C4B_T2F));
			gcpRendD3D->m_DevBufMan.UpdateBuffer(m_vertexBuffer, &m_vertices[0], m_vertices.size() * sizeof(SVF_P3F_C4B_T2F));
		}

		if (!m_indices.empty())
		{
			m_indexBuffer = gcpRendD3D->m_DevBufMan.Create(BBT_INDEX_BUFFER, BU_STATIC, m_indices.size() * sizeof(uint16));
			gcpRendD3D->m_DevBufMan.UpdateBuffer(m_indexBuffer, &m_indices[0], m_indices.size() * sizeof(uint16));
		}

		m_meshDirty = false;
	}
}

int AbstractMeshElement::GetMeshDataSize() const
{
	return m_vertices.size() * sizeof(SVF_P3F_C4B_T2F) + m_indices.size() * sizeof(uint16);
}

