#include "stdafx.h"
#include "OG.h"

#include "OGDoc.h"
#include "OGView.h"
#include "PropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline void MinMax (float d, float& Min, float& Max)
{
	if (d > Max)
		Max = d;
	else if (d < Min)
		Min = d;
}

IMPLEMENT_DYNCREATE(COGView, CView)

BEGIN_MESSAGE_MAP(COGView, CView)
	//{{AFX_MSG_MAP(COGView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_QUAD, OnViewQuad)
	ON_UPDATE_COMMAND_UI(ID_VIEW_QUAD, OnUpdateViewQuad)
	ON_COMMAND(ID_VIEW_FILL, OnViewFill)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILL, OnUpdateViewFill)
	ON_COMMAND(ID_EDIT_PROP, OnEditProp)
	ON_COMMAND(ID_EDIT_BACKGROUND, OnEditBackground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COGView::COGView()
{
	//====== Rendering context does not exist yet
	m_hRC = 0;

	//====== Initial image turn
	m_AngleX = 35.f;
	m_AngleY = 20.f;

	//====== Projection matrix view angle
	m_AngleView = 45.f;

	//====== Initial bkcolor
	m_BkClr = RGB(0, 0, 96);

	// Initial mode to fill the inner polygons (quads) points
	m_FillMode = GL_FILL;

	//====== Initial plot creation
	DefaultGraphic();

	//====== Initial image shift
	//====== One and a half object size (backward)
	m_zTrans = -1.5f*m_fRangeX;
	m_xTrans = m_yTrans = 0.f;

	//== Initial shift quantums (for rotation animation)
	m_dx = m_dy = 0.f;

	//====== Mouse is not captured
	m_bCaptured = false;
	//====== Right mouse button has not been pressed
	m_bRightButton = false;
	//====== We use quads to create the surface
	m_bQuad = true;

	//====== Initial lighting params
	m_LightParam[0] = 50;	// X position
	m_LightParam[1] = 80;	// Y position
	m_LightParam[2] = 100;	// Z position
	m_LightParam[3] = 15;	// Ambient light
	m_LightParam[4] = 70;	// Diffuse light
	m_LightParam[5] = 100;	// Specular light
	m_LightParam[6] = 100;	// Ambient material
	m_LightParam[7] = 100;	// Diffuse material
	m_LightParam[8] = 40;	// Specular material
	m_LightParam[9] = 70;	// Shininess material
	m_LightParam[10] = 0;	// Emission material
}

COGView::~COGView(){}

BOOL COGView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

void COGView:: OnDraw(CDC* pDC)
{
	//========== Clear the background and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//========== Clean the modeling matrix (make it equal the unity matrix)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//======= At first set the light (otherwise it will rotate with the image)
	SetLight();

	//====== Change the modeling matrix coefficients in order
	glTranslatef(m_xTrans,m_yTrans,m_zTrans);	// to shift
	glRotatef (m_AngleX, 1.0f, 0.0f, 0.0f );	// and to rotate
	glRotatef (m_AngleY, 0.0f, 1.0f, 0.0f );

	//====== the following vertices coordinates (they are being multiplied by matrix)
	glCallList(1);

	//====== Switch back and front buffers (to show what happened)
	SwapBuffers(m_hdc);
}

BOOL COGView::OnEraseBkgnd(CDC* pDC) { return TRUE; }

int COGView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	PIXELFORMATDESCRIPTOR pfd =	// Structure used to describe the format
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,			// Version
		PFD_DRAW_TO_WINDOW |	// Supports GDI
		PFD_SUPPORT_OPENGL |	// Supports OpenGL
		PFD_DOUBLEBUFFER,	// Use double buffering (more efficient drawing)
		PFD_TYPE_RGBA,		// No pallettes
		24, 			// Number of color planes
		 			// in each color buffer
		24,	0,		// for Red-component
		24,	0,		// for Green-component
		24,	0,		// for Blue-component
		24,	0,		// for Alpha-component
		0,			// Number of planes
					// of Accumulation buffer
		0,			// for Red-component
		0,			// for Green-component
		0,			// for Blue-component
		0,			// for Alpha-component
		32, 			// Depth of Z-buffer
		0,			// Depth of Stencil-buffer
		0,			// Depth of Auxiliary-buffer
		0,			// Now is ignored
		0,			// Number of planes
		0,			// Now is ignored
		0,			// Color of transparent mask
		0			// Now is ignored
	};

	//====== Get current Windows context
	m_hdc = ::GetDC(GetSafeHwnd());

	//====== Ask to find the nearest compatible pixel-format
	int iD = ChoosePixelFormat(m_hdc, &pfd);
	if ( !iD )
	{
		MessageBox("ChoosePixelFormat::Error");
		return -1;
	}

	//====== Try to set this format
	if ( !SetPixelFormat (m_hdc, iD, &pfd) )
	{
		MessageBox("SetPixelFormat::Error");
		return -1;
	}

	//====== Try to create the OpenGL rendering context
	if ( !(m_hRC = wglCreateContext (m_hdc)))
	{
		MessageBox("wglCreateContext::Error");
		return -1;
	}

	//====== Try to put it in action
	if ( !wglMakeCurrent (m_hdc, m_hRC))
	{
		MessageBox("wglMakeCurrent::Error");
		return -1;
	}

	//====== Now you can issue OpenGL commands (i.e. call gl*** functions)
	glEnable(GL_LIGHTING);		// Lighting will be used
	//====== Only one (first) source of light
	glEnable(GL_LIGHT0);
	//====== The depth of the Z-buffer will be taken into account
	glEnable(GL_DEPTH_TEST);
	//====== Material colors will be taken into account
	glEnable(GL_COLOR_MATERIAL);

	//====== Our function to set the background
	SetBkColor();

	//====== Create and store the image in a special list of OpenGL commands
	DrawScene();
	return 0;
}

