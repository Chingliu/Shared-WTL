#pragma once


class CReadStream
{
public:
	CReadStream();

// Initializers
public:
	CAtlArray<BYTE>& BufferStream( UINT size );
	inline void Free() { m_databuff.SetCount(0); };
	operator const CAtlArray<BYTE>&() { ASSERT(m_databuff.GetCount()>0); return m_databuff; }

// Data members
private:
	class : public CAtlArray<BYTE>
	{
	public:
		inline void ShrinkData() { m_nSize = 0; }// wont call destructors, safe for BYTEs
	} m_databuff;

	UINT m_buffsize;
	UINT m_seek;
	BYTE* m_readPos;
	UINT m_spot;

// Function members
private:
	bool ReadSeek( int );

// Interface
public:
	void DynamicSpotCopy();
	void DynamicEndCopy( CAtlArray<BYTE>& dynbuff ); //not responsible for the allocation
	bool BufferUTF16LE2Text(CStringW& refstr);

// Read Interface
public:
	// template guessed params
	template<typename T> bool Read( T* buff )
	{
		if( !ReadSeek(sizeof(T)) )
			return false;
		memcpy( buff, m_readPos, sizeof(T) );
		return true;
	}

	// normal methods
	void Skip( UINT );
	bool Read( void*, UINT );
	bool ReadSizedString( CString& );
	bool ReadLine( CString& );

	CReadStream& operator>>( DWORD& );
	CReadStream& operator>>( UINT& );
	CReadStream& operator>>( CString& );
	CReadStream& operator>>( CStringA& );
};
