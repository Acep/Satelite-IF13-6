#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <stdio.h>
#include <math.h>

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;


////////////////////Inisilaisasi Global//////////////////////////////////////
float SatelitePos[] = {-0.7f,0.5f,-3.0f};
float EarthPos[] = {0.0f,1.3f, -4.0f};
float LightPos[] = {0.4f, -0.2f, -1};

GLint   base,loop;
GLuint  texture[14];
GLUquadricObj *Obiekt1;

GLfloat roll,roll2;
GLfloat step=0.01f;
GLfloat step2=0.3f;
GLfloat rotate, rotateobiekt1,rotateobiekt2;
GLfloat angle,rotsat;
GLfloat RotateWenus;
GLfloat x,y;

GLfloat LightAmbient[]=		{ 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat LightDiffuse[]=		{ 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat LightPosition[]=	{ 10.0f, -10.0f, 10.0f, 1.0f };

GLfloat	roo;
GLfloat	ro; 
GLfloat	ro2;
GLfloat	alpha;
GLfloat	alphaplus=0.0003f;


int	   Tekst = 0;
int	   TextSync = 0;
////////////////////////////////////////////////////////////////////////////////////////

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void Build_font(void)
{
	int i;
	float cx;											// Holds Our X Character Coord
	float cy;											// Holds Our Y Character Coord

	base = glGenLists(256);							// Creating 256 Display Lists
	glBindTexture(GL_TEXTURE_2D, texture[4]);			// Select Our Font Texture
	for (i=0 ; i<256 ; i++) {                           // Loop Through All 256 Lists
		cx = float(i%16)/16.0f;							// X Position Of Current Character
		cy = float(i/16)/16.0f;							// Y Position Of Current Character
		glNewList(base+i,GL_COMPILE);					// Start Building A List
			glBegin(GL_QUADS);							// Use A Quad For Each Character
				glTexCoord2f(cx,1-cy-0.0625f);			// Texture Coord (Bottom Left)
				glVertex2i(0,0);						// Vertex Coord (Bottom Left)
				glTexCoord2f(cx+0.0625f,1-cy-0.0625f);	// Texture Coord (Bottom Right)
				glVertex2i(16,0);						// Vertex Coord (Bottom Right)
				glTexCoord2f(cx+0.0625f,1-cy);			// Texture Coord (Top Right)
				glVertex2i(16,16);						// Vertex Coord (Top Right)
				glTexCoord2f(cx,1-cy);					// Texture Coord (Top Left)
				glVertex2i(0,16);						// Vertex Coord (Top Left)
			glEnd();									// Done Building Our Quad (Character)
			glTranslated(10,0,0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}													// Loop Until All 256 Are Built
}

// Delete The Font From Memory (NEW)
void KillFont(void)
{
	glDeleteLists(base, 256);							// Delete All 256 Display Lists
}

void glPrint(GLint x, GLint y, char* string, int set)
{
	if (set > 1) set = 1;
	glBindTexture(GL_TEXTURE_2D, texture[4]);			// Select Our Font Texture
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,640,0,480,-100,100);						// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	glTranslated(x,y,0);								// Position The Text (0,0 - Bottom Left)
	glListBase(base-32+(128*set));					// Choose The Font Set (0 or 1)
	glCallLists(strlen(string),GL_BYTE,string);			// Write The Text To The Screen
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}

///////////////////////Mengeset Pencahayaan/////////////////////////////////////////
void Swiatlo(void)
{

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Mengeset Cahaya ambient
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Mengeset Cahaya diffuse
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Posisi cahaya

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);								// Mengaktifkan Cahaya satu
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
}
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////Meload Gambar bertipe Bitmap////////////////////////////////
AUX_RGBImageRec *LoadBMP(char *Filename)                                        
{
        FILE *File=NULL;
        if (!Filename)                                                          
        {
                return NULL;                                                    
        }
        File=fopen(Filename,"r");
        if (File)                                                               
        {
            fclose(File);                                                   
            return auxDIBImageLoad(Filename);                               
        }
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////Prosedur membuat tekstur///////////////////////////////////////
int LoadTextures()													
{
    int Status = FALSE;
    AUX_RGBImageRec *TextureImage[14];
    memset(TextureImage, 0, sizeof(void *)*14);
    if ( (TextureImage[0]=LoadBMP("kosmos.bmp")) &&
         (TextureImage[1]=LoadBMP("belka.bmp"))  &&
         (TextureImage[2]=LoadBMP("ziemia.bmp"))&&           
		 (TextureImage[3]=LoadBMP("moon.bmp"))&&
         (TextureImage[4]=LoadBMP("fonty.bmp"))  &&
         (TextureImage[5]=LoadBMP("mars.bmp")) &&         
		 (TextureImage[6]=LoadBMP("wenus.bmp")) &&         
		 (TextureImage[7]=LoadBMP("merkury.bmp")) &&         
		 (TextureImage[8]=LoadBMP("satelitemaska.bmp")) &&
         (TextureImage[9]=LoadBMP("satelite.bmp")) &&
		 (TextureImage[10]=LoadBMP("saturnmaska.bmp")) &&
		 (TextureImage[11]=LoadBMP("saturn.bmp")) &&
		 (TextureImage[12]=LoadBMP("space.bmp")) &&
		 (TextureImage[13]=LoadBMP("sun.bmp")))
    {
        Status=TRUE;

        glGenTextures(14, &texture[0]);                                  
        for(loop=0;loop<14;loop++)
        {
            glBindTexture(GL_TEXTURE_2D, texture[loop]);

            gluBuild2DMipmaps(GL_TEXTURE_2D, 3,TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
        }
        for(loop=0;loop<14;loop++)
        {
            if (TextureImage[loop])                                                    
            {
                if (TextureImage[loop]->data)                                  
                    free(TextureImage[loop]->data);                            
            free(TextureImage[loop]);                                          
            }
        }
    }
    return Status;
}
//////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////Mengubah ukuran tampilan window//////////////////////////////////////////
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		
{
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);						

	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();									
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////Prosuder main OpenGL/////////////////////////////////////////
int InitGL(GLvoid)										
{
    if(!LoadTextures())
        return FALSE;
    Swiatlo();

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Background warna Hitam
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							 
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
  	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    Obiekt1=gluNewQuadric();
    gluQuadricTexture(Obiekt1, GLU_TRUE);
    gluQuadricDrawStyle(Obiekt1, GLU_FILL);  

	return TRUE;										
}
//////////////////////////////////////////////////////////////////////////////////////////////
 
GLint DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
    glLoadIdentity();
    gluLookAt(0,0,10, 0,0,0, 0,1,0);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);			

	//MEMBUAT BACKGROUND    
	glDisable(GL_LIGHTING);

    glTranslatef(0,0,1);
    glRotatef(20,-1,0,0);
    glRotatef(rotate,0,0,0);
    glRotatef(rotate, 0,0,0);
	
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, roll + 0.0f);  glVertex3f(-10.1f, -10.1f, 0.0);
        glTexCoord2f(6.0f, roll + 0.0f);  glVertex3f(10.1f,  -10.1f, 0.0);
        glTexCoord2f(6.0f, roll + 6.0f);  glVertex3f(10.1f,   10.1f, 0.0);
        glTexCoord2f(0.0f, roll + 6.0f);   glVertex3f(-10.1f,  10.1f, 0.0);
    glEnd();
	
	glEnable(GL_LIGHTING);
	//AKHIR MEMBUAT BACKGROUND

//***********************************************************//

	//MEMBUAT PLANET MARS
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTranslatef(-2.6f,-1.8f,-5);
    glRotatef(90,1,0,0);
    glRotatef(rotateobiekt2,0,0,1);
    gluSphere(Obiekt1, 0.9, 20,20);	
	//AKHIE MEMBUAT PLANET MARS

//***********************************************************//


	//MEMBUAT PLANET MERKURY
    glLoadIdentity();
    glTranslatef(4.5,3.5,-10);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glRotatef(rotateobiekt2,1,1,0);
    gluSphere(Obiekt1, 0.1,20,20);
	//AKHIR MEMBUAT MERKURY

//***********************************************************//

	//MEMBUAT PLANET BUMI DAN BULAN
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTranslatef(0,0,-4);
    glTranslatef(0,-1.5,0);

    glDisable(GL_LIGHT1);
    glEnable(GL_LIGHT0);

    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
	
	glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture[2]);

	glTranslatef(EarthPos[0], EarthPos[1], EarthPos[2]);		// Merubah Posisi Bumi
    glRotatef(rotateobiekt1,0,1,0);
    glRotatef(-75,1,0,0);
    gluSphere(Obiekt1,0.7,20,20);

    glTranslatef(1,0,0.5);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glRotatef(rotateobiekt1,1,1,1);
    gluSphere(Obiekt1,0.1,20,20);
	//AKHIR MEMBUAT PLANET DAN BUMI

