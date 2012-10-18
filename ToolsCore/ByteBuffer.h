// ByteBuffer.h

#pragma once


class CByteBuffer
{
public:
	CByteBuffer();
	CByteBuffer(size_t size);
	CByteBuffer(CByteBuffer&&);
	~CByteBuffer();

private:
	CByteBuffer(const CByteBuffer&);// copying?
	BYTE* m_mem;
	size_t m_size;

public:
	inline void Allocate(size_t size);
	inline void Relocate(size_t newsize);

	template<typename TCast> TCast* GetDataAs() { return reinterpret_cast<TCast*>(m_mem); }
	size_t GetSize() { return m_size; }

	BYTE* operator[](size_t iElement) const { return m_mem+iElement; }
	operator BYTE*() const { return m_mem; }
	operator void*() const { return m_mem; }
};
