////////////////////////////////////////
// CThiscallThunk.h:	Header file for the CThiscallThunk class
//
// Author:			JAB
// Date:			December 2000
//
// Purpose:			Allows you to use a C++ member function (non-static) to act
//					as a CALLBACK function for a WIN32 API callback (e.g. as a WNDPROC,
//					DLGPROC, TIMERPROC etc.). The C++ member function will receive the
//					correct 'this' pointer.
//
// Usage:			1) Instantiate an object of this class, e.g. m_cThunk, usually as a
//					member variable within your class, e.g. CMyClass.
//					2) Write a member function for CMyClass which has the same return
//					type and parameter list as the WIN32 API CALLBACK function you want
//					to use, e.g., in the case of a WNDPROC:
//
//						LRESULT CMyClass::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//  
//					Since this is a member function it will also be able to use the 'this'
//					pointer, explicitly or implicitly.
//					3) Use the call to setup the thunk which will be called
//					by WIN32 and will in turn call your member function:
//					
//						m_cThunk.Setup(this, WindowProc);
//
//					(the second parameter of this call will not be type-checked by the compiler so
//					please be careful to get it right!)
//
//					This step can even be done in your constructor (CMyClass::CMyClass), even in the
//					case of thunking to a virtual function. You might
//					think that this would not work, since the vtable is not initialised at the
//					time the constructor runs, therefore you might expect an invalid function pointer.
//					However, when you take the address of a virtual function, the compiler actually
//					gives you the address of a compiler-generated thunk which dynamically performs
//					the vtable look-up at the time the function invocation is done. So as long as
//					you don't actually try to *call* the thunked function from your constructor,
//					you can happily set up the thunk there.
//					It is quite OK for the thunked function
//					to be implemented in a class *derived* from the class which performs the
//					thunk setup - see CThread.cpp for an example.
//					4) The method CThiscallThunk::operator &() allows you to get the address of
//					the thunk. Cast this to the appropriate function pointer type which you will
//					need to pass to the WIN32 API, e.g. in our WNDPROC example:
//						m_pOldWndProc = (WNDPROC)::SetWindowLong(m_hWnd, GWL_WNDPROC,
//								(LONG) &m_cThunk);
//					(well actually in this case we cast it to a LONG not a WNDPROC, but for
//					the case of a timer you would cast it to a TIMERPROC and use it as the
//					fourth parameter of a ::SetTimer() call)
//					5) Note that the member function should not be static and should not use
//					VARARGS (",..." in the parameter list). CThiscallThunk was not designed for
//					these types of functions and may or may not work in this case.
//					6) If the callback is coming from a 'C'/'C++' package rather than a Windows
//					component it is likely to use the default (for non-class-member-functions)
//					calling convention "__cdecl", rather than __stdcall which is the norm
//					for Windows API related callbacks. In this case the member function in your
//					class must be defined as "__cdecl" and you should use the variant class
//					CThiscallThunkCdecl for your thunk. Note, however, that the thunk generated
//					by CThiscallThunkCdecl is not re-entrant - your callback function must
//					not directly or indirectly call itself through the same thunk again.
//
////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CThiscallThunk class

// Don't want a gap between the thunk's instructions due to alignment
#pragma pack(push,1)

class CThiscallThunk
{
// Structures, enumerations, typedefs etc.
public:

private:
	struct SThunk
	{
		BYTE bMovEcx;
		DWORD dwThis;
		BYTE bJmp;
		DWORD pProc;
	};

// Construction
public:
	CThiscallThunk();

// Attributes
public:
	void *operator &() const {return (void *) &m_sThunk;}

// Operations
public:
	// Template setup function to be called by our user
	//		pThat_: "this" pointer of the object to receive the callback
	//		pFunc_: (implied second parameter) address of the member function of that object which
	//				will handle the callback. Use of '...' avoids the user having to do an ugly type-cast
	//				on the member function name, though admittedly it's a little un-type-safe :-(
	// Example of usage:
	//		m_cThunk.Setup(this, MyCallbackProc)
	//				where MyCallbackProc is a non-static member function of the class. 
	template <class T>
	void Setup(T *pThat_, ...)
	{
		// This class is totally dependent on the compiler's function calling code generation
		// anyway so I won't bother to use proper varargs here...
		void *pFunc = *(void **)(&pThat_ + 1);	// pick up the second parameter
		// Call underlying implementation
		_Setup(pThat_, pFunc);
	}

// Overrides
protected:

// Implementation: methods
public:
	~CThiscallThunk();
private:
	// The underlying setup function
	void _Setup(void *pThat_, void *pFunc_);

// Implementation: data members
protected:
private:
	SThunk m_sThunk;
};

#pragma pack(pop)

/////////////////////////////////////////////////////////////////////////////
// CThiscallThunkCdecl class

// Don't want a gap between the thunk's instructions due to alignment
#pragma pack(push,1)

class CThiscallThunkCdecl
{
// Structures, enumerations, typedefs etc.
public:

private:
	struct SThunk
	{
		WORD wPop;
		DWORD dwSaveAddr;
		BYTE bPush;
		DWORD dwThis;
		BYTE bCall;
		DWORD pProc;
		WORD wAddEsp;
		BYTE bFour;
		WORD wJmp;
		DWORD dwSaveAddr2;
	};

// Construction
public:
	CThiscallThunkCdecl();

// Attributes
public:
	void *operator &() const {return (void *) &m_sThunk;}

// Operations
public:
	// Template setup function to be called by our user
	//		pThat_: "this" pointer of the object to receive the callback
	//		pFunc_: (implied second parameter) address of the member function of that object which
	//				will handle the callback. Use of '...' avoids the user having to do an ugly type-cast
	//				on the member function name, though admittedly it's a little un-type-safe :-(
	// Example of usage:
	//		m_cThunk.Setup(this, MyCallbackProc)
	//				where MyCallbackProc is a non-static member function of the class. 
	template <class T>
	void Setup(T *pThat_, ...)
	{
		// This class is totally dependent on the compiler's function calling code generation
		// anyway so I won't bother to use proper varargs here...
		void *pFunc = *(void **)(&pThat_ + 1);	// pick up the second parameter
		// Call underlying implementation
		_Setup(pThat_, pFunc);
	}

// Overrides
protected:

// Implementation: methods
public:
	~CThiscallThunkCdecl();
private:
	// The underlying setup function
	void _Setup(void *pThat_, void *pFunc_);

// Implementation: data members
protected:
private:
	SThunk m_sThunk;
	DWORD m_dwSaveAddr;					// used by thunk code to store the return address
};

#pragma pack(pop)