//***********************************************************//

	//MEMBUAT PLANET SATURNUS
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glTranslatef(0.60f, 0.35f,-2.8f);
	glRotatef(20,0,0,1);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	glScalef(0.1f,0.1f,0.1f);
	glBegin(GL_QUADS);
        glTexCoord2f(0.0 ,  0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0 ,  0.0);  glVertex3f(1.0,  -1.0, 0.0);
        glTexCoord2f(1.0,   1.0);   glVertex3f(1.0,   1.0, 0.0);
        glTexCoord2f(0.0,   1.0);   glVertex3f(-1.0,  1.0, 0.0);
    glEnd();

	glBlendFunc(GL_ONE, GL_ONE);

	
	glBindTexture(GL_TEXTURE_2D, texture[11]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0 ,  0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0 ,  0.0);  glVertex3f(1.0,  -1.0, 0.0);
        glTexCoord2f(1.0,   1.0);   glVertex3f(1.0,   1.0, 0.0);
        glTexCoord2f(0.0,   1.0);   glVertex3f(-1.0,  1.0, 0.0);
    glEnd();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	//AKHIR MEMBUAT PLANET SATURNUS
	
	//ONEMORE
	glLoadIdentity();
	
	glTranslatef(1.8f, 1.0f, -8);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor4f(1,1,1, alpha);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glRotatef(ro2,0,0,1);
	glBindTexture(GL_TEXTURE_2D, texture[12]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0 ,  0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0 ,  0.0);  glVertex3f(1.0,  -1.0, 0.0);
        glTexCoord2f(1.0,   1.0);   glVertex3f(1.0,   1.0, 0.0);
        glTexCoord2f(0.0,   1.0);   glVertex3f(-1.0,  1.0, 0.0);
    glEnd();
	
	glRotatef(ro,0,0,1);
	glBegin(GL_QUADS);
        glTexCoord2f(0.0 ,  0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0 ,  0.0);  glVertex3f(1.0,  -1.0, 0.0);
        glTexCoord2f(1.0,   1.0);   glVertex3f(1.0,   1.0, 0.0);
        glTexCoord2f(0.0,   1.0);   glVertex3f(-1.0,  1.0, 0.0);
    glEnd();

	glDisable(GL_BLEND);	
	glColor4f(1,1,1,1);
	//THAT'S IT
	
	alpha+=alphaplus;

	if(alpha>0.5)
		alphaplus=-alphaplus;
	if(alpha<0.0)
		alphaplus=-alphaplus;

	ro2+=0.1f;
	ro+=0.05f;