void COGView::SetBkColor()
{
	//====== Split the color to 3 components
	GLclampf red	= GetRValue(m_BkClr)/255.f,
			 green	= GetGValue(m_BkClr)/255.f,
			 blue	= GetBValue(m_BkClr)/255.f;
	//====== Set the clear (background) color
	glClearColor (red, green, blue, 0.f);

	//====== Actual background erasure
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void COGView::DrawScene()
{
	//====== Create the new list of OpenGL commands
	glNewList(1, GL_COMPILE);

	//====== Set the polygon filling mode
	glPolygonMode(GL_FRONT_AND_BACK, m_FillMode);

	//====== Image grid sizes
	UINT	nx = m_xSize-1,
		nz = m_zSize-1;

	//====== Turn on the primitive connection mode (not connected)
	if (m_bQuad)
		glBegin (GL_QUADS);

	for (UINT z=0, i=0;  z < nz;  z++, i++)
	{
		//====== Turn on the primitive connection mode (connected)
		//====== The strip of connected quads begins here
		if (!m_bQuad)
			glBegin(GL_QUAD_STRIP);

		for (UINT x=0;  x < nx;  x++, i++)
		{
			// i, j, k, n — 4 indices of a quad
			// Counter Clockwise direction

			int	j = i + m_xSize,	// Other vertices indices
				k = j+1,
				n = i+1;

			//=== Get coordinates of 4 vertices
			float
				xi = m_cPoints[i].x,
				yi = m_cPoints[i].y,
				zi = m_cPoints[i].z,

				xj = m_cPoints[j].x,
				yj = m_cPoints[j].y,
				zj = m_cPoints[j].z,

				xk = m_cPoints[k].x,
				yk = m_cPoints[k].y,
				zk = m_cPoints[k].z,

				xn = m_cPoints[n].x,
				yn = m_cPoints[n].y,
				zn = m_cPoints[n].z,

				//=== Quad side lines vectors coordinates
				ax = xi-xn,
				ay = yi-yn,

				by = yj-yi,
				bz = zj-zi,

				//====== Normal vector coordinates
				vx = ay*bz,
				vy = -bz*ax,
				vz = ax*by,

				//====== Normal vector length
				v  = float(sqrt(vx*vx + vy*vy + vz*vz));
			
			//====== Scale to unity
			vx /= v;
			vy /= v;
			vz /= v;

			//====== Set the normal vector
			glNormal3f (vx,vy,vz);

			//===== Not connected quads branch
			if (m_bQuad)
			{
				//====== Vertices are given in counter clockwise direction order
				glColor3f (0.2f, 0.8f, 1.f);
				glVertex3f (xi, yi, zi);
				glColor3f (0.6f, 0.7f, 1.f);
				glVertex3f (xj, yj, zj);
				glColor3f (0.7f, 0.9f, 1.f);
				glVertex3f (xk, yk, zk);
				glColor3f (0.7f, 0.8f, 1.f);
				glVertex3f (xn, yn, zn);
			}
			else
			//===== Connected quads branch
			{
				glColor3f (0.9f, 0.9f, 1.0f);
				glVertex3f (xi, yi, zi);
				glColor3f (0.5f, 0.8f, 1.0f);
				glVertex3f (xj, yj, zj);
			}
		}
		//====== Close block of GL_QUAD_STRIP commands
		if (!m_bQuad)
			glEnd();
	}
		//====== Close block of GL_QUADS commands
	if (m_bQuad)
		glEnd();

	//====== Close the list of OpenGL commands
	glEndList();
}	

void COGView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	double dAspect = cx<=cy ? double(cy)/cx : double(cx)/cy;
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (m_AngleView, dAspect, 0.1, 10000.);
	glViewport(0, 0, cx, cy);
}

