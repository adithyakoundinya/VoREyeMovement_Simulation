#include <stdio.h>
#include <iostream>
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl\glut.h>
using namespace std;
#define PI 3.141582
#include "SOIL.h"
#include <Windows.h>

const int M = 18;
const int N = 20;
int _InputData[N][M];
float _MovementAngleData[N][M];
float _ScenarioPositions[N][M];
bool flag = 0;
int p = 0;
int numberOfPoints = 0;

float Vertex[4][3] = {
	-2.0, 5.0, 0.0,
	-2.0, 1.0, 0.0,
	2.0, 1.0, 0.0,
	2.0, 5.0, 0.0
};

float VertexB[4][3] = {
	-2.0, 5.0, -2.0,
	-2.0, 1.0, -2.0,
	2.0, 1.0, -2.0,
	2.0, 5.0, -2.0
};

int width = 1000, height = 800;
float mousePosx, mousePosy;
float CamPosx = 0.0, CamPosy = 0.0, CamPosz = 15.0;
float CamPosx2 = 0.0, CamPosy2 = 0.0, CamPosz2 = 15.0;
float CamLookatX = 0.0, CamLookatY = 0.0, CamLookatZ = 0.0;
float CamLookatX2 = 0.0, CamLookatY2 = 0.0, CamLookatZ2 = 0.0;
float CamUpX = 0.0, CamUpY = 1.0, CamUpZ = 0.0;
float frustrumAngleX = 0.0;
float frustrumAngleY = 0.0;
float frustrumAngleX2 = 0.0;
float frustrumAngleY2 = 0.0;
int verticalCount = 0, HorizontalCount = 0;
bool movementDataAvailable = false;
bool setSimulationFlag = false;
int pointIndex = 0;
int gx = 0, gy = 0, gz = 0;
float tX = 0, tY = 0, tZ = 0;
float tWithoutVorX = 0, tWithoutVorY = 0, tWithoutVorZ = 0;
int tracingSteps = 0, retractionSteps = 0;
int totalStepsToMove = 0, grandTotalStepsToMove = 0;
void executeMovement(int buttonType, float angle, float initialCompansateAngle);
int totalFlag = 1;
int stepSize = 25;
int stepCount = 0;
int xbutton, ybutton;
bool firstPoint = true;
float initialCompansateAngleY, initialCompansateAngleX;

GLuint	texture[10];			// Storage For One Texture ( NEW )
GLuint	skyboxTex[6];
float SkySize = 1000.0;
GLfloat Acoord = 1023.0 / 1024.0, Bcoord = 1.0 / 1024.0;

void addLighting();

int LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	int i;
	char file1[40];
	char file2[40];
	char *filename;

	for (int i = 1; i < 11; i++) {
		strcpy(file1, "Data/");
		_itoa(i, file2, 10);
		strcat(file2, ".bmp");
		filename = strcat(file1, file2);
		//printf("file data : %s\n", filename);

		texture[i - 1] = SOIL_load_OGL_texture	/* load an image file directly as a new OpenGL texture */
		(
			filename,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
		);

		if (texture[i - 1] == 0) {
			printf("false");
			return false;
		}
	}
	//Skybox texture loading
	/*skyboxTex[0] = SOIL_load_OGL_texture("Data/sky_up.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[1] = SOIL_load_OGL_texture("Data/sky_down.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[2] = SOIL_load_OGL_texture("Data/sky_left.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[3] = SOIL_load_OGL_texture("Data/sky_right.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[4] = SOIL_load_OGL_texture("Data/sky_front.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[5] = SOIL_load_OGL_texture("Data/sky_back.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);*/

	skyboxTex[0] = SOIL_load_OGL_texture("Data/top.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[1] = SOIL_load_OGL_texture("Data/down.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[2] = SOIL_load_OGL_texture("Data/left.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[3] = SOIL_load_OGL_texture("Data/right.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[4] = SOIL_load_OGL_texture("Data/front.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	skyboxTex[5] = SOIL_load_OGL_texture("Data/back.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	// Typical Texture Generation Using Data From The Bitmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	return true;										// Return Success
}

void DrawSkybox() {
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	//front
	glBindTexture(GL_TEXTURE_2D, skyboxTex[4]);
	glBegin(GL_QUADS);
		glTexCoord2f(Bcoord, Bcoord);	glVertex3f(-SkySize, -SkySize, -SkySize);
		glTexCoord2f(Acoord, Bcoord);	glVertex3f(SkySize, -SkySize, -SkySize);
		glTexCoord2f(Acoord, Acoord);	glVertex3f(SkySize, SkySize, -SkySize);
		glTexCoord2f(Bcoord, Acoord);	glVertex3f(-SkySize, SkySize, -SkySize);
	glEnd();

	//back
	glBindTexture(GL_TEXTURE_2D, skyboxTex[5]);
	glBegin(GL_QUADS);
		glTexCoord2f(Bcoord, Bcoord);	glVertex3f(SkySize, -SkySize, SkySize);
		glTexCoord2f(Acoord, Bcoord);	glVertex3f(-SkySize, -SkySize, SkySize);
		glTexCoord2f(Acoord, Acoord);	glVertex3f(-SkySize, SkySize, SkySize);
		glTexCoord2f(Bcoord, Acoord);	glVertex3f(SkySize, SkySize, SkySize);
	glEnd();

	//left
	glBindTexture(GL_TEXTURE_2D, skyboxTex[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(Bcoord, Bcoord);	glVertex3f(SkySize, -SkySize, -SkySize);
		glTexCoord2f(Acoord, Bcoord);	glVertex3f(SkySize, -SkySize, SkySize);
		glTexCoord2f(Acoord, Acoord);	glVertex3f(SkySize, SkySize, SkySize);
		glTexCoord2f(Bcoord, Acoord);	glVertex3f(SkySize, SkySize, -SkySize);
	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, skyboxTex[3]);
	glBegin(GL_QUADS);
		glTexCoord2f(Bcoord, Bcoord);	glVertex3f(-SkySize, -SkySize, SkySize);
		glTexCoord2f(Acoord, Bcoord);	glVertex3f(-SkySize, -SkySize, -SkySize);
		glTexCoord2f(Acoord, Acoord);	glVertex3f(-SkySize, SkySize, -SkySize);
		glTexCoord2f(Bcoord, Acoord);	glVertex3f(-SkySize, SkySize, SkySize);
	glEnd();

	//up
	glBindTexture(GL_TEXTURE_2D, skyboxTex[0]);
	glBegin(GL_QUADS);
		glTexCoord2f(Bcoord, Bcoord);	glVertex3f(-SkySize, SkySize, -SkySize);
		glTexCoord2f(Acoord, Bcoord);	glVertex3f(SkySize, SkySize, -SkySize);
		glTexCoord2f(Acoord, Acoord);	glVertex3f(SkySize, SkySize, SkySize);
		glTexCoord2f(Bcoord, Acoord);	glVertex3f(-SkySize, SkySize, SkySize);
	glEnd();

	//down
	glBindTexture(GL_TEXTURE_2D, skyboxTex[1]);
	glBegin(GL_QUADS);
		glTexCoord2f(Bcoord, Bcoord);	glVertex3f(-SkySize, -SkySize, SkySize);
		glTexCoord2f(Acoord, Bcoord);	glVertex3f(SkySize, -SkySize, SkySize);
		glTexCoord2f(Acoord, Acoord);	glVertex3f(SkySize, -SkySize, -SkySize);
		glTexCoord2f(Bcoord, Acoord);	glVertex3f(-SkySize, -SkySize, -SkySize);
	glEnd();

	//down for sky
	/*glBindTexture(GL_TEXTURE_2D, skyboxTex[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(Bcoord, Acoord);	glVertex3f(-SkySize, -SkySize, SkySize);
	glTexCoord2f(Bcoord, Bcoord);	glVertex3f(SkySize, -SkySize, SkySize);
	glTexCoord2f(Acoord, Bcoord);	glVertex3f(SkySize, -SkySize, -SkySize);
	glTexCoord2f(Acoord, Acoord);	glVertex3f(-SkySize, -SkySize, -SkySize);
	glEnd();*/

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glFrontFace(GL_CCW);
}
// NO Change//
void makeMovementWithVOR(int value)
{
	// Make the EyeBall Movement by ________ Degrees
	// Stop EyeBall Movement 
	// Make EyeBall Movement and HeadMovement by ___________ Degrees (till the object is at center or when the head limit is reached)
	// Stop EyeBall Movement and HeadMovement
	// if(Object is found)
	//	retract the eyeball near the object
	// else
	//	move the eye ball in the direction of object
	// do the process until head limit is reached or until the object is found by eyeball or untill the eyecall limit is reached. 

	// Move the Gaze Point towards the target point with speed
	if (setSimulationFlag && pointIndex <= numberOfPoints - 1)
	{
		if (stepCount < stepSize)
		{
			if (_MovementAngleData[pointIndex][0] < 0)
				xbutton = 1;
			else
				xbutton = 2;
			if (_MovementAngleData[pointIndex][1] > 0)
				ybutton = 3;
			else
				ybutton = 4;


			//executeMovement(movementDirection,angleWithVOR,AngleWithoutVOR);
			if (stepCount > 12)
			{
				executeMovement(xbutton, -_MovementAngleData[pointIndex][2], -_MovementAngleData[pointIndex][2] * 1.65);
				executeMovement(ybutton, _MovementAngleData[pointIndex][3], _MovementAngleData[pointIndex][3] * 1.65);
			}
			else if (pointIndex == 0)
			{
				executeMovement(xbutton, -_MovementAngleData[pointIndex][2] * 1.65, -_MovementAngleData[pointIndex][2] * 1.65);
				executeMovement(ybutton, _MovementAngleData[pointIndex][3] * 1.65, _MovementAngleData[pointIndex][3] * 1.65);
			}
			else
			{
				executeMovement(xbutton, -_MovementAngleData[pointIndex][2] * 1.65, -_MovementAngleData[pointIndex][2] * 1.65);
				executeMovement(ybutton, _MovementAngleData[pointIndex][3] * 1.65, _MovementAngleData[pointIndex][3] * 1.65);
			}
			stepCount = stepCount + 1;
		}
		if (stepCount == stepSize)
		{
			pointIndex = pointIndex + 1;
			stepCount = 0;
			Sleep(2000);
		}
	}
	glutTimerFunc(100, makeMovementWithVOR, 1);
}

//No Change//
void computeMovement()
{
	int max, min;
	float centerX = 0.0, centerY = 0.0;
	for (int i = 0; i < numberOfPoints; i++)
	{
		if (i == 0)
		{
			centerX = 0;
			centerY = 0;
		}
		else
		{
			centerX = _InputData[i - 1][0]; // assigning new center for the set of points
			centerY = _InputData[i - 1][1]; // assigning new center for the set of points
		}

		_MovementAngleData[i][0] = _InputData[i][0] - centerX;
		_MovementAngleData[i][1] = _InputData[i][1] - centerY;
		_MovementAngleData[i][2] = _MovementAngleData[i][0] / stepSize;
		_MovementAngleData[i][3] = _MovementAngleData[i][1] / stepSize;
		//printf("%d,%d = %f,%f,%f,%f\n", _InputData[i][0], _InputData[i][1], _MovementAngleData[i][0], _MovementAngleData[i][1], _MovementAngleData[i][2], _MovementAngleData[i][3]);
	}
	//movementDataAvailable = true;
}

//No Change //
void computeScenarioPosition()
{
	for (int i = 0; i<numberOfPoints; i++)
	{
		_ScenarioPositions[i][0] = _InputData[i][0];
		_ScenarioPositions[i][1] = _InputData[i][1];
	}
}

// No Change //
void ReadFile()
{
	int k = 0, j = 0;
	FILE *fp = fopen("test_4pointoriginal.csv", "r");
	const char s[2] = ",";
	char *token;
	int i;
	if (fp != NULL)
	{
		char line[20];
		while (fgets(line, sizeof line, fp) != NULL)
		{
			token = strtok(line, ",");
			for (i = 0; i<2; i++)
			{
				if (i == 0)
				{
					_InputData[j][k++] = atoi(token);
					token = strtok(NULL, s);
				}
				else {
					_InputData[j++][k] = atoi(token);
					k = 0;
				}
			}
		}
		fclose(fp);
		numberOfPoints = j;
	}
	else {
		perror("test.csv");
	}

}

void InitGraphics(void)
{
	glFrontFace(GL_CCW);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	addLighting();
}

void reShape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, 1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}


// No Change //
void executeMovement(int buttonType, float angle, float angle2)
{
	float tempX, tempY, tempZ;
	//int tWithoutVorCompansate,tXCompansate;
	CamLookatX -= CamPosx;
	CamLookatY -= CamPosy;
	CamLookatZ -= CamPosz;
	CamLookatX2 -= CamPosx2;
	CamLookatY2 -= CamPosy2;
	CamLookatZ2 -= CamPosz2;

	if (pointIndex == 0)
	{
		angle = angle2;
	}

	switch (buttonType)
	{
	case 1:
		/************************************ VoR *************************************************/
		tempX = CamLookatX * cosf(angle * PI / 180.0) + CamLookatZ * sinf(angle * PI / 180.0);
		tempZ = -CamLookatX * sinf(angle * PI / 180.0) + CamLookatZ * cosf(angle * PI / 180.0);
		CamLookatX = tempX;
		CamLookatZ = tempZ;

		//tWithoutVorX =  tWithoutVorX - (abs(angle)*0.25*initialCompansateAngle); // Move the gaze Point without VOR
		if (pointIndex == 0)
		{
			frustrumAngleX = frustrumAngleX + angle;
			tX = tX - (abs(angle)*0.25);
		}
		else
		{
			frustrumAngleX = frustrumAngleX + angle * 0.5;
		}
		if (abs(stepCount) < abs(stepSize / 2) && pointIndex != 0)
		{

			tX = tX - (abs((angle / 1.65) * 2)*0.40);
		}
		/*********************************** End *************************************************/
		/************************************ FixedEye *************************************************/
		tempX = CamLookatX2 * cosf(angle2 * PI / 180.0) + CamLookatZ2 * sinf(angle2 * PI / 180.0);
		tempZ = -CamLookatX2 * sinf(angle2 * PI / 180.0) + CamLookatZ * cosf(angle2 * PI / 180.0);
		CamLookatX2 = tempX;
		CamLookatZ2 = tempZ;
		frustrumAngleX2 = frustrumAngleX2 + angle2;
		tWithoutVorX = tWithoutVorX - (abs(angle2)*0.25); // Move the gaze Point without VOR
														  /*if(abs(stepCount) < abs(stepSize/2))
														  tX =  tX - (abs(angle*2)*0.40);*/
														  /*********************************** End *************************************************/
		break;

	case 2:
		/************************************ VoR *************************************************/
		tempX = CamLookatX * cosf(angle * PI / 180.0) + CamLookatZ * sinf(angle * PI / 180.0);
		tempZ = -CamLookatX * sinf(angle * PI / 180.0) + CamLookatZ * cosf(angle * PI / 180.0);
		CamLookatX = tempX;
		CamLookatZ = tempZ;

		if (pointIndex == 0)
		{
			frustrumAngleX = frustrumAngleX + angle;
			tX = tX + (abs(angle)*0.25);
		}
		else
		{
			frustrumAngleX = frustrumAngleX + angle * 0.5;
		}
		if (abs(stepCount) < abs(stepSize / 2) && pointIndex != 0)
			tX = tX + (abs((angle / 1.65) * 2)*0.40);
		/*********************************** End *************************************************/
		/************************************ FixedEye *************************************************/
		tempX = CamLookatX2 * cosf(angle2 * PI / 180.0) + CamLookatZ2 * sinf(angle2 * PI / 180.0);
		tempZ = -CamLookatX2 * sinf(angle2 * PI / 180.0) + CamLookatZ2 * cosf(angle2 * PI / 180.0);
		CamLookatX2 = tempX;
		CamLookatZ2 = tempZ;
		frustrumAngleX2 = frustrumAngleX2 + angle2;
		tWithoutVorX = tWithoutVorX + (abs(angle2)*0.25);  // Move the gaze Point without VOR
														   /*if(abs(stepCount) < abs(stepSize/2))
														   tX =  tX + (abs(angle*2)*0.40);*/
														   /*********************************** End *************************************************/
		break;

	case 3:
		/************************************ VoR *************************************************/
		tempY = CamLookatY * cosf(angle * PI / 180.0) - CamLookatZ * sinf(angle * PI / 180.0);
		tempZ = CamLookatZ * cosf(angle * PI / 180.0) + CamLookatY * sinf(angle * PI / 180.0);
		CamLookatY = tempY;
		CamLookatZ = tempZ;

		if (pointIndex == 0)
		{
			frustrumAngleY = frustrumAngleY + angle;
			tY = tY + (abs(angle)*0.25);
		}
		else
		{
			frustrumAngleY = frustrumAngleY + angle * 0.5;
		}
		if (abs(stepCount) < abs(stepSize / 2) && pointIndex != 0)
			tY = tY + (abs((angle / 1.65) * 2)*0.40);
		/*********************************** End *************************************************/
		/************************************ FixedEye *************************************************/
		tempY = CamLookatY2 * cosf(angle2 * PI / 180.0) - CamLookatZ2 * sinf(angle2 * PI / 180.0);
		tempZ = CamLookatZ2 * cosf(angle2 * PI / 180.0) + CamLookatY2 * sinf(angle2 * PI / 180.0);
		CamLookatY2 = tempY;
		CamLookatZ2 = tempZ;
		frustrumAngleY2 = frustrumAngleY2 + angle2;
		tWithoutVorY = tWithoutVorY + (abs(angle2)*0.25);  // Move the gaze Point without VOR
														   /*if(abs(stepCount) < abs(stepSize/2))
														   tY =  tY + (abs(angle*2)*0.40);*/
														   /*********************************** End *************************************************/
		break;

	case 4:
		/************************************ VoR *************************************************/
		tempY = CamLookatY * cosf(angle * PI / 180.0) - CamLookatZ * sinf(angle * PI / 180.0);
		tempZ = CamLookatZ * cosf(angle * PI / 180.0) + CamLookatY * sinf(angle * PI / 180.0);
		CamLookatY = tempY;
		CamLookatZ = tempZ;

		if (pointIndex == 0)
		{
			frustrumAngleY = frustrumAngleY + angle;
			tY = tY - (abs(angle)*0.25);
		}
		else
		{
			frustrumAngleY = frustrumAngleY + angle * 0.5;
		}
		if (abs(stepCount) < abs(stepSize / 2) && pointIndex != 0)
			tY = tY - (abs((angle / 1.65) * 2)*0.40);
		/*********************************** End *************************************************/
		/************************************ FixedEye *************************************************/
		tempY = CamLookatY2 * cosf(angle2 * PI / 180.0) - CamLookatZ2 * sinf(angle2 * PI / 180.0);
		tempZ = CamLookatZ2 * cosf(angle2 * PI / 180.0) + CamLookatY2 * sinf(angle2 * PI / 180.0);
		CamLookatY2 = tempY;
		CamLookatZ2 = tempZ;
		frustrumAngleY2 = frustrumAngleY2 + angle2;
		tWithoutVorY = tWithoutVorY - (abs(angle2)*0.25);  // Move the gaze Point without VOR
														   /*if(abs(stepCount) < abs(stepSize/2))
														   tY =  tY  -(abs(angle*2)*0.40);*/
														   /*********************************** End *************************************************/
		break;

	default:
		break;
	}

	CamLookatX += CamPosx;
	CamLookatY += CamPosy;
	CamLookatZ += CamPosz;
	CamLookatX2 += CamPosx2;
	CamLookatY2 += CamPosy2;
	CamLookatZ2 += CamPosz2;
	//glutPostRedisplay();

	//cout << "after X, Y, Z : " << CamLookatX << "," << CamLookatY << "," << CamLookatZ << endl;
}


void drawGrazePoint(float radius)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	//Set Drawing Color - Will Remain this color until otherwise specified
	glColor3f(0.3, 0.9, 0.3);  //Some type of blue
	glTranslatef(0, 0, 0.5);
	//Draw Circle
	glBegin(GL_POLYGON);
	//Change the 6 to 12 to increase the steps (number of drawn points) for a smoother circle
	//Note that anything above 24 will have little affect on the circles appearance
	//Play with the numbers till you find the result you are looking for
	//Value 1.5 - Draws Triangle
	//Value 2 - Draws Square
	//Value 3 - Draws Hexagon
	//Value 4 - Draws Octagon
	//Value 5 - Draws Decagon
	//Notice the correlation between the value and the number of sides
	//The number of sides is always twice the value given this range
	for (double i = 0; i < 2 * PI; i += PI / 24) //<-- Change this Value
		glVertex3f(cos(i) * radius, sin(i) * radius, 0.0);
	glEnd();
	//Draw Circle

	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DrawRect()
{
	GLfloat material_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat material_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat material_specular[] = { 0.0, 0.00, 0.00, 1.0 };
	GLfloat material_shininess[] = { 25.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	
	
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);		//Front face
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(Vertex[0][0], Vertex[0][1], Vertex[0][2]);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(Vertex[1][0], Vertex[1][1], Vertex[1][2]);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(Vertex[2][0], Vertex[2][1], Vertex[2][2]);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(Vertex[3][0], Vertex[3][1], Vertex[3][2]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);		//Side face1
	glNormal3f(-1, 0, 0);
	glVertex3f(VertexB[0][0], VertexB[0][1], VertexB[0][2]);
	glVertex3f(VertexB[1][0], VertexB[1][1], VertexB[1][2]);
	glVertex3f(Vertex[1][0], Vertex[1][1], Vertex[1][2]);
	glVertex3f(Vertex[0][0], Vertex[0][1], Vertex[0][2]);
	glEnd();

	
	glBegin(GL_POLYGON);		//Side face2
	glNormal3f(0, -1, 0);
	glVertex3f(Vertex[1][0], Vertex[1][1], Vertex[1][2]);
	glVertex3f(VertexB[1][0], VertexB[1][1], VertexB[1][2]);
	glVertex3f(VertexB[2][0], VertexB[2][1], VertexB[2][2]);
	glVertex3f(Vertex[2][0], Vertex[2][1], Vertex[2][2]);
	glEnd();

	
	glBegin(GL_POLYGON);		//Side face3
	glNormal3f(1, 0, 0);
	glVertex3f(Vertex[3][0], Vertex[3][1], Vertex[3][2]);
	glVertex3f(Vertex[2][0], Vertex[2][1], Vertex[2][2]);
	glVertex3f(VertexB[2][0], VertexB[2][1], VertexB[2][2]);
	glVertex3f(VertexB[3][0], VertexB[3][1], VertexB[3][2]);
	glEnd();

	
	glBegin(GL_POLYGON);		//Side face4
	glNormal3f(0, 1, 0);
	glVertex3f(Vertex[3][0], Vertex[3][1], Vertex[3][2]);
	glVertex3f(VertexB[3][0], VertexB[3][1], VertexB[3][2]);
	glVertex3f(VertexB[0][0], VertexB[0][1], VertexB[0][2]);
	glVertex3f(Vertex[0][0], Vertex[0][1], Vertex[0][2]);
	glEnd();

	glBegin(GL_POLYGON);		//Back face
	glNormal3f(0, 0, -1);
	glVertex3f(VertexB[3][0], VertexB[3][1], VertexB[3][2]);
	glVertex3f(VertexB[2][0], VertexB[2][1], VertexB[2][2]);
	glVertex3f(VertexB[1][0], VertexB[1][1], VertexB[1][2]);
	glVertex3f(VertexB[0][0], VertexB[0][1], VertexB[0][2]);
	glEnd();
}

void DrawGrid()
{
	GLfloat material_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat material_diffuse[] = { 0.7, 0.749, 0.0627, 1.0 };
	GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_shininess[] = { 5.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	float GridScale = 20.0f;
	float Step = 1.0f;

	glColor3f(0.2, 0.2, 0.2);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	for (int i = 0; i <= GridScale; i++) {
		glVertex3f(-GridScale / 2.0, 0, GridScale / 2.0 - i);	//X-axis parallel lines
		glVertex3f(GridScale / 2.0, 0, GridScale / 2.0 - i);

		glVertex3f(-GridScale / 2.0 + i, 0, GridScale / 2.0);	//Z-axis parallel lines
		glVertex3f(-GridScale / 2.0 + i, 0, -GridScale / 2.0);
	}
	glEnd();
}

void drawCamera()
{

}

void drawScenario()
{
	glMatrixMode(GL_MODELVIEW);
	//glDisable(GL_LIGHTING);
	for (int i = 0; i<numberOfPoints; i++)
	{
		
		glPushMatrix();
		//glColor3f(0.2+i, 0.3+i, 0.5+i);

		glScalef(0.4, 0.4, 1);
		
		glTranslatef(0, -3, 0);
		glTranslatef(_ScenarioPositions[i][0], _ScenarioPositions[i][1], 0); // Adjusting the values of the scenarios
		
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		
		DrawRect();
		glPopMatrix();
		
		//glutPostRedisplay();
	}
	
}

void drawFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
	float DEG2RAD = 3.141593f / 180;
	float tangent = tanf(fovY / 2 * DEG2RAD);
	float nearHeight = nearPlane * tangent;
	float nearWidth = nearHeight * aspectRatio;
	float farHeight = farPlane * tangent;
	float farWidth = farHeight * aspectRatio;

	// compute 8 vertices of the frustum
	float vertices[8][3];
	// near top right
	vertices[0][0] = nearWidth;     vertices[0][1] = nearHeight;    vertices[0][2] = -nearPlane;
	// near top left
	vertices[1][0] = -nearWidth;    vertices[1][1] = nearHeight;    vertices[1][2] = -nearPlane;
	// near bottom left
	vertices[2][0] = -nearWidth;    vertices[2][1] = -nearHeight;   vertices[2][2] = -nearPlane;
	// near bottom right
	vertices[3][0] = nearWidth;     vertices[3][1] = -nearHeight;   vertices[3][2] = -nearPlane;
	// far top right
	vertices[4][0] = farWidth;      vertices[4][1] = farHeight;     vertices[4][2] = -farPlane;
	// far top left
	vertices[5][0] = -farWidth;     vertices[5][1] = farHeight;     vertices[5][2] = -farPlane;
	// far bottom left
	vertices[6][0] = -farWidth;     vertices[6][1] = -farHeight;    vertices[6][2] = -farPlane;
	// far bottom right
	vertices[7][0] = farWidth;      vertices[7][1] = -farHeight;    vertices[7][2] = -farPlane;

	float colorLine1[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	float colorLine2[4] = { 0.2f, 0.2f, 0.2f, 0.7f };
	float colorPlane[4] = { 0.0f, 0.4f, 0.8f, 0.5f };

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw the edges around frustum
	glBegin(GL_LINES);
	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[4]);

	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[5]);

	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[6]);

	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[7]);
	glEnd();

	glColor4fv(colorLine1);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glEnd();

	glColor4fv(colorLine1);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);
	glEnd();

	// draw near and far plane
	glColor4fv(colorPlane);
	glBegin(GL_QUADS);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glEnd();

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

