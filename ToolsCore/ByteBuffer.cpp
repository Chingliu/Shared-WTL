// ByteBuffer.cpp

#include "stdafx.h"
#include "ByteBuffer.h"


CByteBuffer::CByteBuffer()
	: m_mem(nullptr), m_size(0)
{
}
CByteBuffer::CByteBuffer(size_t size)
	: m_mem(nullptr), m_size(0)
{
	Allocate(size);
}
CByteBuffer::CByteBuffer(CByteBuffer&&)
	: m_mem(nullptr), m_size(0)
{
}
CByteBuffer::~CByteBuffer()
{
	if( m_mem )
		delete m_mem;
}

void CByteBuffer::Allocate(size_t size)
{
	ASSERT(m_mem==nullptr);
	m_mem = new BYTE[size];
	ENSURE(m_mem);
	m_size = size;
}
void CByteBuffer::Relocate(size_t newsize)
{
	if( newsize > m_size )
	{
		delete m_mem;
		m_mem = new BYTE[newsize];
		ENSURE(m_mem);
		m_size = newsize;
	}
}