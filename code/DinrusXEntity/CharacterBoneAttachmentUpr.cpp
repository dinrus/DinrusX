// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXEntity/stdafx.h>
#include <DinrusX/DinrusXEntity/CharacterBoneAttachmentUpr.h>
#include <DinrusX/DinrusXEntity/Entity.h>

#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>

void CCharacterBoneAttachmentUpr::Update()
{
	for (VectorMap<SBinding, SAttachmentData>::iterator iter = m_attachments.begin(); iter != m_attachments.end(); ++iter)
	{
		iter->first.pChild->InvalidateTM(ENTITY_XFORM_FROM_PARENT);
	}
}

void CCharacterBoneAttachmentUpr::RegisterAttachment(CEntity* pChild, CEntity* pParent, const uint32 targetCRC)
{
	const ICharacterInstance* pCharacterInstance = pParent->GetCharacter(0);

	if (pCharacterInstance)
	{
		const IDefaultSkeleton& defaultSkeleton = pCharacterInstance->GetIDefaultSkeleton();
		const uint32 numJoints = defaultSkeleton.GetJointCount();

		for (uint i = 0; i < numJoints; ++i)
		{
			const uint32 jointCRC = defaultSkeleton.GetJointCRC32ByID(i);
			if (jointCRC == targetCRC)
			{
				SBinding binding;
				binding.pChild = pChild;
				binding.pParent = pParent;

				SAttachmentData attachment;
				attachment.m_boneIndex = i;

				m_attachments[binding] = attachment;
				break;
			}
		}
	}
}

void CCharacterBoneAttachmentUpr::UnregisterAttachment(const CEntity* pChild, const CEntity* pParent)
{
	SBinding binding;
	binding.pChild = const_cast<CEntity*>(pChild);
	binding.pParent = const_cast<CEntity*>(pParent);

	m_attachments.erase(binding);
}

Matrix34 CCharacterBoneAttachmentUpr::GetNodeWorldTM(const CEntity* pChild, const CEntity* pParent) const
{
	SBinding binding;
	binding.pChild = const_cast<CEntity*>(pChild);
	binding.pParent = const_cast<CEntity*>(pParent);

	VectorMap<SBinding, SAttachmentData>::const_iterator findIter = m_attachments.find(binding);

	if (findIter != m_attachments.end())
	{
		const SAttachmentData data = findIter->second;
		const ICharacterInstance* pCharacterInstance = binding.pParent->GetCharacter(0);
		if (pCharacterInstance)
		{
			const ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
			const QuatT jointTransform = pSkeletonPose->GetAbsJointByID(data.m_boneIndex);
			return pParent->GetWorldTM() * Matrix34(jointTransform);
		}
	}

	return pParent->GetWorldTM();
}

bool CCharacterBoneAttachmentUpr::IsAttachmentValid(const CEntity* pChild, const CEntity* pParent) const
{
	SBinding binding;
	binding.pChild = const_cast<CEntity*>(pChild);
	binding.pParent = const_cast<CEntity*>(pParent);

	VectorMap<SBinding, SAttachmentData>::const_iterator findIter = m_attachments.find(binding);
	return findIter != m_attachments.end();
}
