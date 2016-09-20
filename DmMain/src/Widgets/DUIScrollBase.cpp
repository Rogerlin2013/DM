#include "DmMainAfx.h"
#include "DUIScrollBase.h"

namespace DM
{
	DUIScrollBase::DUIScrollBase()
	{
		m_bnowheelscroll        = false;
		m_isbWid				= -1;
		m_isbAllowSize          = -1;
		m_isbVisible            = DMSB_NULL;
		m_isbEnable             = DMSB_BOTH;

		m_bsbDrag               = false;

		memset(&m_siHoz,0,sizeof(SCROLLINFO));
		memset(&m_siVer,0,sizeof(SCROLLINFO));
		m_siHoz.nTrackPos		= -1;
		m_siVer.nTrackPos		= -1;

		m_dwUpdateInterval      = 0;

		m_pDUIXmlInfo->m_bOnlyDrawClient = true;/// �˴�һ��Ϊtrue,�������û���ֻ���ƿͻ���

		m_szRange.SetSize(-1,-1);
		m_ptCurPos.SetPoint(0, 0);
		m_bUseRange             = true;

		// scrollbar
		DMADDEVENT(DMEventScrollArgs::EventID);
	}

	//---------------------------------------------------
	// Function Des: ����ӿ�
	bool DUIScrollBase::GetScrollInfo(bool bVert, LPSCROLLINFO lpsi)
	{
		bool bRet = false;
		do 
		{
			if (NULL == lpsi)
			{
				break;
			}

			if (bVert)
			{
				memcpy(lpsi,&m_siVer,sizeof(SCROLLINFO));
			}
			else
			{
				memcpy(lpsi,&m_siHoz,sizeof(SCROLLINFO));
			}
			bVert = true;
		} while (false);
		return bRet;
	}

	bool DUIScrollBase::SetScrollInfo(bool bVert,LPSCROLLINFO lpsi, bool bRedraw)
	{
		bool bRet = false;
		do 
		{
			if (NULL == lpsi)
			{
				break;
			}

			if (bVert)
			{
				memcpy(&m_siVer,lpsi,sizeof(SCROLLINFO));
			}
			else
			{
				memcpy(&m_siHoz,lpsi,sizeof(SCROLLINFO));
			}

			DM_SendMessage(WM_NCCALCSIZE);// ���¼��������ͻ�����С
			if(bRedraw)
			{
				CRect rcSb = GetScrollBarRect(bVert);
				DM_InvalidateRect(rcSb);
			}
			bVert = true;
		} while (false);
		return bVert;
	}

	bool DUIScrollBase::ShowScrollBar(int wBar, bool bShow)
	{
		if (bShow) 
		{
			m_isbVisible|=wBar;
		}
		else 
		{
			m_isbVisible&=~wBar;
		}
		DM_SendMessage(WM_NCCALCSIZE);// ���¼��������ͻ�����С
		DM_InvalidateRect(m_rcWindow);
		return TRUE;
	}

	bool DUIScrollBase::EnableScrollBar(int wBar,bool bEnable)
	{
		if (bEnable)
		{
			m_isbEnable|=wBar;
		}
		else 
		{
			m_isbEnable&=~wBar;
		}
		if (wBar&DMSB_VERT)
		{
			CRect rcSb = GetScrollBarRect(true);
			DM_InvalidateRect(rcSb);// ˢ�¹�����
		}
		if (wBar&DMSB_HORZ)
		{
			CRect rcSb = GetScrollBarRect(false);
			DM_InvalidateRect(rcSb);
		}
		return TRUE;
	}

	bool DUIScrollBase::SetScrollPos(bool bVert, int nNewPos,bool bRedraw)
	{
		SCROLLINFO*psi = bVert?(&m_siVer):(&m_siHoz);

		//make sure nNewPos is valid.
		if (nNewPos<psi->nMin)
		{
			nNewPos=psi->nMin;
		}
		if (nNewPos>psi->nMax-(int)psi->nPage+1) 
		{
			nNewPos=psi->nMax-psi->nPage+1;
		}

		psi->nPos=nNewPos;

		if (bRedraw)
		{
			CRect rcSb = GetScrollBarRect(bVert);
			DM_InvalidateRect(rcSb);
		}

		OnScroll(bVert,SB_THUMBPOSITION,nNewPos);
		return TRUE;
	}

	bool DUIScrollBase::SetScrollRange( bool bVert,int nMinPos,int nMaxPos,bool bRedraw )
	{
		SCROLLINFO *psi = bVert?(&m_siVer):(&m_siHoz);
		psi->nMin       = nMinPos;
		psi->nMax       = nMaxPos;

		if (psi->nPos<psi->nMin) psi->nPos=psi->nMin;
		if (psi->nPos>psi->nMax - (int)psi->nPage+1) psi->nPos=psi->nMax-psi->nPage+1;

		if (bRedraw)
		{
			CRect rcSb = GetScrollBarRect(bVert);
			DM_InvalidateRect(rcSb);
		}
		return true;
	}

	bool DUIScrollBase::GetScrollRange( bool bVert, LPINT lpMinPos, LPINT lpMaxPos )
	{
		SCROLLINFO *psi   = bVert?(&m_siVer):(&m_siHoz);
		if (lpMaxPos) *lpMaxPos = psi->nMax;
		if (lpMinPos) *lpMinPos = psi->nMin;
		return true;
	}

	bool DUIScrollBase::IsScrollBarEnable(bool bVert)
	{
		bool bRet = false;
		if (m_isbEnable&(bVert?DMSB_VERT:DMSB_HORZ))
		{
			bRet = true;
		}

		return bRet;
	}

	void DUIScrollBase::SetScrollInfo(SCROLLINFO si,bool bVert)
	{
		SCROLLINFO* psi = bVert?(&m_siVer):(&m_siHoz);
		if (si.fMask&SIF_RANGE)
		{
			psi->nMin = si.nMin;
			psi->nMax = si.nMax;
		}
		if (si.fMask & SIF_PAGE) psi->nPage=si.nPage;
		if (si.fMask & SIF_POS)  psi->nPos=si.nPos;
		if (si.fMask & SIF_TRACKPOS) psi->nTrackPos=si.nTrackPos;
		if (si.nPage==0) psi->nPos=0;
		else
		{
			if (psi->nPos>(psi->nMax-(int)psi->nPage+1)) psi->nPos=(psi->nMax-psi->nPage+1);
			if (psi->nPos<psi->nMin) psi->nPos=psi->nMin;
		}
	}

