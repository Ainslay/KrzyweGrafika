/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// sta�e do obs�ugi menu podr�cznego

enum
{
    EXIT // wyj�cie
};

// wska�nik naci�ni�cia lewego przycisku myszki

int button_state = GLUT_UP;

// po�o�enie kursora myszki

int button_x, button_y;

// punkty kontrolne krzywej

GLfloat points[5 * 3] =
{
    50.0, 50.0, 0.0, 250.0, 250.0, 0.0, 50.0, 450.0, 0.0, 450.0, 50.0, 0.0, 450.0, 450.0, 0.0
};

// w�z�y

GLfloat knots[5 * 2] =
{
    0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0
};

// warto�� w�a�ciwo�ci GLU_SAMPLING_TOLERANCE

GLfloat sampling_tolerance = 20.0;

// identyfikatory wy�wietlanych obiekt�w

enum
{
    NONE,
    CURVE,
    POINT_0,
    POINT_1,
    POINT_2,
    POINT_3,
    POINT_4
};

// identyfikator wybranego obiektu

int select_object = NONE;

// funkcja rysuj�ca napis w wybranym miejscu

void DrawString(GLfloat x, GLfloat y, char* string)
{
    // po�o�enie napisu
    glRasterPos2f(x, y);

    // wy�wietlenie napisu
    int len = strlen(string);
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);

}

// funkcja generuj�ca scen� 3D

void Display()
{
    // kolor t�a - zawarto�� bufora koloru
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // czyszczenie bufora koloru
    glClear(GL_COLOR_BUFFER_BIT);

    // inicjalizacja stosu nazw obiekt�w
    glInitNames();

    // umieszczenie nazwy na stosie nazw, aby nie by� on pusty
    glPushName(NONE);

    // obiekt CURVE
    glLoadName(CURVE);

    // ustawienie grubo�ci linii
    glLineWidth(2.0);

    // kolor krzywej
    glColor3f(1.0, 0.0, 0.0);

    // utworzenie obiektu NURBS
    GLUnurbsObj* nurbs = gluNewNurbsRenderer();

    // pocz�tek definicji krzywej NURBS
    gluBeginCurve(nurbs);

    // ustawienie w�a�ciwo�ci GLU_SAMPLING_TOLERANCE
    gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, sampling_tolerance);

    // ewaluacja krzywej
    gluNurbsCurve(nurbs, 10, knots, 3, points, 5, GL_MAP1_VERTEX_3);

    // koniec definicji krzywej
    gluEndCurve(nurbs);

    // usuni�cie obiektu NURBS
    gluDeleteNurbsRenderer(nurbs);

    // kolor punkt�w
    glColor3f(0.0, 1.0, 1.0);

    // narysowanie punkt�w kontrolnych
    for (int i = 0; i < 5; i++)
    {
        // nazwa obiektu
        glLoadName(POINT_0 + i);

        // punkt kontrolny
        glRectf(points[i * 3] - 5, points[i * 3 + 1] - 5, points[i * 3] + 5, points[i * 3 + 1] + 5);
    }

    // wy�wietlenie warto�ci w�a�ciwo�ci GLU_SAMPLING_TOLERANCE
    char string[100];
    glColor3f(1.0, 0.0, 0.0);
    sprintf(string, "GLU_SAMPLING_TOLERANCE = %f", sampling_tolerance);

    // narysowanie napisu
    DrawString(2, 2, string);

    // skierowanie polece� do wykonania
    glFlush();

    // zamiana bufor�w koloru
    glutSwapBuffers();
}

// zmiana wielko�ci okna

void Reshape(int width, int height)
{
    // obszar renderingu - ca�e okno
    glViewport(0, 0, width, height);

    // wyb�r macierzy rzutowania
    glMatrixMode(GL_PROJECTION);

    // macierz rzutowania = macierz jednostkowa
    glLoadIdentity();

    // rzutowanie prostok�tne
    gluOrtho2D(0, width, 0, height);

    // generowanie sceny 3D
    Display();
}

// obs�uga selekcji obietk�w