void COGView::LimitAngles()
{
	while (m_AngleX < -360.f)
		m_AngleX += 360.f;
	while (m_AngleX > 360.f)
		m_AngleX -= 360.f;
	while (m_AngleY < -360.f)
		m_AngleY += 360.f;
	while (m_AngleY > 360.f)
		m_AngleY -= 360.f;
}

void COGView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//====== Stop rotation
	KillTimer(1);

	//====== Zero the quantums
	m_dx = 0.f;
	m_dy = 0.f;

	//====== Capture the mouse messages and direct them in our window
	SetCapture();
	//====== Remember the fact
	m_bCaptured = true;
	//====== and where it happened
	m_pt = point;
}


void COGView::OnRButtonDown(UINT nFlags, CPoint point)
{
	//====== Remember the fact
	m_bRightButton = true;

	//====== and reproduce the left button response
	OnLButtonDown(nFlags, point);
}

void COGView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bCaptured)
	{
		// Desired rotation speed components
		m_dy = float(point.y - m_pt.y)/40.f;
		m_dx = float(point.x - m_pt.x)/40.f;

		//====== If Ctrl was pressed
		if (nFlags & MK_CONTROL)
		{
			//=== we shift (translate) the image
			m_xTrans += m_dx;
			m_yTrans -= m_dy;
		}
		else
		{
			//====== If the right mouse button is pressed
			if (m_bRightButton)
				//====== we shift along the z-axis
				m_zTrans += (m_dx + m_dy)/2.f;
			else
			{
				//====== otherwise we rotate the image
				LimitAngles();
				double a = fabs(m_AngleX);
				if (90. < a && a < 270.)
					m_dx = -m_dx;
				m_AngleX += m_dy;
				m_AngleY += m_dx;
			}
		}
		//=== In any case we should store the coordinates
		m_pt = point;
		Invalidate(FALSE);
	}
}

void COGView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//====== If we captured the mouse,
	if (m_bCaptured)
	{
		//=== query the desired quantum value
		//=== if it exeeds the sensativity threshold
		if (fabs(m_dx) > 0.5f || fabs(m_dy) > 0.5f)
			//=== Turn on the constant rotation
			SetTimer(1,33,0);
		else
			//=== Turn off the constant rotation
			KillTimer(1);

		//====== Clear the capture flag
		m_bCaptured = false;
		ReleaseCapture();
	}
}


void COGView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_bRightButton = m_bCaptured = false;
	ReleaseCapture();
}

void COGView::OnTimer(UINT nIDEvent)
{
	LimitAngles();
	//====== Increase the angles
	m_AngleX += m_dy;
	m_AngleY += m_dx;
	Invalidate(FALSE);
}