	int DUIScrollBase::GetScrollPos(bool bVert)
	{
		return bVert?m_siVer.nPos:m_siHoz.nPos;
	}

	void DUIScrollBase::UpdateScrollBar()
	{
		CRect rcClient;
		DUIWindow::DV_GetClientRect(&rcClient);
		if (rcClient.IsRectEmpty())
		{
			m_isbVisible    = DMSB_NULL;		     /// �رչ�����
			return;
		}

		CSize size		= rcClient.Size();
		m_isbVisible    = DMSB_NULL;		     /// �رչ�����

		if (size.cy<m_szRange.cy				/// ��Ҫ��ֱ������
			|| (size.cx<m_szRange.cx&&size.cy<m_szRange.cy+m_isbWid))
		{
			m_isbVisible  |= DMSB_VERT;
			m_siVer.nMin   = 0;
			m_siVer.nMax   = m_szRange.cy-1;
			m_siVer.nPage  = size.cy;

			if (size.cx<m_szRange.cx+m_isbWid)/// ��Ҫˮƽ������
			{
				m_isbVisible	|= DMSB_HORZ;
				m_siVer.nPage    = size.cy-m_isbWid > 0 ? size.cy-m_isbWid : 0;

				m_siHoz.nMin     = 0;
				m_siHoz.nMax     = m_szRange.cx-1;
				m_siHoz.nPage    = size.cx-m_isbWid > 0 ? size.cx-m_isbWid : 0;
			}
			else
			{
				m_siHoz.nPage = size.cx;
				m_siHoz.nMin  = 0;
				m_siHoz.nMax  = m_siHoz.nPage-1;
				m_siHoz.nPos  = 0;
				m_ptCurPos.x     = m_siHoz.nPos;/// ��¼ˮƽ��nPos
			}
		}
		else
		{
			/// ����Ҫˮƽ������
			m_siVer.nPage = size.cy;
			m_siVer.nMin  = 0;
			m_siVer.nMax  = size.cy-1;
			m_siVer.nPos  = 0;
			m_ptCurPos.y     = m_siVer.nPos;

			if (size.cx<m_szRange.cx)
			{
				/// ��Ҫˮƽ������
				m_isbVisible  |= DMSB_HORZ;
				m_siHoz.nMin   = 0;
				m_siHoz.nMax   = m_szRange.cx-1;
				m_siHoz.nPage  = size.cx;
			}
			else/// ����Ҫˮƽ������
			{
				m_siHoz.nPage  = size.cx;
				m_siHoz.nMin   = 0;
				m_siHoz.nMax   = m_siHoz.nPage-1;
				m_siHoz.nPos   = 0;
				m_ptCurPos.x	   = m_siHoz.nPos;
			}
		}

		SetScrollPos(true,m_siVer.nPos,true);
		SetScrollPos(false,m_siHoz.nPos,false);

		DM_SendMessage(WM_NCCALCSIZE);   ///< ����ǿͻ�����С
		DM_InvalidateRect(m_rcWindow);
	}

	void DUIScrollBase::SetRangeSize(CSize szRange)
	{
		do 
		{
			if (szRange == m_szRange)
			{
				break;
			}

			CSize oldViewSize = m_szRange;
			m_szRange = szRange;
			UpdateScrollBar();
			OnRangeSizeChanged(oldViewSize,szRange);
		} while (false);
	}

	void DUIScrollBase::SetRangeCurPos(CPoint pt)
	{// �˴�������view�ı䶯
		do 
		{
			if (m_ptCurPos==pt)
			{
				break;
			}
			CPoint ptOld = m_ptCurPos;
			m_ptCurPos = pt;

			OnRangeCurPosChanged(ptOld,pt);
			DM_Invalidate();
		} while (false);
	}

	bool DUIScrollBase::OnScroll(bool bVert,UINT uCode,int nPos)
	{
		SCROLLINFO*psi = bVert?(&m_siVer):(&m_siHoz);
		int nNewPos = psi->nPos;
		switch (uCode)
		{
		case SB_LINEUP:
			{
				nNewPos -= GetScrollLineSize(bVert);
			}
			break;

		case SB_LINEDOWN:
			{
				nNewPos += GetScrollLineSize(bVert);
			}
			break;
		case SB_PAGEUP:
			{
				nNewPos -= psi->nPage;
			}
			break;
		case SB_PAGEDOWN:
			{
				nNewPos += psi->nPage;
			}
			break;

		case SB_THUMBTRACK:
			{
				nNewPos = nPos;
			}	
			break;
		case SB_THUMBPOSITION:
			{
				nNewPos = nPos;
			}
			break;
		}

		if (nNewPos<psi->nMin)
		{
			nNewPos=psi->nMin;
		}
		if (nNewPos>psi->nMax - (int)psi->nPage+1) 
		{
			nNewPos=psi->nMax-psi->nPage+1;
		}
		if (psi->nPage==0)
		{
			nNewPos=0;
		}

		if (nNewPos!=psi->nPos)
		{
			psi->nPos=nNewPos;
			if (uCode!=SB_THUMBTRACK)
			{
				CRect rcRail = GetScrollBarRect(bVert);
				if (bVert)
				{
					rcRail.DeflateRect(0,m_isbAllowSize);
				}
				else 
				{
					rcRail.DeflateRect(m_isbAllowSize,0);
				}
				if (m_psbSkin)
				{// ���ʱ���Ƶ��״̬�͹����鲿��
					DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rcRail,DMOLEDC_PAINTBKGND,FALSE);
					DrawScrollBar(pCanvas,rcRail,SB_PAGEDOWN,DMSBST_NORMAL,bVert);
					psi->nTrackPos = -1;
					CRect rcSlide = GetSbPartRect(bVert,SB_THUMBTRACK);
					DrawScrollBar(pCanvas,rcSlide,SB_THUMBTRACK,DMSBST_NORMAL,bVert);
					DM_ReleaseCanvas(pCanvas);
				}
			}
		}
		DM_InvalidateRect(m_rcWindow);