void Selection(int x, int y)
{
    // wielko�� bufora selekcji
    const int BUFFER_LENGTH = 64;

    // bufor selekcji
    GLuint select_buffer[BUFFER_LENGTH];

    // przygotowanie bufora selekcji
    glSelectBuffer(BUFFER_LENGTH, select_buffer);

    // pobranie obszaru rozmiaru renderingu
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // szeroko�� i wysoko�� obszaru renderingu
    int width = viewport[2];
    int height = viewport[3];

    // wyb�r macierzy rzutowania
    glMatrixMode(GL_PROJECTION);

    // od�o�enie macierzy rzutowania na stos
    glPushMatrix();

    // macierz rzutowania = macierz jednostkowa
    glLoadIdentity();

    // parametry bry�y obcinania - jednostkowa kostka dooko�a punktu wska�nika
    // myszy (x,y) rozci�gaj�cej si� na dwa piksele w poziomie i w pionie
    gluPickMatrix(x, height - y, 2, 2, viewport);

    // rzutowanie prostok�tne
    gluOrtho2D(0, width, 0, height);

    // w��czenie trybu selekcji
    glRenderMode(GL_SELECT);

    // generowanie sceny 3D
    Display();

    // zliczenie ilo�ci rekord�w trafie�, powr�t do domy�lnego trybu renderingu
    GLint hits = glRenderMode(GL_RENDER);

    // wyb�r macierzy rzutowania
    glMatrixMode(GL_PROJECTION);

    // zdj�cie macierzy rzutowania ze stosu
    glPopMatrix();

    // domy�lnie w wyniku selekcji nie wybrano �adnego punktu kontrolnego
    select_object = NONE;

    // w wyniku selekcji wybrano co najmniej jeden obiekt
    // dla uproszczenia sprawdzamy tylko pierwszy obiekt na stosie
    if (hits > 0)
        if (select_buffer[3] > CURVE)
            select_object = select_buffer[3];

}

// obs�uga przycisk�w myszki

void MouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        // obs�uga selekcji obiekt�w
        Selection(x, y);
        glutPostRedisplay();

        // zapami�tanie stanu lewego przycisku myszki
        button_state = state;

        // zapami�tanie po�o�enia kursora myszki
        if (state == GLUT_DOWN)
        {
            button_x = x;
            button_y = y;
        }
    }
}

// obs�uga ruchu kursora myszki

void MouseMotion(int x, int y)
{
    // sprawdzenie czy wska�nik myszy nie znajduje si� poza obszarem okna
    if (x > 0 & x < glutGet(GLUT_WINDOW_WIDTH) & y > 0 & y < glutGet(GLUT_WINDOW_HEIGHT))
        if (button_state == GLUT_DOWN)
        {
            // zmiana wsp�rz�dnych punkt�w
            switch (select_object)
            {
            case POINT_0:
                points[0] += x - button_x;
                points[1] += button_y - y;
                break;
            case POINT_1:
                points[3] += x - button_x;
                points[4] += button_y - y;
                break;
            case POINT_2:
                points[6] += x - button_x;
                points[7] += button_y - y;
                break;
            case POINT_3:
                points[9] += x - button_x;
                points[10] += button_y - y;
                break;
            case POINT_4:
                points[12] += x - button_x;
                points[13] += button_y - y;
                break;
            }

            // aktualizacja po�o�enia myszki
            button_x = x;
            button_y = y;

            // wy�wietlenie sceny
            glutPostRedisplay();
        }
}

// obs�uga klawiatury

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        // klawisz "+" -
    case '+':
        sampling_tolerance += 10.0;
        break;

        // klawisz "-" -
    case '-':
        if (sampling_tolerance > 10.0)
            sampling_tolerance -= 10.0;

        break;
    }

    // narysowanie sceny
    Display();
}

// obs�uga menu podr�cznego

void Menu(int value)
{
    switch (value)
    {
        // wyj�cie
    case EXIT:
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    // inicjalizacja biblioteki GLUT
    glutInit(&argc, argv);

    // inicjalizacja bufora ramki
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // rozmiary g��wnego okna programu
    glutInitWindowSize(500, 500);

    // utworzenie g��wnego okna programu
    glutCreateWindow("Krzywa NURBS");

    // do��czenie funkcji generuj�cej scen� 3D
    glutDisplayFunc(Display);

    // do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
    glutReshapeFunc(Reshape);

    // do��czenie funkcji obs�ugi klawiatury
    glutKeyboardFunc(Keyboard);

    // obs�uga przycisk�w myszki
    glutMouseFunc(MouseButton);

    // obs�uga ruchu kursora myszki
    glutMotionFunc(MouseMotion);

    // utworzenie menu podr�cznego
    glutCreateMenu(Menu);

    // menu g��wne
    glutCreateMenu(Menu);
#ifdef WIN32

    glutAddMenuEntry("Wyj�cie", EXIT);
#else

    glutAddMenuEntry("Wyjscie", EXIT);
#endif

    // okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // wprowadzenie programu do obs�ugi p�tli komunikat�w
    glutMainLoop();
    return 0;
}