void LeftView()
{
	glViewport(0, height / 2, width / 2, height / 2);
	glScissor(0, height / 2, width / 2, height / 2);
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 2.75, 3000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CamPosx, CamPosy, CamPosz, CamLookatX, CamLookatY, CamLookatZ, 0, 1, 0);
	
	glPushMatrix();
	glTranslatef(-300, -600, 500);
	DrawSkybox();
	glPopMatrix();

	DrawGrid();

	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(3.0);
	glPushMatrix();
	glTranslatef(tX, tY, tZ);
	glDisable(GL_LIGHTING);
	drawGrazePoint(0.3);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	
	drawScenario();
}


void RightView() {
	glViewport(width / 2, height / 2, width / 2, height / 2);
	glScissor(width / 2, height / 2, width / 2, height / 2);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.7, 3000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0, 4, 10, 0, 2.5, 0, 0, 1, 0); // Fixed Camera Position
	
	glPushMatrix();
	glTranslatef(-300, -600, 500);
	DrawSkybox();
	glPopMatrix();

	glTranslatef(0.5, 0.0, 0.0);

	glDisable(GL_LIGHTING);
	DrawGrid();
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(0, 0, 8);
	glRotatef(frustrumAngleX, 0, 1, 0);
	glRotatef(frustrumAngleY, 1, 0, 0);
	//drawCamera();
	drawFrustum(60, 1, 1, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(tX, tY, tZ);
	glTranslatef(0, 0.2, 0);
	glDisable(GL_LIGHTING);
	drawGrazePoint(0.5);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	drawScenario();
}

