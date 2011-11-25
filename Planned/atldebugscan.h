#if !defined(AFX_ATLDEBUGSCAN_H__5537FE56_D493_11D3_80E3_00C04F0F05BA__INCLUDED_)
#define AFX_ATLDEBUGSCAN_H__5537FE56_D493_11D3_80E3_00C04F0F05BA__INCLUDED_

//////////////////////////////////////////////////////////////////////
// atldebugscan.h - debug COM interface enumerator
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2000 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#pragma once


// To activate memory leak detection at CRT level:
//  Call the following function at the very beginnning of the excutable function (e.g. _tWinMain)
//    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
//  or place this function at the very end
//    _CrtDumpMemoryLeaks(); 
#ifdef _DEBUG
  #define _CRTDBG_MAP_ALLOC 
  #include <stdlib.h> 
  #include <crtdbg.h> 
  #define new new( _NORMAL_BLOCK, __FILE__, __LINE__) 
  #define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__,__LINE__)  
#endif // _DEBUG 


ATLINLINE void AtlPrintInterfaceName(REFIID riid)
{
   HKEY hkey;
   OLECHAR szName[128];
   *szName = OLECHAR('\0');

   // Open the Interface (IID) key
   LONG r = ::RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("Interface"), 0, KEY_QUERY_VALUE, &hkey);
   if( r == ERROR_SUCCESS ) {
      OLECHAR szGuid[64];
      ::StringFromGUID2(riid, szGuid, sizeof(szGuid)/sizeof(OLECHAR));

      long cb = 127;
      r = ::RegQueryValueW(hkey, szGuid, szName, &cb);
      if( r == ERROR_SUCCESS )
         ATLTRACE("  %ws\n", szName);
      else
         ATLTRACE("  %ws\n", szGuid);
      ::RegCloseKey(hkey);
   }
};

inline void AtlTraceScanInterface(LPCSTR szName, IUnknown *pUnk )
{
   IUnknown *punkIf = NULL;
   HRESULT Hr;
   HKEY hkey;

   if( pUnk==NULL ) return;

   ATLTRACE("Interface scan: %s\n", szName);

   // Open the Interface (IID) key for scanning
   LONG r = ::RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("Interface"),
                            0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hkey);
   if( r == ERROR_SUCCESS ) {
      OLECHAR szGuid[128];
      DWORD index = 0;

      // Get each subkey
      while( ERROR_SUCCESS == ::RegEnumKeyW(hkey, index, szGuid, sizeof(szGuid)) ) {
         IID iid;
         ::IIDFromString(szGuid, &iid);
         // Test the IID and append to array if supported
         Hr = pUnk->QueryInterface(iid, (LPVOID*) &punkIf);
         if( SUCCEEDED(Hr) ) {
            punkIf->Release();
            AtlPrintInterfaceName(iid);
         }
         index++;
      }
      ::RegCloseKey(hkey);
   }
   // Scan some default stuff
   for( int i=0; i<255; i++ ) {
      OLECHAR id[64];
      ::wsprintfW(id, L"{000214%.2X-0000-0000-C000-000000000046}", i);
      IID iid;
      ::IIDFromString(id, &iid);
      Hr = pUnk->QueryInterface(iid, (LPVOID*) &punkIf);
      if( SUCCEEDED(Hr) ) {
         punkIf->Release();
         AtlPrintInterfaceName(iid);
      };
   }
};

#ifdef _DEBUG
#define ATLTRACE_INTERFACES(x) AtlTraceScanInterface(#x,x)
#else
#define ATLTRACE_INTERFACES(x)
#endif

#endif // !defined(AFX_ATLDEBUGSCAN_H__5537FE56_D493_11D3_80E3_00C04F0F05BA__INCLUDED_)
