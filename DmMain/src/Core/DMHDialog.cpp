#include "DmMainAfx.h"
#include "DMHDialog.h"

namespace DM
{
	BEGIN_MSG_MAP(DMHDialog)
		MSG_WM_KEYDOWN(OnKeyDown)
	    MSG_WM_CLOSE(OnCancel)
		CHAIN_MSG_MAP(DMHWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	BEGIN_EVENT_MAP(DMHDialog)
		EVENT_ID_COMMAND(IDOK,OnOK)
		EVENT_ID_COMMAND(IDCANCEL,OnCancel)
	END_EVENT_MAP()

	DMHDialog::DMHDialog()
	{
		m_nRetCode = IDOK;
	}

	DMHDialog::~DMHDialog()
	{
	}

	INT_PTR DMHDialog::DoModal(LPCWSTR lpszXmlId, HWND hParent/*=NULL*/, bool bShadow/*=false*/, DM::CRect rect/* = NULL*/)
	{
		BOOL bEnableParent = FALSE;
		if(NULL == hParent)
		{
			hParent = g_pDMThreadActiveWndTool->GetActiveWnd();
		}
		
		if (hParent && hParent != ::GetDesktopWindow() && ::IsWindowEnabled(hParent))
		{
			::EnableWindow(hParent, FALSE);
			bEnableParent = TRUE;
		}

		if(!DM_CreateWindow(lpszXmlId, rect.left,rect.top,rect.Width(),rect.Height(), hParent, bShadow))
		{
			::EnableWindow(hParent, TRUE);
			return 0; // �˴�ʧ����
		}

		GetClientRect(rect);
		if (!rect.IsRectEmpty())
		{
			CenterWindow();
		}

		SendMessage(WM_INITDIALOG);//����init��Ϣ

		if(GetExStyle()&WS_EX_TOOLWINDOW)
		{
			::ShowWindow(m_hWnd,SW_SHOWNOACTIVATE);
		}
		else
		{
			::ShowWindow(m_hWnd,SW_SHOWNORMAL);
		}

		/*
		�����װqq��ʵ��¡��˳����Ѵ��ڡ��޷����ֵ�����---��������yggui�г���
		HWND_NOTOPMOST���������������зǶ��㴰��֮�ϣ��������ж��㴰��֮�󣩡���������Ѿ��ǷǶ��㴰����ñ�־�������á�
		HWND_TOP:����������Z��Ķ�����
		*/
		::SetWindowPos(m_hWnd, /*HWND_TOP*/HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	    g_pDMApp->Run(m_hWnd,true);// ���ڲ��ѷ�װ��ActiveWnd�л���DestoryWindow

		if (bEnableParent)
		{
			::EnableWindow(hParent, TRUE);
		}

		return m_nRetCode;
	}

	void DMHDialog::EndDialog( INT_PTR nResult )
	{
		m_nRetCode = nResult;
		PostMessage(WM_QUIT);
	} 

	// Protect
	DMCode DMHDialog::OnOK()
	{
		EndDialog(IDOK);
		return DM_ECODE_OK;
	}

	DMCode DMHDialog::OnCancel()
	{
		EndDialog(IDCANCEL);
		return DM_ECODE_OK;
	}

	void DMHDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		do 
		{
			DMHWnd::OnKeyEvent(WM_KEYDOWN,(WPARAM)nChar,MAKELPARAM(nRepCnt,nFlags));
			if (DMHWnd::IsMsgHandled())
			{
				break;
			}
			if (VK_ESCAPE!=nChar&&VK_RETURN!=nChar)
			{
				break;
			}
			int iId = VK_ESCAPE==nChar?IDCANCEL:IDOK;
			DUIWindow* pExitWnd = FindChildById(iId);
			if (pExitWnd)
			{
				DMEventCmdArgs Evt(pExitWnd);
				pExitWnd->DV_FireEvent(Evt);
			}
		} while (false);
	}

}// namespace DM