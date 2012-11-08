#pragma once


// based on: http://bobmoore.mvps.org/Win32/w32tip7.htm

class CSingleInstance
{
public:
	CSingleInstance();
	bool IsExistingInstanceOf( PCTSTR szName );

private:
	void CreateNullDaclSd();
	void FreeNullDaclSd();

	SECURITY_ATTRIBUTES m_sa;
	HANDLE m_hsa;
};
