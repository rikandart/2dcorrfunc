#include "2dcorrfunc.h"


float f(float x, float y)
{
	return 0;
}

void ClearACFResult()
{
	for (int i = 0; i < 2 * ARRAY_LENGTH; i++)
	{
		acfResult[i].resize(2*ARRAY_LENGTH);
	}
}

void ACF()
{
	//использовать попозже
	/*float delta = 0.1;
	float tau[1024];
	tau[0]=-102.4;
	for (int i = 0; i <= 1023; i++)
	{
		tau[i + 1] = tau[i] + delta;
	}*/
	//дополнение нулями
	float data_null[3 * ARRAY_LENGTH];

	for (int i = 0; i < 3 * ARRAY_LENGTH; i++)
	{
		if (i < ARRAY_LENGTH)
		{
			data_null[i] = 0;
		}
		
		if (i >= ARRAY_LENGTH && i < 2 * ARRAY_LENGTH)
		{
			data_null[i] = OverlBuffer[i - ARRAY_LENGTH];
		}

		if (i >= 2 * ARRAY_LENGTH)
		{
			data_null[i] = 0;
		}

	}


	for (int tau = 0; tau < 2 * ARRAY_LENGTH; tau+=2)
	{
		for (int freq = 0; freq < 2 * ARRAY_LENGTH; freq++)
		{
			complex<float> tmp(0, 0);
			for (int i = 0; i < 0.5*(ARRAY_LENGTH + tau); i++)
			{
					complex<float> signal(data_null[i * 2], data_null[i * 2 + 1]);
					complex<float> signalConj(OverlBuffer[i * 2], -(OverlBuffer[i * 2 + 1]));
					complex<float> exp(cos(2 * M_PI*freq*i * 2 / (2*ARRAY_LENGTH)), -sin(2 * M_PI*freq*i * 2 / (2*ARRAY_LENGTH)));
					tmp += signal*signalConj*exp;

			}
			if (tau == 254 && freq == 255)
			{
				complex<float> tmp1 = tmp;
				float tmp2 = abs(tmp);
				int p = 0;
			}
			//progress counter
			if (progressStep%(2*ARRAY_LENGTH*2*ARRAY_LENGTH/200) == 0)
			{
				if (progressStep == 0)
				{
					cout << "Progress " << percentage << "%\r";
				}
				else
				{
					percentage++;
					cout << "Progress " << percentage << "%\r";
				}
				
			}
			progressStep++;

			//acfResult assignment
			
			acfResult[tau][freq] = abs(tmp) * sz;

		}
		//signal data buffer resize
		OverlBuffer.resize(ARRAY_LENGTH + tau + 2);
		for (int i = (ARRAY_LENGTH + tau + 2)-1; i > 1; i--)
		{
			OverlBuffer[i] = OverlBuffer[i - 2];
		}
		OverlBuffer[0] = 0;
		OverlBuffer[1] = 0;
	}
	
	
	
}

void ReceivingOverlBuffer()
{
	HANDLE hCreateFile;
	//writefile
	/*BOOL bWritefile;
	char szWriteFileBuffer[1023] = "Hello From NamedPipe Client!";
	DWORD dwWriteBufferSize = sizeof(szWriteFileBuffer);
	DWORD dwNoBytesWrite;*/
	//readfile
	BOOL bReadfile;
	float szReadFileBuffer[ARRAY_LENGTH];
	DWORD dwReadBufferSize = sizeof(szReadFileBuffer);
	DWORD dwNoBytesRead;
	hCreateFile = CreateFile(
		_T("\\\\.\\pipe\\MYNAMEDPIPE"),
		GENERIC_READ /*| GENERIC_WRITE*/,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hCreateFile == INVALID_HANDLE_VALUE)
	{
		cout << "File Creation Failed & Error number is " << GetLastError() << endl;
		system("pause");
	}
	else
	{
		cout << "File Creation succesful" << endl;
	}

	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		bReadfile = ReadFile(
			hCreateFile,
			&szReadFileBuffer[i],
			dwReadBufferSize,
			&dwNoBytesRead,
			NULL);

		if (bReadfile == FALSE)
		{
			cout << "ReadFile Failed & Error number is " << GetLastError() << endl;
			system("pause");
			break;

		}
		else
		{
			OverlBuffer[i] = szReadFileBuffer[i];
			cout << "Data reading from server -> " << OverlBuffer[i] << "\r";
			if (i == ARRAY_LENGTH - 1)
			{
				cout << endl << "ReadFile succesful" << endl;
			}
		}
	}
	

}