void LeftBottomView()
{
	glViewport(0, 0, width / 2, height / 2);
	glScissor(0, 0, width / 2, height / 2);
	glClearColor(0.45, 0.45, 0.45, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 2.75, 3000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CamPosx2, CamPosy2, CamPosz2, CamLookatX2, CamLookatY2, CamLookatZ2, 0, 1, 0);

	glPushMatrix();
	glTranslatef(-300, -600, 500);
	DrawSkybox();
	glPopMatrix();

	DrawGrid();

	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(3.0);
	glPushMatrix();
	glTranslatef(tWithoutVorX, tWithoutVorY, tWithoutVorZ);
	glDisable(GL_LIGHTING);
	drawGrazePoint(0.3);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	drawScenario();
}


void RightBottomView() {
	glViewport(width / 2, 0, width / 2, height / 2);
	glScissor(width / 2, 0, width / 2, height / 2);
	glClearColor(0.9, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.7, 3000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0, 4, 10, 0, 2.5, 0, 0, 1, 0); // Fixed Camera Position

	glPushMatrix();
	glTranslatef(-300, -600, 500);
	DrawSkybox();
	glPopMatrix();

	glTranslatef(0.5, 0.0, 0.0);

	DrawGrid();

	glPushMatrix();
	glTranslatef(1, 1, 0); // Move the teapot tothe origin					   //glutSolidTeapot(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 8);
	glRotatef(frustrumAngleX2, 0, 1, 0);
	glRotatef(frustrumAngleY2, 1, 0, 0);
	drawFrustum(60, 1, 1, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(tWithoutVorX, tWithoutVorY, tWithoutVorZ);
	glTranslatef(0, 0.2, 0);
	drawGrazePoint(0.5);
	glPopMatrix();

	drawScenario();
	
}

void keyboardCmdInput(unsigned char cmd, int x, int y)
{

	if (cmd == ' ')
	{
		cout << "S or space bar is pressed\n";
		//ReadFile();
		setSimulationFlag = true;
		computeScenarioPosition();
		computeMovement();
	}

	if (cmd == 'a' || cmd == 'A') //LEFT
	{
		tX = tX - 0.25;
	}

	if (cmd == 'd' || cmd == 'D') //RIGHT
	{
		tX = tX + 0.25;
	}

	if (cmd == 'w' || cmd == 'W') //UP
	{
		tY = tY + 0.25;
	}

	if (cmd == 's' || cmd == 'S') //down
	{
		tY = tY - 0.25;
	}
	glutPostRedisplay();
}

void KeyboardInput(int key, int x, int y)
{

	int buttonType = 0;
	float angle = 0;
	switch (key)
	{
	case GLUT_KEY_LEFT:	// Heading to left, rotate y-axis +degree
						//HorizontalCount = HorizontalCount - 1;
		if (frustrumAngleX > 20)
		{
			//HorizontalCount = -20;
		}
		else
		{
			buttonType = 1;
			angle = 1.5;

			executeMovement(buttonType, angle, 1);
		}
		break;

	case GLUT_KEY_RIGHT: // Heading to right, rotate y-axis -degree
		HorizontalCount = HorizontalCount + 1;
		if (frustrumAngleX < -20)
		{
			//HorizontalCount = 20;
		}
		else
		{
			buttonType = 2;
			angle = -1.5;

			//frustrumAngleX = frustrumAngleX - 1;
			executeMovement(buttonType, angle, 1);
		}

		break;

	case GLUT_KEY_UP:	// Heading to up, rotate x-axis +degree
						//verticalCount = verticalCount + 1;
		if (frustrumAngleY > 20)
		{
			//verticalCount = 20;
		}
		else
		{
			buttonType = 3;
			angle = 1.5;

			//frustrumAngleY = frustrumAngleY + 1;
			executeMovement(buttonType, angle, 1);
		}

		break;

	case GLUT_KEY_DOWN:	// Heading to down, rotate x-axis -degree
		verticalCount = verticalCount - 1;
		if (frustrumAngleY < -20)
		{
			//verticalCount = -20;
		}
		else
		{
			buttonType = 4;
			angle = -1.5;

			//frustrumAngleY = frustrumAngleY - 1;
			executeMovement(buttonType, angle, 1);
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void addLighting()
{
	GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };	
	GLfloat light1_diffuse[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light1_specular[] = { 0.2, 0.4, 0.5, 1.0 };
	GLfloat light1_pos[] = { 3.0, 100.0, -100.0, 0.0 };

	/*GLfloat light2_ambient[] = { 0.9, 0.9, 0.9, 1.0 };	
	GLfloat light2_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light2_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light2_pos[] = { 0.0, 20.0, -15.0, 1.0 };*/

	glShadeModel(GL_SMOOTH);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light1_pos);

	//Add Ambient Light
	float ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // color (0.2,0.2,0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void Display()
{
	addLighting();

	LeftView();
	RightView();
	LeftBottomView();
	RightBottomView();

	glFlush();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	// GLUT Window Initialization:
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Eye Simulator");

	ReadFile();
	computeScenarioPosition();
	// Initialize OpenGL graphics state
	InitGraphics();
	LoadGLTextures();
	setSimulationFlag = false;
	totalFlag = 1;

	// Register callbacks:
	glutDisplayFunc(Display);
	glutSpecialFunc(KeyboardInput);
	glutKeyboardFunc(keyboardCmdInput);
	glutReshapeFunc(reShape);
	glutIdleFunc(Display);
	glutTimerFunc(1000, makeMovementWithVOR, 1);
	glutMainLoop();
	return 0;
}