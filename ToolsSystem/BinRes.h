#pragma once


// TO-DO ver uso com WTL::CResource

class CBinRes
{
public:
	CBinRes( UINT nIDRes, PCTSTR type = L"BIN" );
	~CBinRes();

// Interface
public:
	void BufferResource( CAtlArray<BYTE>& buff );
	bool SaveResource( PCTSTR path );
	UINT Size() { return m_size; }

// Data members
private:
	UINT m_size;
	PVOID m_resAdrr;
};