void initRendering()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthRange(200, 2000);
	glDepthRange(1, 0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	initRendering();
	for (float x = -1500; x < 1500; x += 50)
	{
		glBegin(GL_LINE_STRIP);
		for (float y = -1200; y < 1200; y += 50)
		{
			glColor3f(0.921, 0.921, 0.921);
			glVertex3f(x, y, f(x, y));
		}
		glEnd();
	}

	for (float y = -1200; y < 1200; y += 50)
	{
		glBegin(GL_LINE_STRIP);
		for (float x = -1500; x < 1500; x += 50)
		{
			glColor3f(0.921, 0.921, 0.921);
			glVertex3f(x, y, f(x, y));
		}
		glEnd();
	}

	for (float x = -1500; x < 1500; x += 50)
	{
		glBegin(GL_LINE_STRIP);
		for (float z = 0; z < 1300; z += 50)
		{
			glColor3f(0.921, 0.921, 0.921);
			glVertex3f(x, f(x, z), z);
		}
		glEnd();
	}

	for (float z = 0; z < 1300; z += 50)
	{
		glBegin(GL_LINE_STRIP);
		for (float x = -1500; x < 1500; x += 50)
		{
			glColor3f(0.921, 0.921, 0.921);
			glVertex3f(x, f(x, z), z);
		}
		glEnd();
	}

	//light cornflover blue
	float rColor = 0.592;
	float gColor = 0.784;
	float bColor = 0.921;

	//HARLEQUIN
	/*float rColor = 0.188;
	float gColor = 0.878;
	float bColor = 0.0;*/

	//black
	/*float rColor = 0;
	float gColor = 0;
	float bColor = 0;*/
	float fMax = 1e-6;

	for (int tau = 0; tau < 2 * ARRAY_LENGTH + 1; tau += 16)
	{
		//разбиваю числа на сотни, десятки и единицы для вывода на экран
		int tauH, tauT, tauU;
		div_t d;
		d = div(tau - ARRAY_LENGTH, 100);
		tauH = d.quot;//целая часть от деления
		d = div(abs(d.rem), 10);
		tauT = d.quot;
		tauU = d.rem;//остаток от деления
		char tauChar[3];
		float zeroFive = 0;
		glColor3f(0, 0, 0);
		//перевод числа в char
		if (tau - ARRAY_LENGTH < 0 && !freqSet)
		{
			glRasterPos3f(5 * (tau - ARRAY_LENGTH) + zeroFive, 0, 450);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '-');
			zeroFive += 10;
		}

		if (freqSet)
		{
			tauH = tauT = 0;
		}

		if (tauH != 0)
		{
			_itoa_s(tauH, tauChar, 10);//10 - система исчисления
			if (tauChar[0] == '-')//если число отрицательное
			{
				/*glRasterPos3f(5 * (tau - ARRAY_LENGTH), 0, 250);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tauChar[0]);
				zeroFive += 10;*/
				glRasterPos3f(5 * (tau - ARRAY_LENGTH) + zeroFive, 0, 450);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tauChar[1]);
				zeroFive += 10;
			}
			else
			{
				glRasterPos3f(5 * (tau - ARRAY_LENGTH), 0, 450);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tauChar[0]);
				zeroFive += 10;
			}
		}
		if (tauT != 0 || (tauH != 0))
		{
			_itoa_s(tauT, tauChar, 10);
			glRasterPos3f(5 * (tau - ARRAY_LENGTH) + zeroFive, 0, 450);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tauChar[0]);
			zeroFive += 10;
		}


		if (!freqSet)
		{
			_itoa_s(tauU, tauChar, 10);
			glRasterPos3f(5 * (tau - ARRAY_LENGTH) + zeroFive, 0, 450);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tauChar[0]);
		}
	}

	for (int freq = 0; freq < 2 * ARRAY_LENGTH + 1; freq += 16)
	{
		//разбиваю числа на сотни, десятки и единицы для вывода на экран
		int freqH, freqT, freqU;
		div_t d;
		d = div(freq - ARRAY_LENGTH, 100);
		freqH = d.quot;//целая часть от деления
		d = div(abs(d.rem), 10);
		freqT = d.quot;
		freqU = d.rem;//остаток от деления
		char freqChar[3];
		float zeroFive = 0;//расстояние между символами
		glColor3f(0, 0, 0);
		if (tauSet)
		{
			freqH = freqT = 0;
		}
		//перевод числа в char
		if (freq - ARRAY_LENGTH < 0 && !tauSet)
		{
			glRasterPos3f(0, 5 * (freq - ARRAY_LENGTH) + zeroFive, 450);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '-');
			zeroFive += 10;
		}
		if (freqH != 0)
		{
			_itoa_s(freqH, freqChar, 10);//10 - система исчисления
			if (freqChar[0] == '-')//если число отрицательное
			{
				/*glRasterPos3f(0, 5 * (freq - ARRAY_LENGTH) + zeroFive, 250);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, freqChar[0]);
				zeroFive += 10;*/
				glRasterPos3f(0, 5 * (freq - ARRAY_LENGTH) + zeroFive, 450);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, freqChar[1]);
				zeroFive += 10;
			}
			else
			{
				glRasterPos3f(0, 5 * (freq - ARRAY_LENGTH) + zeroFive, 450);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, freqChar[0]);
				zeroFive += 10;
			}
		}
		if (freqT != 0 || freqH != 0)
		{
			_itoa_s(freqT, freqChar, 10);
			glRasterPos3f(0, 5 * (freq - ARRAY_LENGTH) + zeroFive, 450);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, freqChar[0]);
			zeroFive += 10;
		}

		if (!tauSet)
		{
			_itoa_s(freqU, freqChar, 10);
			glRasterPos3f(0, 5 * (freq - ARRAY_LENGTH) + zeroFive, 450);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, freqChar[0]);
		}
		
	}

	for (int acfZ = 0; acfZ < 801; acfZ += 100)
	{
		int acfZ_H, acfZ_T, acfZ_U;
		div_t d;
		d = div(acfZ, 100);
		acfZ_H = d.quot;//целая часть от деления
		d = div(abs(d.rem), 10);
		acfZ_T = d.quot;
		acfZ_U = d.rem;//остаток от деления
		char acfZ_Char[3];
		float zeroFive = 0;//расстояние между символами
		glColor3f(0, 0, 0);
		if (!upViewSet)
		{
			if (!freqSet)
			{
				if (acfZ_H != 0)
				{
					_itoa_s(acfZ_H, acfZ_Char, 10);//10 - система исчисления
					if (acfZ_Char[0] == '-')//если число отрицательное
					{
						glRasterPos3f(zeroFive + 10, 0, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
						zeroFive += 10;
						glRasterPos3f(zeroFive + 10, 0, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[1]);
						zeroFive += 10;
					}
					else
					{
						glRasterPos3f(zeroFive + 10, 0, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
						zeroFive += 10;
					}
				}
				if (acfZ_T != 0)
				{
					_itoa_s(acfZ_T, acfZ_Char, 10);
					glRasterPos3f(zeroFive + 10, 0, acfZ);
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
					zeroFive += 10;
				}
				else
				{
					if (acfZ_H != 0)
					{
						_itoa_s(acfZ_T, acfZ_Char, 10);
						glRasterPos3f(zeroFive + 10, 0, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
						zeroFive += 10;
					}
				}

				_itoa_s(acfZ_U, acfZ_Char, 10);
				glRasterPos3f(zeroFive + 10, 0, acfZ);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
			}
			else
			{
				if (acfZ_H != 0)
				{
					_itoa_s(acfZ_H, acfZ_Char, 10);//10 - система исчисления
					if (acfZ_Char[0] == '-')//если число отрицательное
					{
						glRasterPos3f(0, zeroFive + 10, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
						zeroFive += 10;
						glRasterPos3f(zeroFive + 10, 0, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[1]);
						zeroFive += 10;
					}
					else
					{
						glRasterPos3f(0, zeroFive + 10, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
						zeroFive += 10;
					}
				}
				if (acfZ_T != 0)
				{
					_itoa_s(acfZ_T, acfZ_Char, 10);
					glRasterPos3f(0, zeroFive + 10, acfZ);
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
					zeroFive += 10;
				}
				else
				{
					if (acfZ_H != 0)
					{
						_itoa_s(acfZ_T, acfZ_Char, 10);
						glRasterPos3f(0, zeroFive + 10, acfZ);
						glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
						zeroFive += 10;
					}
				}

				_itoa_s(acfZ_U, acfZ_Char, 10);
				glRasterPos3f(0, zeroFive + 10, acfZ);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, acfZ_Char[0]);
			}
		}
		
	}

	for (int tau = 0; tau < 2 * ARRAY_LENGTH; tau += 2)
	{
		glBegin(GL_LINE_STRIP);

		for (int freq = 0; freq < 2 * ARRAY_LENGTH; freq++)
		{
			/*if (acfResult[tau][freq] > fMax)
			{
			float step = abs(acfResult[tau][freq] / fMax);
			fMax = f2(tau - 128, freq - 128);
			rColor += step*(5e-6);
			gColor -= step*(5e-6);
			bColor -= step*(5e-6);
			glColor3f(rColor, gColor, bColor);
			}
			else if (acfResult[tau][freq] == fMax)
			{
			glColor3f(rColor, gColor, bColor);
			}
			else if (f2(tau - 128, freq - 128) < fMax)
			{
			float step;
			if (f2(tau - 128, freq - 128) == 0)
			{
			step = abs(fMax / 1e-6);
			}
			else
			{
			step = abs(fMax / acfResult[tau][freq]);
			}
			rColor -= step*(5e-6);
			gColor += step*(5e-6);
			bColor += step*(5e-6);
			}
			//step for cornflower blue
			/*rColor += (5.49e-6);
			gColor -= (2.64e-6);
			bColor -= (7.66e-6);*/
			glColor3f(rColor, gColor, bColor);
			//glColor3f(0.592, 0.784, 0.921);
			//to
			//glColor3f(0.952, 0.611, 0.419);
			glVertex3f(5 * (tau - ARRAY_LENGTH), 5 * (freq - ARRAY_LENGTH), acfResult[tau][freq]);
		}
		glEnd();
	}

	glutSwapBuffers();
}

void timer(int = 0)
{
	display();
	glutTimerFunc(10, timer, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == '0')
	{
			if (wasAnabled)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glFrustum(-150, 150, -150, 150, 200, 2000);
				gluLookAt(-200, -400, 200, 0, 0.0, 0.0, 0.0, 0.0, 1.0);
				glScalef(0.4, 0.4, 0.4);
				if (keyCheckPlus != 0){ keyCheckPlus = 0; }
				if (keyCheckMinus != 0){ keyCheckMinus = 0; }
				tauSet = freqSet = upViewSet = false;
				wasAnabled = false;
			}
		}
	if (key == '+')
	{
		//можно нажать только 4 раза
		if (keyCheckMinus != 0)
		{
			if (keyCheckPlus < 4 + keyCheckMinus)
			{
				glScalef(1.3, 1.3, 1.3);
				keyCheckPlus++;
			}
			if (keyCheckPlus == 4 + keyCheckMinus)
			{
				keyCheckPlus = keyCheckPlus - keyCheckMinus;
				keyCheckMinus = 0;
				
			}
		}
		else
		{
			if (keyCheckPlus < 4)
			{
				glScalef(1.3, 1.3, 1.3);
				keyCheckPlus++;
			}
		}
		wasAnabled = true;

	}
	if (key == '=')
	{ 
		if (keyCheckMinus != 0)
		{
			if (keyCheckPlus < 4 + keyCheckMinus)
			{
				glScalef(1.3, 1.3, 1.3);
				keyCheckPlus++;
			}
			if (keyCheckPlus == 4 + keyCheckMinus)
			{
				keyCheckPlus = keyCheckPlus - keyCheckMinus;
				keyCheckMinus = 0;
			}
		}
		else
		{
			if (keyCheckPlus < 4)
			{
				glScalef(1.3, 1.3, 1.3);
				keyCheckPlus++;
			}
		}
		wasAnabled = true;
	}
	if (key == '-')
	{ 
		if (keyCheckPlus != 0)
		{
			if (keyCheckMinus < 3 + keyCheckPlus)
			{
				glScalef(0.77, 0.77, 0.77);
				keyCheckMinus++;
			}
			if (keyCheckMinus == 3 + keyCheckPlus)
			{
				keyCheckMinus = keyCheckMinus - keyCheckPlus;
				keyCheckPlus = 0;
			}
		}
		else
		{
			if (keyCheckMinus < 3)
			{
				glScalef(0.77, 0.77, 0.77);
				keyCheckMinus++;
			}
		}
		wasAnabled = true;		
	}
	if (key == 'r'){ glRotatef(5, 0, 1, 0);}
	if (key == 't'){ glRotatef(-5, 0, 1, 0); }
	if (key == 'f'){ glRotatef(5, 1, 0, 0); }
	if (key == 'g'){ glRotatef(-5, 1, 0, 0); }
	if (key == '5')
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-150, 150, -150, 150, 200, 2000);
		gluLookAt(0, 0, 1000, 0.0, 0.0, 0.0, -1.0, 0, 0);
		if (keyCheckPlus != 0){ keyCheckPlus = 0; }
		if (keyCheckMinus != 0){ keyCheckMinus = 0; }
		upViewSet = true;
		tauSet = freqSet = false;
		wasAnabled = true;	
	}
	if (key == '6')
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-150, 150, -150, 150, 200, 2000);
		gluLookAt(0, -800, 200, 0.0, 0.0, 200, 0, 0, 1);
		
		if (keyCheckPlus != 0){ keyCheckPlus = 0; }
		if (keyCheckMinus != 0){ keyCheckMinus = 0; }
		tauSet = true;
		freqSet = upViewSet = false;
		wasAnabled = true;

	}
	if (key == '4')
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-150, 150, -150, 150, 200, 2000);
		gluLookAt(800, 0, 200, 0.0, 0.0, 200, 0, 0, 1);
		if (keyCheckPlus != 0){ keyCheckPlus = 0; }
		if (keyCheckMinus != 0){ keyCheckMinus = 0; }
		freqSet = true;
		tauSet = upViewSet = false;
		wasAnabled = true;
	}
	glutPostRedisplay();
}



