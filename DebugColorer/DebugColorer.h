#pragma once

#include "resource.h"
#include "XColorSpectrumCtrl.h"


class CDebugColorer
	: public CDialogImpl<CDebugColorer>
{
public:
	CDebugColorer();
	enum { IDD = IDD_DBGCOLORER };

// Interface
public:
	static void ShowColorDialog( CWindow wndrepaint, BOOL behind=FALSE );
	static void SelectClrref( COLORREF* refcolor );
	static void SelectBrush( CBrush* refbrush );
	static void SelectPen( CPen* refpen );

// Data members
private:
	static CDebugColorer* m_dynaDlg;
	CWindow m_repaint;

	COLORREF* m_refcolor;
	CBrush* m_refbrush;
	CPen* m_refpen;

// Components
public:
	CXColorSpectrumCtrl m_ColorSpectrum;
	CContainedWindow m_ControlRedir;

// Maps
public:
	BEGIN_MSG_MAP(CDebugColorer)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(WM_XCOLORPICKER_SELCHANGE, OnColor)
	ALT_MSG_MAP(1)
		MSG_WM_SETFOCUS(OnSetFocus)
	END_MSG_MAP()

// Handler prototypes
public:
	LRESULT OnInitDialog(HWND hwndFocus, LPARAM lParam);
	LRESULT OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSetFocus(HWND hwndOldFocus);
};
