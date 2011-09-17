#include "stdafx.h"
#include "ControlAdjust.h"


void OffsetControl( CWindow wndmove, int xff, int yff )
{
	CRect rect;
	wndmove.GetWindowRect( rect );
	wndmove.GetParent().ScreenToClient( rect );
	rect.OffsetRect( xff, yff );
	wndmove.MoveWindow( rect, FALSE );
}

void SizeControl( CWindow wndsize, int lff, int tff, int rff, int bff )
{
	CRect rect;
	wndsize.GetWindowRect( rect );
	wndsize.GetParent().ScreenToClient( rect );
	rect.DeflateRect( lff, tff, rff, bff );
	wndsize.MoveWindow( rect, FALSE );
}

void DimensionControl( CWindow wnddim, int dimx, int dimy )
{
	CRect rect;
	wnddim.GetWindowRect( rect );
	wnddim.GetParent().ScreenToClient( rect );
	wnddim.MoveWindow( CRect(rect.TopLeft(), CSize(dimx,dimy)), FALSE );
}

void ExpandControl( CWindow wndexp, int right, int bottom )
{
	CRect rect;
	wndexp.GetClientRect( &rect );
	wndexp.SetWindowPos( NULL, 0, 0, rect.right+right, rect.bottom+bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
}
