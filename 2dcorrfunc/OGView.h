#if !defined(AFX_OGVIEW_H__882A98B6_F913_424C_A692_EA5E21EFB28E__INCLUDED_)
#define AFX_OGVIEW_H__882A98B6_F913_424C_A692_EA5E21EFB28E__INCLUDED_
#pragma once

class CPropDlg;

class CPoint3D
{
public:
	float x, y, z;
	CPoint3D () { x=y=z=0; }
	CPoint3D (float c1, float c2, float c3)
	{
		x = c1;		z = c2;		y = c3;
	}
	CPoint3D& operator=(const CPoint3D& pt)
	{
		x = pt.x;	z = pt.z;	y = pt.y;
		return *this;
	}
	CPoint3D (const CPoint3D& pt)
	{
		*this = pt;
	}
};


class COGView : public CView
{
protected:
	COGView();
	DECLARE_DYNCREATE(COGView)
public:
	//======== New data
	long	m_BkClr;	// Background color
	HGLRC	m_hRC;		// Rendering context OpenGL
	HDC	m_hdc;		// Windows device context
	GLfloat	m_AngleX;	// Rotation angle (around X-axis)
	GLfloat m_AngleY;	// Rotation angle (around Y-axis)
	GLfloat	m_AngleView;	// Perspective angle
	GLfloat	m_fRangeX;	// Graphics dimension (along X-axis)
	GLfloat m_fRangeY;	// Graphics dimension (along Y-axis)
	GLfloat m_fRangeZ;	// Graphics dimension (along Z-axis)
	GLfloat	m_dx;		// Displacement quantum (along X-axis)
	GLfloat m_dy;		// Displacement quantum (along Y-axis)
	GLfloat	m_xTrans;	// Displacement (along X-axis)
	GLfloat m_yTrans;	// Displacement (along Y-axis)
	GLfloat m_zTrans;	// Displacement (along Z-axis)
	GLenum	m_FillMode;	// Polygon filling mode
	bool	m_bCaptured;	// Mouse capture flag
	bool	m_bRightButton;	// Right mouse button flag
	bool	m_bQuad;	// Flag of using GL_QUAD (instead of GL_QUAD_STRIP)
	CPoint	m_pt;		// Current mouse position
	UINT	m_xSize;	// Current client window sixe (along X-axis)
	UINT	m_zSize;	// Current client window sixe (along -axis)
	vector < CPoint3D > m_cPoints;	// Graphics dimension (along X-axis)
	int	m_LightParam[11];	// Graphics dimension (along X-axis)
	CPropDlg *m_pDlg;		// Graphics dimension (along X-axis)

	//======== Public methods
	COGDoc* GetDocument() { return DYNAMIC_DOWNCAST(COGDoc,m_pDocument); }
	virtual ~COGView();

	//======== New methods
	void DrawScene();	// Prepare and store the image
	void DefaultGraphic();	// Create and save the default plot
	void ReadData();	// Data-file manipulations
	bool DoRead(HANDLE hFile);	// Data-file reading		
	//===== Take the data from buffer and store in m_cPoints
	void SetGraphPoints(BYTE* buff, DWORD nSize);
	void SetLightParam (short lp, int nPos);	// Set lighting parameters
	void GetLightParams(int *pPos);			// Get lighting parameters
	void SetLight();				// Set the lighting
	void SetBkColor();				// Set background color
	void LimitAngles();				// Limit rotation angles
	
	//{{AFX_VIRTUAL(COGView)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(COGView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnViewQuad();
	afx_msg void OnUpdateViewQuad(CCmdUI* pCmdUI);
	afx_msg void OnViewFill();
	afx_msg void OnUpdateViewFill(CCmdUI* pCmdUI);
	afx_msg void OnEditProp();
	afx_msg void OnEditBackground();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
#endif
