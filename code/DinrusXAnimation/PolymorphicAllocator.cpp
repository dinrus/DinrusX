// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAnimation/stdafx.h>
#include <DinrusX/DinrusXAnimation/PolymorphicAllocator.h>

#include <algorithm>
#include <utility>

#include <DinrusX/DinrusXAnimation/IAllocator.h>

namespace Memory
{

void* PolymorphicAllocator::Allocate(uint32 length)
{
	return m_pAllocator->Allocate(length);
}

void PolymorphicAllocator::Free(void* pAddress)
{
	return m_pAllocator->Free(pAddress);
}

size_t PolymorphicAllocator::GetGuaranteedAlignment() const
{
	return m_pAllocator->GetGuaranteedAlignment();
}

PolymorphicAllocator::PolymorphicAllocator(const PolymorphicAllocator& other)
	: m_pAllocator(other.m_pAllocator->Clone())
{
}

PolymorphicAllocator& PolymorphicAllocator::operator=(const PolymorphicAllocator& other)
{
	m_pAllocator = other.m_pAllocator->Clone();
	return *this;
}

void PolymorphicAllocator::Swap(PolymorphicAllocator& other)
{
	std::swap(m_pAllocator, other.m_pAllocator);
}

} // namespace Memory