//***********************************************************//
	glLoadIdentity();	
	
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);		// Position The Sphere
	glScalef(0.2f, 0.2f, 0.2f);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4f(1.1f, 1.99f, 1.4f, 1.5f);			// Warna cahaya
	
	glBindTexture(GL_TEXTURE_2D, texture[13]);
    
	glBegin(GL_QUADS);
        glTexCoord2f(0.0 ,  0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0 ,  0.0);  glVertex3f(1.0,  -1.0, 0.0);
        glTexCoord2f(1.0,   1.0);   glVertex3f(1.0,   1.0, 0.0);
        glTexCoord2f(0.0,   1.0);   glVertex3f(-1.0,  1.0, 0.0);
    glEnd();
	

	glRotatef(roo,0,0,1);
	
	glBindTexture(GL_TEXTURE_2D, texture[13]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0 ,  0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0 ,  0.0);  glVertex3f(1.0,  -1.0, 0.0);
        glTexCoord2f(1.0,   1.0);   glVertex3f(1.0,   1.0, 0.0);
        glTexCoord2f(0.0,   1.0);   glVertex3f(-1.0,  1.0, 0.0);
    glEnd();
	glColor4f(1,1,1,1);
	glDisable(GL_BLEND);
	
//***********************************************************//

	roo+=0.1f;
	if(roo>360)
		roo=0.0f;	


	rotsat+=0.1f;
	if(rotsat>360)
		rotsat=0;


	RotateWenus+=0.2f;
    if(RotateWenus>360)
        RotateWenus=0;

    TextSync++;
	if (TextSync > 600)
	{
		TextSync = 0;
		Tekst++;
	}
    if (Tekst  > 6)
     	Tekst = 0;

    rotateobiekt1+=step2;
    rotateobiekt2+=0.05f;

    roll+=0.002f;
    if(roll>1.0f)
        roll-=1.0f;
    
	
    roll2+=0.0005f;

    if(roll2>1.0f)
        roll2-=1.0f;


    return TRUE;										// Everything Went OK
}
////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////Proses dari ////////////////////////////////////////
void ProcessKeyboard()									
{
	// Posisi Matahari
	if (keys['L']) LightPos[0] += 0.01f;				// Kanan
	if (keys['J']) LightPos[0] -= 0.01f;				// Kiri

	if (keys['I']) LightPos[1] += 0.01f;				// Atas
	if (keys['K']) LightPos[1] -= 0.01f;				// Bawah

	if (keys['O']) LightPos[2] += 0.01f;				// Depan
	if (keys['U']) LightPos[2] -= 0.01f;				// Belakang

	// Posisi Bumi
	if (keys[VK_RIGHT]) EarthPos[0] += 0.01f;			// Kanan
	if (keys[VK_LEFT]) EarthPos[0] -= 0.01f;			// Kiri

	if (keys[VK_UP]) EarthPos[1] += 0.01f;				// Atas
	if (keys[VK_DOWN]) EarthPos[1] -= 0.01f;				// Bawah

	if (keys['X']) EarthPos[2] += 0.01f;				// Depan
	if (keys['Z']) EarthPos[2] -= 0.01f;				// Belakang

	// Posisi Satelite
	if (keys['D']) SatelitePos[0] += 0.01f;				// Kanan
	if (keys['A']) SatelitePos[0] -= 0.01f;				// Kiri

	if (keys['W']) SatelitePos[1] += 0.01f;				// Atas
	if (keys['S']) SatelitePos[1] -= 0.01f;				// Bawah

	if (keys['E']) SatelitePos[2] += 0.01f;				// Depan
	if (keys['Q']) SatelitePos[2] -= 0.01f;				// Belakang
}
/////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////Menghancurkan Tampilan Window/////////////////////////////////
GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
    gluDeleteQuadric(Obiekt1);    
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////Mengaktifkan Tampilan Window Fullscreen//////////////////////////////////////

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							
		{
			switch (wParam)							
			{
				case SC_SCREENSAVE:					
				case SC_MONITORPOWER:				
				return 0;							
			}
			break;									
		}

		case WM_CLOSE:								
		{
			PostQuitMessage(0);						
			return 0;							
		}

		case WM_KEYDOWN:							
		{
			keys[wParam] = TRUE;					
			return 0;								
		}

		case WM_KEYUP:								
		{
			keys[wParam] = FALSE;					
			return 0;								
		}

		case WM_SIZE:								
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  
			return 0;								
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////Prosedur Pertanyaan////////////////////////////////////////////////////////////
int WINAPI WinMain(	HINSTANCE	hInstance,			
					HINSTANCE	hPrevInstance,		
					LPSTR		lpCmdLine,			
					int			nCmdShow)			
{
	MSG		msg;									
	BOOL	done=FALSE;								


	if (MessageBox(NULL,"Apakah akan dijalankan pada mode layar penuh?", "Mode Layar Penuh?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							
	}


	if (!CreateGLWindow("Kelompok 6 Grafkom IF13",640,480,16,fullscreen))
	{
		return 0;									
	}

	while(!done)									
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{
			if (msg.message==WM_QUIT)				
			{
				done=TRUE;							
			}
			else									
			{
				TranslateMessage(&msg);				
				DispatchMessage(&msg);				
			}
		}
		else										
		{
			
			if (active)								
			{
				if (keys[VK_ESCAPE])				// Tekan ESC untuk keluar program
				{
					done=TRUE;						
				}
				else								
				{
					DrawGLScene();					// Memanggil Prosedur Layar
					SwapBuffers(hDC);				// 
					ProcessKeyboard();				// Memanggil Proses Keyboard
					// Go back and study again.
              	}
			}

			if (keys[VK_F1])						// Tekan F1 untuk merubah tampilan fullscreen menjadi kecil
			{
				keys[VK_F1]=FALSE;					
				KillGLWindow();						
				fullscreen=!fullscreen;				
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("Kelompok 6 Grafkom IF13",640,480,16,fullscreen))
				{
					return 0;						
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Membunuh Tampiilan
	return (msg.wParam);							// Keluar dari Program
}
///////////////////////////////////////////////////////////////////////////////////////////////