void COGView::DefaultGraphic()
{
	//====== Coordinate grid dimensions
	m_xSize = m_zSize = 33;

	//====== Number of meshes is less than number of nodes
	UINT	nz = m_zSize - 1,
		nx = m_xSize - 1;

	// File size in bytes
	DWORD nSize = m_xSize * m_zSize * sizeof(float)	+ 2*sizeof(UINT);

	//====== Temporary buffer
	BYTE *buff = new BYTE[nSize+1];

	//====== Point at the start of it with UINT-type pointer
	UINT *p = (UINT*)buff;

	//====== Place the two UINTs
	*p++ = m_xSize;
	*p++ = m_zSize;

	//====== Change the pointer type to continue with the floating numbers
	float *pf = (float*)p;

	//=== Default formula coefficients
	double	fi = atan(1.)*6,
		kx = fi/nx,
		kz = fi/nz;

	//====== For all the grid nodes calculate
	//=== calculate and place default function values in the same buffer
	for (UINT i=0;  i < m_zSize;  i++)
	{
		for (UINT j=0;  j < m_xSize;  j++)
		{
			//====== Sample function
			*pf++ = float (sin(kz*(i-nz/2.)) * sin(kx*(j-nx/2.)));
		}
	}

	//=== We want to know the real number of bytes written to the file
	DWORD nBytes;

	//=== Create and open the default data file (sin.dat)
	HANDLE hFile = CreateFile(_T("sin.dat"), GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);

	//====== Write the whole buffer
	WriteFile(hFile,(LPCVOID)buff, nSize,&nBytes, 0);

	//====== Close the file
	CloseHandle(hFile);

	//====== Create and fill container m_cPoints (using the same buffer)
	SetGraphPoints (buff, nSize);

	//====== Free temporary buffer
	delete [] buff;
}

void COGView::SetGraphPoints(BYTE* buff, DWORD nSize)
{
	UINT *p = (UINT*)buff;

	m_xSize = *p;
	m_zSize = *++p;

	if (m_xSize<2 || m_zSize<2 || m_xSize*m_zSize*sizeof(float) + 2 * sizeof(UINT) != nSize)
	{
		MessageBox(_T("Wrong data format"));
		return;
	}

	m_cPoints.resize(m_xSize*m_zSize);

	if (m_cPoints.empty())
	{
		MessageBox(_T("Can not allocate the data"));
		return;
	}

	float	x, z,
			*pf   = (float*)++p,
			fMinY = *pf,
			fMaxY = *pf,
			right = (m_xSize-1)/2.f,
			left  = -right,
			rear  = (m_zSize-1)/2.f,
			front = -rear, 
			range = (right + rear)/2.f;

	UINT	i, j, n;

	m_fRangeY = range;
	m_fRangeX = float(m_xSize);
	m_fRangeZ = float(m_zSize);

	m_zTrans = -1.5f * m_fRangeZ;

	for (z=front, i=0, n=0;  i<m_zSize;  i++, z+=1.f)
	{
		for (x=left, j=0;  j<m_xSize;  j++, x+=1.f, n++)
		{
			MinMax (*pf, fMinY, fMaxY);
			m_cPoints[n] = CPoint3D(x,z,*pf++);
		}
	}

	float zoom = fMaxY > fMinY ? range/(fMaxY-fMinY) : 1.f;

	for (n=0;  n<m_xSize*m_zSize;  n++)
	{
		m_cPoints[n].y = zoom * (m_cPoints[n].y - fMinY) - range/2.f;
	}
}


void COGView::OnViewQuad(void)
{
	m_bQuad = ! m_bQuad;

	DrawScene();
	Invalidate(FALSE);
	UpdateWindow();
}


void COGView::OnUpdateViewQuad(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bQuad==true);	
}

void COGView::OnViewFill(void)
{
	m_FillMode = m_FillMode==GL_FILL ? GL_LINE : GL_FILL;

	DrawScene();
	Invalidate(FALSE);
	UpdateWindow();
}


void COGView::OnUpdateViewFill(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_FillMode==GL_FILL);	
}

void COGView::GetLightParams(int *pPos)
{
	for (int i=0; i<11; i++)
		pPos[i] = m_LightParam[i];
}


void COGView::SetLightParam (short lp, int nPos)
{
	m_LightParam[lp] = nPos;
	Invalidate(FALSE);
}


void COGView::OnEditProp() 
{
	if (!m_pDlg)
	{
		m_pDlg = new CPropDlg(this);
		m_pDlg->Create(IDD_PROP);
	}
	else
		m_pDlg->SetActiveWindow();
}

