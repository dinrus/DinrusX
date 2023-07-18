// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#pragma once

class CEntity;

#include <DinrusX/CoreX/Containers/VectorMap.h>

class CCharacterBoneAttachmentUpr
{
public:
	void     Update();

	void     RegisterAttachment(CEntity* pChild, CEntity* pParent, const uint32 targetCRC);
	void     UnregisterAttachment(const CEntity* pChild, const CEntity* pParent);

	Matrix34 GetNodeWorldTM(const CEntity* pChild, const CEntity* pParent) const;
	bool     IsAttachmentValid(const CEntity* pChild, const CEntity* pParent) const;

private:
	struct SBinding
	{
		CEntity* pChild;
		CEntity* pParent;

		bool operator<(const SBinding& rhs) const
		{
			return (pChild != rhs.pChild) ? (pChild < rhs.pChild) : (pParent < rhs.pParent);
		}
	};

	struct SAttachmentData
	{
		uint m_boneIndex;
	};

	VectorMap<SBinding, SAttachmentData> m_attachments;
};
