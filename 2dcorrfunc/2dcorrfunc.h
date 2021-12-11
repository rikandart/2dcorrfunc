#pragma once
#define _USE_MATH_DEFINES

#include <windows.h> 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <complex>
#include <vector>
#include <string>
#include <glut.h>

#define  ARRAY_LENGTH 256

using namespace std;

vector<vector<float>> acfResult(2*ARRAY_LENGTH);
vector<float> OverlBuffer;
float sz = 1 * 10e3;
float OverlSum = 0;
int percentage = 0;
int progressStep = 0;
float angleX = 0;
float angleY = 0;
int oldX = 0;
int oldY = 0;
bool wasAnabled = false;
int keyCheckPlus = 0;//cм функцию keyboard
int keyCheckMinus = 0;
bool tauSet, freqSet, upViewSet = false;
void mouse(int button, int state, int x, int y)
{
	GLdouble objX, objY, objZ;
	GLdouble matModelView[16], matProjection[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLfloat winX = x;
	GLfloat winY = viewport[3] - y;
	GLfloat winZ = 0;
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	cout << winX << " " << winY << " " << winZ << " " <<endl;
	gluUnProject(winX, winY, winZ, matModelView, matProjection, viewport, &objX, &objY, &objZ);
	cout << objX << " " << objY << " " << objZ << " " <<endl;
}

void motion(int x, int y)
{
	if (abs(x - oldX) > 20)
	{
		angleX += 2*(x - oldX);
		if (x < oldX)
		{
			glRotatef(angleX / 360, 0, 0, 1);
		}
		else
		{
			glRotatef(-angleX / 500, 0, 0, 1);
		}
		oldX = x;
	}

	if (abs(y - oldY) > 20)
	{
		//angleY += 2*(y - oldY);
		if (y < oldY)
		{
			glRotatef(angleY/500, 1, 1, 0);
		}
		else
		{
			glRotatef(-angleY/500, 1, 1, 0);
		}
		oldY = y;
	}
	wasAnabled = true;
}