void COGView::SetLight()
{
	//====== Both surface sides are considered when calculating
	//====== each pixel color with the lighting formula

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

	//====== Light source position depends on the object sizes scaled to (0,100)

	float fPos[] =
	{
		(m_LightParam[0]-50)*m_fRangeX/100,
		(m_LightParam[1]-50)*m_fRangeY/100,
		(m_LightParam[2]-50)*m_fRangeZ/100,
		1.f
	};
	glLightfv(GL_LIGHT0, GL_POSITION, fPos);

	//====== Ambient light intensity
	float f = m_LightParam[3]/100.f;
	float fAmbient[4] = { f, f, f, 0.f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbient);

	//====== Diffuse light intensity
	f = m_LightParam[4]/100.f;	
	float fDiffuse[4] = { f, f, f, 0.f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fDiffuse);

	//====== Specular light intensity
	f = m_LightParam[5]/100.f;
	float fSpecular[4] = { f, f, f, 0.f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, fSpecular);

	//====== Surface material reflection properties for each light component
	f = m_LightParam[6]/100.f;
	float fAmbMat[4] = { f, f, f, 0.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fAmbMat);

	f = m_LightParam[7]/100.f;
	float fDifMat[4] = { f, f, f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fDifMat);

	f = m_LightParam[8]/100.f;
	float fSpecMat[4] = { f, f, f, 0.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fSpecMat);

	//====== Material shininess
	float fShine = 128 * m_LightParam[9]/100.f;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, fShine);

	//====== Material light emission property
	f = m_LightParam[10]/100.f;
	float fEmission[4] = { f, f, f, 0.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, fEmission);
}


void COGView::ReadData()
{
	//=== Here we place the file path 
	TCHAR szFile[MAX_PATH] = { 0 };

	//====== File extensions filter
	TCHAR *szFilter =TEXT("Graphics Data Files (*.dat)\0")
			TEXT("*.dat\0")
			TEXT("All Files\0")
			TEXT("*.*\0");

	//====== Query the current folder
	TCHAR szCurDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH-1,szCurDir);

	//=== Struct used by the standard file dialog
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));

	//====== Dialog parameters
	ofn.lStructSize	= sizeof(OPENFILENAME);
	//====== Window which owns the dialog
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFilter	= szFilter;
	//====== The filters string index (begins with 1)
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= szFile;
	ofn.nMaxFile		= sizeof(szFile);
	//====== Dialog caption
	ofn.lpstrTitle	= _T("Find a data file");
	ofn.nMaxFileTitle = sizeof (ofn.lpstrTitle);
	//====== Dialog style (only in Win2K)
	ofn.Flags		= OFN_EXPLORER;

	//====== Create and open the dialog (retuns 0 on failure)
	if (GetOpenFileName(&ofn))
	{
		// Try to open the file (which must exist)
		HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ,
				FILE_SHARE_READ, 0, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 0);

		//=== On failure CreateFile returns -1
		if (hFile == (HANDLE)-1)
		{
			MessageBox(_T("Could not open this file"));
			return;
		}

		//====== Try to read the data
		if (!DoRead(hFile))
			return;

		//====== Create and store the image
		DrawScene();

		//====== Redraw the window client region
		Invalidate(FALSE);
	}
}


bool COGView::DoRead(HANDLE hFile)
{
	DWORD nSize = GetFileSize (hFile, 0);

	if (nSize == 0xFFFFFFFF)
	{
		GetLastError();
		MessageBox(_T("Could not get file size"));
		CloseHandle(hFile);
		return false;
	}

	BYTE *buff = new BYTE[nSize+1];

	if (!buff)
	{
		MessageBox(_T("The data file is too big"));
		CloseHandle(hFile);
		return false;
	}

	DWORD nBytes;
	ReadFile (hFile, buff, nSize, &nBytes, 0);
	CloseHandle(hFile);

	if (nSize != nBytes)
	{
		MessageBox(_T("Error while reading data file"));
		return false;
	}

	SetGraphPoints (buff, nSize);

	delete [] buff;
	return true;
}


void COGView::OnEditBackground(void)
{
	CColorDialog dlg(m_BkClr);
	dlg.m_cc.Flags |= CC_FULLOPEN;
	if (dlg.DoModal()==IDOK)
	{
		m_BkClr = dlg.m_cc.rgbResult;
		SetBkColor();
		Invalidate(FALSE);
	}
}
