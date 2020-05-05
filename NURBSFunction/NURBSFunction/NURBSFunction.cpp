// Opracowane przez: Jakub Spa³ek, Aleksanda Pyrkosz

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Windows.h>
#include <iostream>
#include <math.h>

using namespace std;

int leftMouseButtonState = GLUT_UP;
int mouseX, mouseY;

GLfloat controlPoints[15] = {
	50.0, 50.0, 0.0,
	750.0, 50.0, 0.0,
	750.0, 750.0, 0.0,
	50.0, 750.0, 0.0,
	400.0, 400.0, 0.0
};

GLfloat nodes[15] = {
	0.0, 0.0,
	0.0, 0.0,
	0.0, 1.0,
	1.0, 1.0,
	1.0, 1.0
};

GLfloat samplingTolerance = 20.0;

enum Objects
{
	NONE,
	CURVE,
	POINT_0,
	POINT_1,
	POINT_2,
	POINT_3,
	POINT_4
};

int selectedObject = Objects::NONE;

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glInitNames();

	glPushName(Objects::NONE);
	glLoadName(Objects::CURVE);

	glLineWidth(2.0);
	glColor3f(1.0, 0.0, 0.0);

	GLUnurbsObj* nurbs = gluNewNurbsRenderer();

	gluBeginCurve(nurbs);

		gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, samplingTolerance);

		gluNurbsCurve(nurbs, 10, nodes, 3, controlPoints, 5, GL_MAP1_VERTEX_3);

	gluEndCurve(nurbs);

	gluDeleteNurbsRenderer(nurbs);

	glColor3f(0.0, 1.0, 1.0);

	glLoadName(Objects::POINT_0);
	glRectf(controlPoints[0] - 5, controlPoints[1] - 5, controlPoints[0] + 5, controlPoints[1] + 5);

	glLoadName(Objects::POINT_1);
	glRectf(controlPoints[3] - 5, controlPoints[4] - 5, controlPoints[3] + 5, controlPoints[4] + 5);
	
	glLoadName(Objects::POINT_2);
	glRectf(controlPoints[6] - 5, controlPoints[7] - 5, controlPoints[6] + 5, controlPoints[7] + 5);

	glLoadName(Objects::POINT_3);
	glRectf(controlPoints[9] - 5, controlPoints[10] - 5, controlPoints[9] + 5, controlPoints[10] + 5);
	
	glLoadName(Objects::POINT_4);
	glRectf(controlPoints[12] - 5, controlPoints[13] - 5, controlPoints[12] + 5, controlPoints[13] + 5);
	
	glFlush();

	glutSwapBuffers();
}

void ChangeViewPort(int width, int height)
{
	const float aspectRatio = (float)width / (float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0 / aspectRatio, height / aspectRatio, -10, 10);

	Render();
}

void Selection(int xPosition, int yPosition)
{
	const int selectionBufferLength = 64;

	GLuint selectionBuffer[selectionBufferLength];

	glSelectBuffer(selectionBufferLength, selectionBuffer);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int width = viewport[2];
	int height = viewport[3];
	const float aspectRatio = (float)width / (float)height;

	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	glLoadIdentity();

	gluPickMatrix(xPosition, height - yPosition, 2, 2, viewport);

	glOrtho(0, width, 0 / aspectRatio, height / aspectRatio, -10, 10);

	glRenderMode(GL_SELECT);

	Render();

	GLint hits = glRenderMode(GL_RENDER);

	glMatrixMode(GL_PROJECTION);

	glPopMatrix();

	selectedObject = Objects::NONE;

	if (hits > 0)
	{
		if (selectionBuffer[3] > Objects::CURVE)
		{
			selectedObject = selectionBuffer[3];
		}
	}
}

void MouseButton(int button, int state, int xPosition, int yPosition)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		Selection(xPosition, yPosition);
		glutPostRedisplay();

		leftMouseButtonState = state;

		if (state == GLUT_DOWN)
		{
			mouseX = xPosition;
			mouseY = yPosition;
		}
	}
}

void MouseMotion(int xPosition, int yPosition)
{
	if (xPosition > 0 && xPosition < glutGet(GLUT_WINDOW_WIDTH) && yPosition > 0 && yPosition < glutGet(GLUT_WINDOW_HEIGHT))
	{
		if (leftMouseButtonState == GLUT_DOWN)
		{
			switch (selectedObject)
			{
			case POINT_0:
				controlPoints[0] += xPosition - mouseX;
				controlPoints[1] += mouseY - yPosition;
				break;
			case POINT_1:
				controlPoints[3] += xPosition - mouseX;
				controlPoints[4] += mouseY - yPosition;
				break;
			case POINT_2:
				controlPoints[6] += xPosition - mouseX;
				controlPoints[7] += mouseY - yPosition;
				break;
			case POINT_3:
				controlPoints[9] += xPosition - mouseX;
				controlPoints[10] += mouseY - yPosition;
				break;
			case POINT_4:
				controlPoints[12] += xPosition - mouseX;
				controlPoints[13] += mouseY - yPosition;
				break;
			}

			mouseX = xPosition;
			mouseY = yPosition;

			glutPostRedisplay();
		}
	}
}

void Menu()
{
	system("cls");

	cout << "Zwieksz wartosc tolerancji probkowania: [+]\n";
	cout << "Zmniejsz wartosc tolerancji probkowania: [-]\n";
	cout << "\nAktualna tolerancja probkowania: " << samplingTolerance << endl;
}

void Key(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+':
		samplingTolerance += 10.0;
		break;
	case '-':
		if (samplingTolerance > 10.0)
		{
			samplingTolerance -= 10.0;
		}
		break;
	}

	Menu();
	Render();
}

void Idle()
{
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(500, 100);

	glutCreateWindow("NURBS function");

	glutReshapeFunc(ChangeViewPort);
	glutDisplayFunc(Render);
	glutIdleFunc(Idle);
	glutKeyboardFunc(Key);

	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);

	glClearColor(0, 0, 0, 0);

	Menu();

	glutMainLoop();

	return 0;
}