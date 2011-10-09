#include "stdafx.h"
#include "BinRes.h"

#include "ToolsSystem\WtlFile.h"


CBinRes::CBinRes( UINT nIDRes, PCTSTR type )
{
	DWORD dwSizeRes;
	HGLOBAL hResLoaded; //not a real HGLOBAL as stated in LoadResource() documentation
	HRSRC hRsrc;
	
	hRsrc = ::FindResource( _Module.GetResourceInstance(),
							MAKEINTRESOURCE(nIDRes), type );
	ATLVERIFY( hRsrc );

	hResLoaded = ::LoadResource( _Module.GetResourceInstance(), hRsrc );
	dwSizeRes = ::SizeofResource( _Module.GetResourceInstance(), hRsrc );
	ATLVERIFY( hResLoaded && m_size );

	m_resAdrr = ::LockResource( hResLoaded );
	m_size = dwSizeRes;
	ATLVERIFY( m_resAdrr );
}

CBinRes::~CBinRes()
{
	// FreeResource don't need to be called
}

void CBinRes::BufferResource( CAtlArray<BYTE>& buff )
{
	buff.SetCount( m_size );
	memcpy( buff.GetData(), m_resAdrr, m_size );
}

bool CBinRes::SaveResource( PCTSTR path )
{
	bool bOK;
	for(;;)
	{
		CWtlFile file;
		bOK = file.Open( path, CWtlFile::modeWrite | CWtlFile::shareDenyWrite | CWtlFile::modeCreate );
		if( !bOK )
			break;

		bOK = file.Write( m_resAdrr, m_size );
		break;
	}
	return bOK;
}