
// GLMFCDemoView.cpp : CGLMFCDemoView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
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

// CGLMFCDemoView 构造/析构

CGLMFCDemoView::CGLMFCDemoView()
{
	// TODO:  在此处添加构造代码
}

CGLMFCDemoView::~CGLMFCDemoView()
{
}

BOOL CGLMFCDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CView::PreCreateWindow(cs);
}

// CGLMFCDemoView 绘制

void CGLMFCDemoView::OnDraw(CDC* /*pDC*/)
{
	CGLMFCDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// CGLMFCDemoView 打印

BOOL CGLMFCDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGLMFCDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CGLMFCDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CGLMFCDemoView 诊断

#ifdef _DEBUG
void CGLMFCDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CGLMFCDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGLMFCDemoDoc* CGLMFCDemoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLMFCDemoDoc)));
	return (CGLMFCDemoDoc*)m_pDocument;
}
#endif //_DEBUG

//成员函数

// CGLMFCDemoView 消息处理程序


int CGLMFCDemoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
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

	// TODO:  在此处添加消息处理程序代码
	gl.Stop();
}


BOOL CGLMFCDemoView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	// 不重绘背景，防止闪烁
	return TRUE;//CView::OnEraseBkgnd(pDC);
}


void CGLMFCDemoView::OnSize(UINT nType, int cx, int cy)
{
	//CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	gl.SetViewport(0, 0, cx, cy);
}


void CGLMFCDemoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类
	gl.Start();
	//MoveWindow(200, 200, 800, 600);

	//在这个地方MoveWindow没用
	//AfxGetMainWnd()->MoveWindow(700, 200, 800, 600);
	GetParentFrame()->MoveWindow(400, 200, 820, 650);
}


void CGLMFCDemoView::OnFps()
{
	// TODO:  在此添加命令处理程序代码
	CString s;
	s.Format(_T("fps:%f"), gl.Fps());
	MessageBox(s);
}


void CGLMFCDemoView::OnPause()
{
	// TODO:  在此添加命令处理程序代码
	if (!gl.isSuspended)
		gl.Suspend();
	else
		gl.Resume();
}


void CGLMFCDemoView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (!bMouseTracking){
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT); //监控鼠标离开   
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;
		bMouseTracking = _TrackMouseEvent(&tme);
	}

	gl.callmousemove(point.x, point.y);
	CView::OnMouseMove(nFlags, point);
}


void CGLMFCDemoView::OnMouseLeave()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	bMouseTracking = false;

	gl.callmouseleave();
	CView::OnMouseLeave();
}


void CGLMFCDemoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callkey(nChar, GLKEYACT_PRESS);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CGLMFCDemoView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callkey(nChar, GLKEYACT_RELEASE);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CGLMFCDemoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callmousebutton(GLKEY_MOUSE_LEFT, GLKEYACT_PRESS, point.x, point.y);

	CView::OnLButtonDown(nFlags, point);
}


void CGLMFCDemoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callmousebutton(GLKEY_MOUSE_LEFT, GLKEYACT_RELEASE, point.x, point.y);

	CView::OnLButtonUp(nFlags, point);
}


void CGLMFCDemoView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callmousebutton(GLKEY_MOUSE_RIGHT, GLKEYACT_PRESS, point.x, point.y);

	CView::OnRButtonDown(nFlags, point);
}


void CGLMFCDemoView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callmousebutton(GLKEY_MOUSE_RIGHT, GLKEYACT_RELEASE, point.x, point.y);

	CView::OnRButtonUp(nFlags, point);
}


BOOL CGLMFCDemoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callscroll(0, zDelta);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


//void CGLMFCDemoView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
//{
//	// TODO:  在此添加专用代码和/或调用基类
//	MoveWindow(200, 200, 800, 600);
//
//	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
//}


void CGLMFCDemoView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	gl.callmousebutton(GLKEY_MOUSE_MID, GLKEYACT_PRESS, point.x, point.y);
	CView::OnMButtonDown(nFlags, point);
}
