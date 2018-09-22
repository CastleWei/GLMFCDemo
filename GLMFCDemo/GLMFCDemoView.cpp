
// GLMFCDemoView.cpp : CGLMFCDemoView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "GLMFCDemo.h"
#endif

#include "GLMFCDemoDoc.h"
#include "GLMFCDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGLMFCDemoView

IMPLEMENT_DYNCREATE(CGLMFCDemoView, CView)

BEGIN_MESSAGE_MAP(CGLMFCDemoView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
//	ON_WM_SIZE()
ON_WM_SIZE()
ON_COMMAND(ID_FPS, &CGLMFCDemoView::OnFps)
//ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_COMMAND(ID_PAUSE, &CGLMFCDemoView::OnPause)
ON_WM_MOUSEMOVE()
ON_WM_KEYDOWN()
ON_WM_KEYUP()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_MOUSEWHEEL()
ON_WM_MBUTTONDOWN()
ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

// CGLMFCDemoView ����/����

CGLMFCDemoView::CGLMFCDemoView()
{
	// TODO:  �ڴ˴���ӹ������
}

CGLMFCDemoView::~CGLMFCDemoView()
{
}

BOOL CGLMFCDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	return CView::PreCreateWindow(cs);
}

// CGLMFCDemoView ����

void CGLMFCDemoView::OnDraw(CDC* /*pDC*/)
{
	CGLMFCDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CGLMFCDemoView ��ӡ

BOOL CGLMFCDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CGLMFCDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CGLMFCDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}


// CGLMFCDemoView ���

#ifdef _DEBUG
void CGLMFCDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CGLMFCDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGLMFCDemoDoc* CGLMFCDemoView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLMFCDemoDoc)));
	return (CGLMFCDemoDoc*)m_pDocument;
}
#endif //_DEBUG

//��Ա����

// CGLMFCDemoView ��Ϣ�������


int CGLMFCDemoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	CRect r;
	GetClientRect(&r);
	gl.Prepare(hDC, r.Width(), r.Height());

	return 0;
}


void CGLMFCDemoView::OnDestroy()
{
	CView::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
	gl.Stop();
}


BOOL CGLMFCDemoView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	// ���ػ汳������ֹ��˸
	return TRUE;//CView::OnEraseBkgnd(pDC);
}


void CGLMFCDemoView::OnSize(UINT nType, int cx, int cy)
{
	//CView::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	gl.SetViewport(0, 0, cx, cy);
}


void CGLMFCDemoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO:  �ڴ����ר�ô����/����û���
	gl.Start();
	//MoveWindow(200, 200, 800, 600);

	//������ط�MoveWindowû��
	//AfxGetMainWnd()->MoveWindow(700, 200, 800, 600);
	GetParentFrame()->MoveWindow(400, 200, 820, 650);
}


void CGLMFCDemoView::OnFps()
{
	// TODO:  �ڴ���������������
	CString s;
	s.Format(_T("fps:%f"), gl.Fps());
	MessageBox(s);
}


void CGLMFCDemoView::OnPause()
{
	// TODO:  �ڴ���������������
	if (!gl.isSuspended)
		gl.Suspend();
	else
		gl.Resume();
}


void CGLMFCDemoView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!bMouseTracking){
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT); //�������뿪   
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;
		bMouseTracking = _TrackMouseEvent(&tme);
	}

	gl.callmousemove(point.x, point.y);
	CView::OnMouseMove(nFlags, point);
}


void CGLMFCDemoView::OnMouseLeave()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	bMouseTracking = false;

	gl.callmouseleave();
	CView::OnMouseLeave();
}


void CGLMFCDemoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callkey(nChar, GLKEYACT_PRESS);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CGLMFCDemoView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callkey(nChar, GLKEYACT_RELEASE);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CGLMFCDemoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callmousebutton(GLKEY_MOUSE_LEFT, GLKEYACT_PRESS, point.x, point.y);

	CView::OnLButtonDown(nFlags, point);
}


void CGLMFCDemoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callmousebutton(GLKEY_MOUSE_LEFT, GLKEYACT_RELEASE, point.x, point.y);

	CView::OnLButtonUp(nFlags, point);
}


void CGLMFCDemoView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callmousebutton(GLKEY_MOUSE_RIGHT, GLKEYACT_PRESS, point.x, point.y);

	CView::OnRButtonDown(nFlags, point);
}


void CGLMFCDemoView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callmousebutton(GLKEY_MOUSE_RIGHT, GLKEYACT_RELEASE, point.x, point.y);

	CView::OnRButtonUp(nFlags, point);
}


BOOL CGLMFCDemoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callscroll(0, zDelta);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


//void CGLMFCDemoView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
//{
//	// TODO:  �ڴ����ר�ô����/����û���
//	MoveWindow(200, 200, 800, 600);
//
//	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
//}


void CGLMFCDemoView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gl.callmousebutton(GLKEY_MOUSE_MID, GLKEYACT_PRESS, point.x, point.y);
	CView::OnMButtonDown(nFlags, point);
}
