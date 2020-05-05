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

GLfloat points[12] =
{
    50.0, 50.0, 0.0, 50.0, 450.0, 0.0, 450.0, 50.0, 0.0, 450.0, 450.0, 0.0
};

// identyfikatory wy�wietlanych obiekt�w

enum
{
    NONE,
    CURVE,
    POINT_0,
    POINT_1,
    POINT_2,
    POINT_3
};

// identyfikator wybranego obiektu

int select_object = NONE;

// funkcja generuj�ca scen� 3D

void Display()
{
    // kolor t�a - zawarto�� bufora koloru
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // czyszczenie bufora koloru
    glClear(GL_COLOR_BUFFER_BIT);

    // inicjalizacja stosu nazw obiekt�w
    glInitNames();

    // umieszczenie nazwy na stosie nazw, aby nie by� on pusty
    glPushName(NONE);

    // obiekt CURVE
    glLoadName(CURVE);

    // w��czenie generowana wsp�rz�dnych (x,y,z) punkt�w krzywej
    glEnable(GL_MAP1_VERTEX_3);

    // ewaluator jednowymiarowy dla czterech punkt�w kontronych
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, points);

    // ustawienie grubo�ci linii
    glLineWidth(2.0);

    // kolor krzywej
    glColor3f(1.0, 1.0, 0.0);

    // narysowanie krzywej z�o�onej z 25 odcink�w
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 25; i++)
        glEvalCoord1f((GLfloat)i / 25.0);

    glEnd();

    // kolor punkt�w
    glColor3f(0.0, 1.0, 1.0);

    // obiekt - pierwszy punkt kontrolny
    glLoadName(POINT_0);

    // narysowanie pierwszego punktu kontrolnego
    glRectf(points[0] - 5, points[1] - 5, points[0] + 5, points[1] + 5);

    // obiekt - drugi punkt kontrolny
    glLoadName(POINT_1);

    // narysowanie drugiego punktu kontrolnego
    glRectf(points[3] - 5, points[4] - 5, points[3] + 5, points[4] + 5);

    // obiekt - trzeci punkt kontrolny
    glLoadName(POINT_2);

    // narysowanie trzeciego punktu kontrolnego
    glRectf(points[6] - 5, points[7] - 5, points[6] + 5, points[7] + 5);

    // obiekt - czwarty punkt kontrolny
    glLoadName(POINT_3);

    // narysowanie czwartego punktu kontrolnego
    glRectf(points[9] - 5, points[10] - 5, points[9] + 5, points[10] + 5);

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
    if (x > 0 && x < glutGet(GLUT_WINDOW_WIDTH) && y > 0 && y < glutGet(GLUT_WINDOW_HEIGHT))
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
            }

            // aktualizacja po�o�enia myszki
            button_x = x;
            button_y = y;

            // wy�wietlenie sceny
            glutPostRedisplay();
        }
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

int mainn(int argc, char* argv[])
{
    // inicjalizacja biblioteki GLUT
    glutInit(&argc, argv);

    // inicjalizacja bufora ramki
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // rozmiary g��wnego okna programu
    glutInitWindowSize(500, 500);

    // utworzenie g��wnego okna programu
#ifdef WIN32

    glutCreateWindow("Krzywa B�ziera");
#else

    glutCreateWindow("Krzywa Beziera");
#endif

    // do��czenie funkcji generuj�cej scen� 3D
    glutDisplayFunc(Display);

    // do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
    glutReshapeFunc(Reshape);

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