		/// -----------------------------
		if (true == m_bUseRange)
		{
			int nCurPos = GetScrollPos(bVert);
			CPoint ptPos = m_ptCurPos;
			if (bVert)
			{
				ptPos.y = nCurPos;
			}
			else
			{
				ptPos.x = nCurPos;
			}

			if (ptPos!=m_ptCurPos)
			{
				SetRangeCurPos(ptPos);
			}

			if (SB_THUMBTRACK == uCode)
			{
				ScrollUpdateWindow();
			}
		}

		return true;
	}

	void DUIScrollBase::OnRangeCurPosChanged(CPoint ptOld,CPoint ptNew) 
	{
		DV_UpdateChildLayout();
	}

	//---------------------------------------------------
	// Function Des: DUI����Ϣ�ַ�ϵ�к���
	int DUIScrollBase::OnCreate(LPVOID)
	{
		int iRet = 0;
		do 
		{
			if (-1 == m_isbWid)
			{
				if (m_psbSkin)
				{
					CSize size;
					m_psbSkin->GetStateSize(size);
					m_isbWid = size.cx/9; // ����9*��5=45��״̬
				}
			}

			if (-1 == m_isbAllowSize)
			{
				m_isbAllowSize = m_isbWid;
			}

			if (-1 == m_szRange.cx)
			{
				m_szRange.cx = m_rcsbClient.Width();
			}
			if (-1 == m_szRange.cy)
			{
				m_szRange.cy = m_rcsbClient.Height();
			}
			SetRangeCurPos(m_ptCurPos);
		} while (false);
		return iRet;
	}

	void DUIScrollBase::OnSize(UINT nType, CSize size)
	{
		__super::OnSize(nType,size);
		if (true == m_bUseRange)
		{
			UpdateScrollBar();
		}
	}

	void DUIScrollBase::DM_OnNcPaint(IDMCanvas *pCanvas)
	{
		do 
		{
			__super::DM_OnNcPaint(pCanvas);

			CRect rcDest;
			for (int i=0; i<2;i++)
			{// ���λ���
				bool bVert = (1==i);
				if (HasScrollBar(bVert))
				{
					int nState = (DM_IsDisable(bVert)||!IsScrollBarEnable(bVert))?DMSBST_DISABLE:DMSBST_NOACTIVE;
					rcDest = GetSbPartRect(bVert,SB_LINEUP);	
					if (WORD(-1) != m_sbInfo.sbCode&&bVert==m_sbInfo.bVert&&SB_LINEUP==m_sbInfo.sbCode)// ������ϼ�ͷ��
					{
						if (PUSH_LBUTTONDOWN)// ����״̬
						{
							DrawScrollBar(pCanvas, rcDest,SB_LINEUP,DMSBST_PUSHDOWN,bVert);
						}
						else
						{	
							DrawScrollBar(pCanvas,rcDest,SB_LINEUP,DMSBST_HOVER,bVert);
						}
					}
					else
					{
						DrawScrollBar(pCanvas,rcDest,SB_LINEUP,DMSBST_NOACTIVE,bVert);
					}

					rcDest = GetSbPartRect(bVert,SB_PAGEUP);		
					DrawScrollBar(pCanvas,rcDest,SB_PAGEUP,DMSBST_NOACTIVE,bVert); // �������ֵĲ�

					rcDest = GetSbPartRect(bVert,SB_THUMBTRACK);	
					DrawScrollBar(pCanvas,rcDest,SB_THUMBTRACK,m_bsbDrag?DMSBST_PUSHDOWN:DMSBST_NOACTIVE,bVert); // ��������

					rcDest = GetSbPartRect(bVert,SB_PAGEDOWN);		
					DrawScrollBar(pCanvas,rcDest,SB_PAGEDOWN,DMSBST_NOACTIVE,bVert);			// �������ֵĲ�

					rcDest = GetSbPartRect(bVert,SB_LINEDOWN);		
					if (WORD(-1) != m_sbInfo.sbCode&&bVert==m_sbInfo.bVert&&SB_LINEDOWN==m_sbInfo.sbCode)// ������¼�ͷ��
					{
						if (PUSH_LBUTTONDOWN)// ����״̬
						{
							DrawScrollBar(pCanvas,rcDest,SB_LINEDOWN,DMSBST_PUSHDOWN,bVert);
						}
						else
						{	
							DrawScrollBar(pCanvas,rcDest,SB_LINEDOWN,DMSBST_HOVER,bVert);
						}
					}
					else
					{
						DrawScrollBar(pCanvas,rcDest,SB_LINEDOWN,DMSBST_NOACTIVE,bVert);
					}
				}
			}
			if (HasScrollBar(true)&&HasScrollBar(false))// ��������������֮��ı߽�
			{
				CRect rcDest;
				__super::DV_GetClientRect(&rcDest);
				rcDest.left = rcDest.right-m_isbWid;
				rcDest.top  = rcDest.bottom-m_isbWid;
				DrawScrollBar(pCanvas,rcDest,SB_CORNOR,0,0);
			}
		} while (false);
	}

	void DUIScrollBase::OnNcLButtonDown(UINT nFlags, CPoint point)
	{
		do 
		{
			m_sbInfo = HitTest(point);
			if (WORD(-1) ==m_sbInfo.sbCode					  // ���㲻�ڹ�������
				||false == IsScrollBarEnable(m_sbInfo.bVert)) // ������Ϊ����״̬
			{
				break;
			}

			DM_SetCapture();  // ����Capture��׽������Ϣ��������갴�º���������������϶���
			if (SB_THUMBTRACK!=m_sbInfo.sbCode)
			{
				if (SB_LINEUP == m_sbInfo.sbCode||SB_LINEDOWN == m_sbInfo.sbCode)
				{// ����ڼ�ͷ����
					CRect rc = GetSbPartRect(m_sbInfo.bVert,m_sbInfo.sbCode);
					DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rc,DMOLEDC_PAINTBKGND,FALSE);
					DrawScrollBar(pCanvas,rc,m_sbInfo.sbCode,DMSBST_PUSHDOWN,m_sbInfo.bVert);
					DM_ReleaseCanvas(pCanvas);
				}
				OnScroll(m_sbInfo.bVert,m_sbInfo.sbCode,m_sbInfo.bVert?m_siVer.nPos:m_siHoz.nPos);
				DM_SetTimer(TIMER_SBWAIT,500);// ��������������ʱ��
			}
			else
			{// ����ڻ��鲿��
				m_bsbDrag  = true;
				m_sbDragPt = point;
				m_isbDragPos = m_sbInfo.bVert?m_siVer.nPos:m_siHoz.nPos;
				m_dwUpdateTime = GetTickCount()-m_dwUpdateInterval;// �õ�һ�ι�����Ϣ�ܹ���ʱˢ��

				CRect rcSlide = GetSbPartRect(m_sbInfo.bVert,SB_THUMBTRACK);
				DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rcSlide,DMOLEDC_PAINTBKGND,FALSE);
				DrawScrollBar(pCanvas,rcSlide,SB_THUMBTRACK,DMSBST_PUSHDOWN,m_sbInfo.bVert);
				DM_ReleaseCanvas(pCanvas);
			}	

		} while (false);
	}

	void DUIScrollBase::OnNcLButtonUp(UINT nFlags,CPoint pt)
	{
		do 
		{
			if (WORD(-1) ==m_sbInfo.sbCode                  // ���㲻�ڹ�������
				||false == IsScrollBarEnable(m_sbInfo.bVert))// ������Ϊ����״̬
			{
				break;
			}
			DM_ReleaseCapture();  // �ͷ�Capture
			if (m_bsbDrag)
			{
				m_bsbDrag = false;  
				SCROLLINFO* psi = m_sbInfo.bVert?(&m_siVer):(&m_siHoz);
				if (-1!=psi->nTrackPos)
				{
					OnScroll(m_sbInfo.bVert,SB_THUMBPOSITION,psi->nTrackPos);
				}

				CRect rcRail = GetScrollBarRect(m_sbInfo.bVert);// ��ȥ�߿�Ĵ�С
				if (m_sbInfo.bVert) 
				{
					rcRail.DeflateRect(0,m_isbAllowSize);
				}
				else 
				{
					rcRail.DeflateRect(m_isbAllowSize,0);
				}

				// �ָ�����״̬
				DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rcRail,DMOLEDC_PAINTBKGND,FALSE);
				DrawScrollBar(pCanvas,rcRail,SB_PAGEDOWN,DMSBST_NORMAL,m_sbInfo.bVert);
				psi->nTrackPos = -1;
				CRect rcSlide = GetSbPartRect(m_sbInfo.bVert,SB_THUMBTRACK);
				DrawScrollBar(pCanvas,rcSlide,SB_THUMBTRACK,DMSBST_NORMAL,m_sbInfo.bVert);
				DM_ReleaseCanvas(pCanvas);
			}
			else 
			{
				if (SB_LINEUP == m_sbInfo.sbCode||SB_LINEDOWN == m_sbInfo.sbCode)
				{// �ָ���ͷ״̬
					CRect rc = GetSbPartRect(m_sbInfo.bVert,m_sbInfo.sbCode);
					DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rc,DMOLEDC_PAINTBKGND,FALSE);
					DrawScrollBar(pCanvas,rc,m_sbInfo.sbCode,DMSBST_NORMAL,m_sbInfo.bVert);
					DM_ReleaseCanvas(pCanvas);
				}
			}

		}while (false);

		// ���
		DM_KillTimer(TIMER_SBWAIT);
		DM_KillTimer(TIMER_SBGO);
		m_sbInfo.sbCode = -1;   
		OnNcMouseMove(nFlags,pt);
	}

	void DUIScrollBase::OnNcMouseMove(UINT nFlags, CPoint point)
	{
		do 
		{
			// ---------1.�϶�״̬������ƶ�------------------------------------------------
			if (m_bsbDrag)
			{// �϶�����״̬
				if (SB_THUMBTRACK!=m_sbInfo.sbCode)
				{
					break;
				}
				CRect rcSb = GetScrollBarRect(m_sbInfo.bVert);
				CRect rcRail = rcSb;// ��ȥ����ͷ�Ĳ۳���
				if (m_sbInfo.bVert) 
				{
					rcRail.DeflateRect(0,m_isbAllowSize);
				}
				else 
				{
					rcRail.DeflateRect(m_isbAllowSize,0);
				}
				int nInterHei  = m_sbInfo.bVert?rcRail.Height():rcRail.Width();
				SCROLLINFO*psi = m_sbInfo.bVert?(&m_siVer):(&m_siHoz);
				int nSlideLen  = GetSbSlideLength(m_sbInfo.bVert);
				int nEmptyHei  = nInterHei-nSlideLen;
				int nDragLen   = m_sbInfo.bVert?(point.y-m_sbDragPt.y):(point.x-m_sbDragPt.x);// �Ϲ��ľ���
				int nSlide	   = (0==nEmptyHei)?0:(nDragLen*(int)(psi->nMax-psi->nMin-psi->nPage+1)/nEmptyHei);
				int nNewTrackPos = m_isbDragPos+nSlide;
				if (nNewTrackPos<psi->nMin)
				{
					nNewTrackPos = psi->nMin;
				}
				else if (nNewTrackPos>(int)(psi->nMax-psi->nMin-psi->nPage+1))
				{
					nNewTrackPos = psi->nMax-psi->nMin-psi->nPage+1;
				}
				CRect rcSlide = GetSbSlideRectByPos(m_sbInfo.bVert,m_isbDragPos);
				if (m_sbInfo.bVert)
				{
					if (nDragLen>0 && nDragLen>rcRail.bottom-rcSlide.bottom)
						nDragLen = rcRail.bottom-rcSlide.bottom;
					if (nDragLen<0 && nDragLen<rcRail.top-rcSlide.top)
						nDragLen = rcRail.top-rcSlide.top;
					rcSlide.OffsetRect(0,nDragLen);
				}
				else
				{
					if (nDragLen>0 && nDragLen>rcRail.right-rcSlide.right)
						nDragLen = rcRail.right-rcSlide.right;
					if(nDragLen<0 && nDragLen<rcRail.left-rcSlide.left)
						nDragLen = rcRail.left-rcSlide.left;
					rcSlide.OffsetRect(nDragLen,0);
				}

				DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rcRail,DMOLEDC_PAINTBKGND,FALSE);
				DrawScrollBar(pCanvas,rcRail,SB_PAGEUP,DMSBST_NORMAL,m_sbInfo.bVert);
				DrawScrollBar(pCanvas,rcSlide,SB_THUMBTRACK,DMSBST_PUSHDOWN,m_sbInfo.bVert);
				DM_ReleaseCanvas(pCanvas);

				if (nNewTrackPos!=psi->nTrackPos)
				{
					psi->nTrackPos = nNewTrackPos;
					OnScroll(m_sbInfo.bVert,SB_THUMBTRACK,psi->nTrackPos);
				}
				break;// ������
			}

			// ---------2.���϶�״̬������ƶ�------------------------------------------------
			SBSTATEINFO uHit = HitTest(point);
			SBSTATEINFO uHitOrig = m_sbInfo;// ���ݳ�������ֹ�������������޸�

			// ---------2.1���϶�״̬������뿪�˹�����------------
			if (WORD(-1) == uHit.sbCode) 
			{
				OnNcMouseLeave();
				break;// ������
			}

			// ---------2.2���϶�״̬�����δ�뿪��������ϸ��Ϊ����ǰ�������ϻ���һ�������ϣ�------------
			if (uHit != uHitOrig)
			{
				// ---------2.2.1���϶�״̬���������˵�ǰ������------------
				if (WORD(-1) == uHitOrig.sbCode|| uHitOrig.bVert != uHit.bVert)// ԭʼ�����������ڻ�ǰ��������ԭʼ����������ͬһ��
				{
					// ---------2.2.1���϶�״̬��ԭʼ������ʧЧ--------------
					if (WORD(-1) != uHitOrig.sbCode)// ����uHitOrig.bVert != uHit.bVertʱ
					{
						OnNcMouseLeave();//�л���������Դ������ʧ��
					}

					// ---------2.2.1���϶�״̬�»����¹�����״̬------------
					if (IsScrollBarEnable(uHit.bVert))
					{
						CRect rcDest  = GetScrollBarRect(uHit.bVert);
						DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rcDest,DMOLEDC_PAINTBKGND,FALSE);
						if (SB_LINEUP != uHit.sbCode) 
						{// ���ϼ�ͷ
							rcDest  = GetSbPartRect(uHit.bVert,SB_LINEUP);
							DrawScrollBar(pCanvas,rcDest,SB_LINEUP,DMSBST_NORMAL,uHit.bVert);
						}
						// �������ֵĲ�	
						rcDest = GetSbPartRect(uHit.bVert,SB_PAGEUP);		
						DrawScrollBar(pCanvas,rcDest,SB_PAGEUP,DMSBST_NORMAL,uHit.bVert);
						rcDest = GetSbPartRect(uHit.bVert,SB_PAGEDOWN);		
						DrawScrollBar(pCanvas,rcDest,SB_PAGEDOWN,DMSBST_NORMAL,uHit.bVert);
						if (SB_LINEDOWN != uHit.sbCode)
						{// ���¼�ͷ
							rcDest = GetSbPartRect(uHit.bVert,SB_LINEDOWN);
							DrawScrollBar(pCanvas,rcDest,SB_LINEDOWN,DMSBST_NORMAL,uHit.bVert);
						}
						if (SB_THUMBTRACK!=uHit.sbCode)
						{
							rcDest = GetSbPartRect(uHit.bVert,SB_THUMBTRACK);
							DrawScrollBar(pCanvas,rcDest,SB_THUMBTRACK,DMSBST_NORMAL,uHit.bVert);
						}

						// ǰ�涼�ǻָ�Ĭ��״̬��������ֱ�ӻ���hover״̬
						//if (SB_PAGEUP!=uHit.sbCode&&SB_PAGEDOWN!=uHit.sbCode)
						{
							rcDest = GetSbPartRect(uHit.bVert,uHit.sbCode);
							DrawScrollBar(pCanvas,rcDest,uHit.sbCode,DMSBST_HOVER,uHit.bVert);
						}
						DM_ReleaseCanvas(pCanvas);
					}
				}	
				else//��Ӧԭʼ���������ڣ��ҵ�ǰͬ��ͬһ��������ֻ��״̬��ͬ��ͣ��λ��ͬ��
				{
					if (IsScrollBarEnable(uHitOrig.bVert))// �Ȼ�Ĭ��״̬
					{
						if(SB_LINEUP == uHitOrig.sbCode|| SB_LINEDOWN == uHitOrig.sbCode)
						{
							CRect rc = GetSbPartRect(uHitOrig.bVert,uHitOrig.sbCode);
							DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rc,DMOLEDC_PAINTBKGND,FALSE);
							DrawScrollBar(pCanvas,rc,uHitOrig.sbCode,DMSBST_NORMAL,uHitOrig.bVert);
							DM_ReleaseCanvas(pCanvas);
						}
						else if (SB_THUMBTRACK == uHitOrig.sbCode)
						{//��Ҫ�Ȼ�������ٻ��϶���,�Դ����϶������ܳ��ֵİ�͸��
							CRect rc = GetSbRailwayRect(uHitOrig.bVert);
							DMSmartPtrT<IDMCanvas> pCanvas=DM_GetCanvas(&rc,DMOLEDC_PAINTBKGND,FALSE);
							DrawScrollBar(pCanvas,rc,SB_PAGEUP,DMSBST_NORMAL,uHitOrig.bVert);
							rc = GetSbPartRect(uHitOrig.bVert,SB_THUMBTRACK);
							DrawScrollBar(pCanvas,rc,SB_THUMBTRACK,DMSBST_NORMAL,uHitOrig.bVert);
							DM_ReleaseCanvas(pCanvas);
						}
					}

					if (IsScrollBarEnable(uHit.bVert))
					{// �����ǰͣ���ڹ������ϣ�����Hover
						if (SB_LINEUP == uHit.sbCode|| SB_LINEDOWN == uHit.sbCode)
						{
							CRect rc = GetSbPartRect(uHit.bVert,uHit.sbCode);
							DMSmartPtrT<IDMCanvas> pCanvas= DM_GetCanvas(&rc,DMOLEDC_PAINTBKGND,FALSE);
							DrawScrollBar(pCanvas,rc,uHit.sbCode,DMSBST_HOVER,uHit.bVert);
							DM_ReleaseCanvas(pCanvas);
						}
						else if(SB_THUMBTRACK == uHit.sbCode)
						{// ��Ҫ�Ȼ�������ٻ��϶���,�Դ����϶������ܳ��ֵİ�͸��
							CRect rc = GetSbRailwayRect(uHit.bVert);
							DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rc,DMOLEDC_PAINTBKGND,FALSE);
							DrawScrollBar(pCanvas,rc,SB_PAGEUP,DMSBST_NORMAL,uHit.bVert);
							rc = GetSbPartRect(uHit.bVert,SB_THUMBTRACK);
							DrawScrollBar(pCanvas,rc,SB_THUMBTRACK,DMSBST_HOVER,uHit.bVert);
							DM_ReleaseCanvas(pCanvas);
						}
					}
				}
				m_sbInfo = uHit;      //  ����״̬��ֻ��δ�϶�״̬�¸��£���ȻButtonUp��û���ͷ���
			}

		} while (false);
	}

	void DUIScrollBase::OnNcMouseLeave()
	{// ����Ϣֻ��δ�϶�״̬�´���
		do 
		{
			if (NULL == m_psbSkin)
			{
				break;
			}

			if (m_bsbDrag||WORD(-1)==m_sbInfo.sbCode)
			{
				break;
			}
			SBSTATEINFO uHit;
			if (IsScrollBarEnable(m_sbInfo.bVert))
			{
				CRect rcSb = GetScrollBarRect(m_sbInfo.bVert);
				DMSmartPtrT<IDMCanvas> pCanvas = DM_GetCanvas(&rcSb,DMOLEDC_PAINTBKGND,FALSE);
				int nState = DMSBST_NOACTIVE;
				CRect rcDest;
				rcDest = GetSbPartRect(m_sbInfo.bVert,SB_LINEUP);		 // ���ϼ�ͷ
				DrawScrollBar(pCanvas,rcDest,SB_LINEUP,nState,m_sbInfo.bVert);

				rcDest = GetSbPartRect(m_sbInfo.bVert,SB_PAGEUP);		 // �������ֵĲ�
				DrawScrollBar(pCanvas,rcDest,SB_PAGEUP,nState,m_sbInfo.bVert);

				rcDest = GetSbPartRect(m_sbInfo.bVert,SB_THUMBTRACK);	 // ��������
				DrawScrollBar(pCanvas,rcDest,SB_THUMBTRACK,nState,m_sbInfo.bVert);

				rcDest = GetSbPartRect(m_sbInfo.bVert,SB_PAGEDOWN);      // �������ֵĲ�
				DrawScrollBar(pCanvas,rcDest,SB_PAGEDOWN,nState,m_sbInfo.bVert);

				rcDest = GetSbPartRect(m_sbInfo.bVert,SB_LINEDOWN);       // ���¼�ͷ
				DrawScrollBar(pCanvas,rcDest,SB_LINEDOWN,nState,m_sbInfo.bVert);
				DM_ReleaseCanvas(pCanvas);
			}
			m_sbInfo = uHit;
		} while (false);
	}

	BOOL DUIScrollBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		if (false == m_bnowheelscroll)
		{
			zDelta /= WHEEL_DELTA;
			if (zDelta>0)
			{
				for (short i=0; i<zDelta; i++) OnScroll(TRUE,SB_LINEUP,0);
			}
			else
			{
				for (short i=0; i>zDelta; i--) OnScroll(TRUE,SB_LINEDOWN,0);
			}

			HWND hHost = GetContainer()->OnGetHWnd();
			::SendMessage(hHost,WM_MOUSEMOVE,nFlags,MAKELPARAM(pt.x,pt.y));
			::SendMessage(hHost,WM_SETCURSOR,WPARAM(hHost),MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
		}
		return FALSE;
	}

	// ��������ʾ��������ʱ���͸���Ϣ
	LRESULT DUIScrollBase::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
	{
		do 
		{
			__super::DV_GetClientRect(&m_rcsbClient);
			if (m_rcsbClient.IsRectEmpty())
			{// δ��ʼ��
				break;
			}
			if (HasScrollBar(true))
			{
				m_rcsbClient.right -= m_isbWid;
			}
			if (HasScrollBar(false)) 
			{
				m_rcsbClient.bottom -= m_isbWid;
			}
		} while (false);
		return 0;
	}

	void DUIScrollBase::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		if (!bShow && m_bsbDrag)
		{// ���ش���ʱ���п��������϶�����������Ҫ����һ�¡�
			OnNcLButtonUp(0,CPoint());
		}
		__super::OnShowWindow(bShow,nStatus);
	}

	void DUIScrollBase::DM_OnTimer(char cTimerID)
	{
		if (cTimerID==TIMER_SBWAIT)
		{
			DM_KillTimer(cTimerID);
			DM_SetTimer(TIMER_SBGO,50);
			DM_OnTimer(TIMER_SBGO);
		}
		else if (cTimerID==TIMER_SBGO)
		{
			if (m_sbInfo.sbCode>=SB_LINEUP && m_sbInfo.sbCode<=SB_PAGEDOWN)
			{
				OnScroll(m_sbInfo.bVert,m_sbInfo.sbCode,0);
			}
		}
	}

	//---------------------------------------------------
	// Function Des: ����
	DMCode DUIScrollBase::DV_GetClientRect(LPRECT lpRect)
	{
		if (lpRect)
		{
			lpRect->left   = m_rcsbClient.left;
			lpRect->right  = m_rcsbClient.right;
			lpRect->top    = m_rcsbClient.top;
			lpRect->bottom = m_rcsbClient.bottom;
		}
		return DM_ECODE_OK;
	}

	DMCode DUIScrollBase::DV_OnNcHitTest(CPoint pt)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			if (m_bsbDrag)// �϶�����״̬
			{
				iErr = DM_ECODE_OK;
				break;
			}

			DV_GetClientRect(m_rcsbClient);
			if (m_rcsbClient.PtInRect(pt))
			{// �˴��ͻ���ָ����ȥ���˹������ͻ���
				break;
			}
			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DUIScrollBase::DV_UpdateSkin(WPARAM wp, LPARAM lp)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			iErr = __super::DV_UpdateSkin(wp, lp);
			if (DMSUCCEEDED(iErr))
			{
				break;
			}

			if (g_pDMApp->IsNeedUpdateSkin(m_psbSkin))
			{
				iErr = DM_ECODE_OK;
				break;
			}
		} while (false);
		return iErr;
	}

	//---------------------------------------------------
	// ����
	bool DUIScrollBase::HasScrollBar(bool bVert)
	{
		bool bRet = false;
		if (m_isbVisible&(bVert?DMSB_VERT:DMSB_HORZ))
		{
			bRet = true;
		}
		return bRet;
	}

	CRect DUIScrollBase::GetScrollBarRect(bool bVert)
	{
		CRect rcSb;
		do 
		{
			if (false == HasScrollBar(bVert))
			{// Ĭ��Ϊ��CRect
				break;
			}
			__super::DV_GetClientRect(&rcSb);// ��ù��������ڿؼ��Ŀͻ�����С
			if (bVert)
			{// �����Ժ���չΪ֧�������ʾ������
				rcSb.left = rcSb.right-m_isbWid;
			}
			else
			{
				rcSb.top = rcSb.bottom-m_isbWid;
			}

			if (HasScrollBar(!bVert))
			{// ͬʱ������һ���Ĺ�����
				if (bVert)
				{
					rcSb.bottom -= m_isbWid;
				}
				else
				{
					rcSb.right -= m_isbWid;
				}
			}
		} while (false);
		return rcSb;
	}

	CRect DUIScrollBase::GetSbPartRect(bool bVert,UINT uSBCode)
	{// �ɲο�DUIScrollBar
		CRect rcDest;
		CRect rcSb;
		do 
		{
			if (NULL == m_psbSkin)
			{
				break;
			}

			rcSb = GetScrollBarRect(bVert);// ֱ�ӻ�ȡ��ֱ�Ĺ�������С��֮������ֱ����DUIScrollBar.GetPartRect
			if (rcSb.IsRectEmpty())
			{
				LOG_INFO("[mid]rcSb is Empty\n");
				break;
			}
			int iArrowLen		= 0;   //0 ���¼�ͷ����
			int iPageUpLen		= 0;   //1 �ϲ۵ĳ���
			int iPageDownLen	= 0;   //2 �²۵ĳ���
			int iThumbLen		= 0;   //3 ����ĳ���

			// ����
			SCROLLINFO m_si = bVert?(m_siVer):(m_siHoz);
			int nMax = m_si.nMax;
			int nTrackPos = m_si.nTrackPos;
			int nAllLen = (bVert?rcSb.Height():rcSb.Width());
			int iPageAllLen = 0;

			// �ٽ��������
			if (-1 == nTrackPos)
			{
				nTrackPos = m_si.nPos; 
			}
			if (nMax < m_si.nMin+(int)m_si.nPage-1) 
			{
				nMax = m_si.nMin + m_si.nPage-1;
			}
			if (0 == nAllLen)
			{
				LOG_INFO("[mid]length is zero\n");
				break;
			}
			iPageAllLen = nAllLen-2*m_isbAllowSize; // �м䳤��Ϊ�ܳ��ȼ�ȥ������ͷ����
			if (iPageAllLen<0)
			{
				iPageAllLen = 0;
			}
			iThumbLen = m_si.nPage*iPageAllLen/(nMax-m_si.nMin+1);	// �ؼ����㹫ʽ��nPage/(nMax-nMin+1)=���鳤��/���������ܳ��ȣ����ǻ���ɻ����Ĳۣ�
			if (nMax <= m_si.nMin+(int)m_si.nPage-1)      // û�л�����
			{
				iThumbLen = 0;						 
			}
			if (iThumbLen>0&&iThumbLen<THUMB_MINSIZE)// �й�����
			{
				iThumbLen = THUMB_MINSIZE;
			}
			if (iPageAllLen<THUMB_MINSIZE)			 // û�л�����
			{
				iThumbLen = 0;
			}
			iArrowLen = m_isbAllowSize;
			if (0 == iPageAllLen)				     // ��ʼ��0����
			{
				iArrowLen = nAllLen/2;
			}

			if (0 == iThumbLen)						// ��ʼ��1,2����
			{
				iPageUpLen = iPageDownLen = iPageAllLen/2;// ��ʱû�������ˣ�ƽ�ְ�
			}
			else
			{
				iPageUpLen = (iPageAllLen-iThumbLen)*nTrackPos/(m_si.nMax-m_si.nMin-m_si.nPage+1);
				iPageDownLen = iPageAllLen-iThumbLen-iPageUpLen;
			}

			switch (uSBCode)
			{
			case SB_LINEUP: 	rcDest.SetRect(0,0,rcSb.Width(),iArrowLen);break;
			case SB_PAGEUP:		rcDest.SetRect(0,iArrowLen,rcSb.Width(),iArrowLen+iPageUpLen); break;
			case SB_THUMBTRACK: rcDest.SetRect(0,iArrowLen+iPageUpLen,rcSb.Width(),iArrowLen+iPageUpLen+iThumbLen);break;
			case SB_PAGEDOWN:	rcDest.SetRect(0,iArrowLen+iPageUpLen+iThumbLen,rcSb.Width(),iArrowLen+iPageUpLen+iThumbLen+iPageDownLen);break;
			case SB_LINEDOWN:   rcDest.SetRect(0,iArrowLen+iPageUpLen+iThumbLen+iPageDownLen,rcSb.Width(),iArrowLen+iPageUpLen+iThumbLen+iPageDownLen+iArrowLen);break;
			default:            rcDest.SetRectEmpty();break;
			}
		} while (false);

		if (!bVert&&FALSE == rcDest.IsRectEmpty())
		{
			rcDest.left   = rcDest.top;
			rcDest.right  = rcDest.bottom;
			rcDest.top	  = 0;
			rcDest.bottom = rcSb.Height();
		}
		rcDest.OffsetRect(rcSb.TopLeft());
		return rcDest;
	}

	CRect DUIScrollBase::GetSbRailwayRect(bool bVert)
	{
		CRect rcDest;
		do 
		{
			if (NULL == m_psbSkin)
			{
				LOG_INFO("[mid]m_pSbSkin is Empty\n");
				break;
			}
			CRect rcSb = GetScrollBarRect(bVert);
			if (rcSb.IsRectEmpty())
			{
				LOG_INFO("[mid]rcSb is Empty\n");
				break;
			}

			if (bVert)
			{
				rcSb.DeflateRect(0,m_isbAllowSize);
			}
			else
			{
				rcSb.DeflateRect(m_isbAllowSize,0);
			}
			rcDest = rcSb;
		} while (false);

		return rcDest;
	}

	CRect DUIScrollBase::GetSbSlideRectByPos(bool bVert,int nPos)
	{
		SCROLLINFO*psi = bVert?(&m_siVer):(&m_siHoz);
		int nOldPos    = psi->nTrackPos;
		psi->nTrackPos = nPos;
		CRect rcRet    = GetSbPartRect(bVert,SB_THUMBTRACK);
		psi->nTrackPos = nOldPos;
		return rcRet;
	}

	SBSTATEINFO DUIScrollBase::HitTest(CPoint pt)
	{
		SBSTATEINFO hi;
		do 
		{
			CRect rcSbVer = GetScrollBarRect(true);
			CRect rcSbHoz = GetScrollBarRect(false);
			if (rcSbVer.PtInRect(pt))
			{
				hi.bVert = true;
			}
			else if (rcSbHoz.PtInRect(pt))
			{
				hi.bVert = false;
			}
			else
			{
				break;
			}

			CRect rc;// ��DUIScrollBar����һ�£��Ա��������
			rc = GetSbPartRect(hi.bVert,SB_LINEUP);    if (rc.PtInRect(pt)) {hi.sbCode=SB_LINEUP;break;}
			rc = GetSbPartRect(hi.bVert,SB_LINEDOWN);  if (rc.PtInRect(pt)) {hi.sbCode= SB_LINEDOWN;break;}
			rc = GetSbPartRect(hi.bVert,SB_THUMBTRACK);if (rc.PtInRect(pt)) {hi.sbCode= SB_THUMBTRACK;break;}
			rc = GetSbPartRect(hi.bVert,SB_PAGEUP);    if (rc.PtInRect(pt)) {hi.sbCode= SB_PAGEUP;break;}
			rc = GetSbPartRect(hi.bVert,SB_PAGEDOWN);  if (rc.PtInRect(pt)) {hi.sbCode= SB_PAGEDOWN;break;}
		} while (false);
		return hi;
	}

	int DUIScrollBase::GetSbSlideLength(bool bVert)
	{
		SCROLLINFO* psi = bVert?(&m_siVer):(&m_siHoz);
		CRect rcSb = GetScrollBarRect(bVert);
		int nInterHei = (bVert?rcSb.Height():rcSb.Width())- 2*m_isbAllowSize;
		int nSlideHei = psi->nPage*nInterHei/(psi->nMax-psi->nMin+1);
		if (nSlideHei<THUMB_MINSIZE)
		{
			nSlideHei = THUMB_MINSIZE;
		}
		if (nInterHei<THUMB_MINSIZE)
		{
			nSlideHei = 0;
		}

		return nSlideHei;
	}

	void DUIScrollBase::ScrollUpdateWindow()
	{
		DWORD dwTime = GetTickCount();
		if (dwTime-m_dwUpdateTime>=m_dwUpdateInterval)
		{
			GetContainer()->OnUpdateWindow();
			m_dwUpdateTime = dwTime;
		}
	}

	void DUIScrollBase::DrawScrollBar(IDMCanvas *pCanvas,LPCRECT lpRectDraw, int iSbCode,int iState,bool bVert, BYTE alpha)
	{
		do 
		{
			if (!m_psbSkin||NULL == pCanvas)
			{
				break;
			}
			if (DM_IsDisable(true)||!IsScrollBarEnable(bVert))
			{
				iState = DMSBST_DISABLE;
			}
			m_psbSkin->Draw(pCanvas,lpRectDraw,MAKESBSTATE(iSbCode,iState,bVert),alpha);
		} while (false);
	}

	//------------------------------------
	DMCode DUIScrollBase::OnAttributesbSkin(LPCWSTR lpszValue, bool bLoadXml)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			m_psbSkin = g_pDMApp->GetSkin(lpszValue);
			if (!bLoadXml)
			{
				if (m_psbSkin)
				{
					if (-1 == m_isbWid)
					{
						CSize size;
						m_psbSkin->GetStateSize(size);
						m_isbWid = size.cx/9; // ����9*��5=45��״̬
					}		
					if (-1 == m_isbAllowSize)
					{
						m_isbAllowSize = m_isbWid;
					}
				}
				else
				{
					m_isbWid = -1;
					m_isbAllowSize = -1;
				}	
				DM_InvalidateRect(m_rcWindow);
			}

			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}
	DMCode DUIScrollBase::OnAttributesbWidth(LPCWSTR lpszValue, bool bLoadXml)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			int isbWidth = m_isbWid;
			dm_parseint(lpszValue, isbWidth);
			if (!bLoadXml&&isbWidth!=m_isbWid)
			{
				m_isbWid = isbWidth;
				DM_InvalidateRect(m_rcWindow);
			}
			else
			{
				m_isbWid = isbWidth;
			}

			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DUIScrollBase::OnAttributesbEnable(LPCWSTR lpszValue, bool bLoadXml)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			int isbEnable = m_isbEnable;
			dm_parseint(lpszValue, isbEnable);
			if (!bLoadXml&&isbEnable!=m_isbEnable)
			{
				m_isbEnable = isbEnable;
				DM_InvalidateRect(m_rcWindow);
			}
			else
			{
				m_isbEnable = isbEnable;
			}

			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DUIScrollBase::OnAttributeRange(LPCWSTR lpszValue, bool bLoadXml)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			CSize szRange = m_szRange;
			dm_parsesize(lpszValue, szRange);
			if (!bLoadXml&&szRange!=m_szRange)
			{
				SetRangeSize(szRange);
			}
			else
			{
				m_szRange = szRange;
			}

			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

	DMCode DUIScrollBase::OnAttributePos(LPCWSTR lpszValue, bool bLoadXml)
	{
		DMCode iErr = DM_ECODE_FAIL;
		do 
		{
			CPoint ptPos = m_ptCurPos;
			dm_parsepoint(lpszValue, ptPos);
			if (!bLoadXml&&m_ptCurPos!=ptPos)
			{
				SetRangeCurPos(ptPos);
			}
			else
			{
				m_ptCurPos = ptPos;
			}

			iErr = DM_ECODE_OK;
		} while (false);
		return iErr;
	}

}//namespace DM