int main(int argc, char **argv)
{
	//2d ACF calculating block
	ClearACFResult();
	OverlBuffer.resize(ARRAY_LENGTH);
	ReceivingOverlBuffer();
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		OverlSum += abs(OverlBuffer[i]);
	}
	/*if (OverlSum / ARRAY_LENGTH < 50000)
	{
		size = 3 * 10e7;
	}
	else	if (OverlSum / ARRAY_LENGTH < 200000)
	{
		size = 3*10e8;
	}
	else if (OverlSum / ARRAY_LENGTH > 200000 && OverlSum / ARRAY_LENGTH < 1000000)
	{
		size = 3 * 10e9;
	}*/
	cout << endl << "Start calculating 2 dimensional correlation function" << endl << endl;
	ACF();
	cout << endl << endl;
	cout << "Hot keys: " << endl;
	cout << "5 - Up view, 6 - Tau dependency, 4 - Frequency dependency, 0 - Reset view" << endl;
	cout << "+ - Zoom in" << endl;
	cout << "- - Zoom out" << endl;
	cout << "Use mouse to rotate image" << endl << endl;
	system("pause");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(50, 86);
	glutCreateWindow("2D correlation function");
	glClearColor(1,1,1,1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-150, 150, -150, 150, 200, 2000);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);//функция захвата мыши
	glutKeyboardFunc(keyboard);
	gluLookAt(-200,-400,200,0,0.0,0.0,0.0,0.0,1.0);
	glScalef(0.4, 0.4, 0.4);
	//glMatrixMode(GL_MODELVIEW);

	glutDisplayFunc(display);
	timer();
	glutMainLoop();


	return 0;
}
