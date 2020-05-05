/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// sta³e do obs³ugi menu podrêcznego

enum
{
    EXIT // wyjœcie
};

// wskaŸnik naciœniêcia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

// punkty kontrolne krzywej

GLfloat points[12] =
{
    50.0, 50.0, 0.0, 50.0, 450.0, 0.0, 450.0, 50.0, 0.0, 450.0, 450.0, 0.0
};

// identyfikatory wyœwietlanych obiektów

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

// funkcja generuj¹ca scenê 3D

void Display()
{
    // kolor t³a - zawartoœæ bufora koloru
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // czyszczenie bufora koloru
    glClear(GL_COLOR_BUFFER_BIT);

    // inicjalizacja stosu nazw obiektów
    glInitNames();

    // umieszczenie nazwy na stosie nazw, aby nie by³ on pusty
    glPushName(NONE);

    // obiekt CURVE
    glLoadName(CURVE);

    // w³¹czenie generowana wspó³rzêdnych (x,y,z) punktów krzywej
    glEnable(GL_MAP1_VERTEX_3);

    // ewaluator jednowymiarowy dla czterech punktów kontronych
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, points);

    // ustawienie gruboœci linii
    glLineWidth(2.0);

    // kolor krzywej
    glColor3f(1.0, 1.0, 0.0);

    // narysowanie krzywej z³o¿onej z 25 odcinków
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 25; i++)
        glEvalCoord1f((GLfloat)i / 25.0);

    glEnd();

    // kolor punktów
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

    // skierowanie poleceñ do wykonania
    glFlush();

    // zamiana buforów koloru
    glutSwapBuffers();
}

// zmiana wielkoœci okna

void Reshape(int width, int height)
{
    // obszar renderingu - ca³e okno
    glViewport(0, 0, width, height);

    // wybór macierzy rzutowania
    glMatrixMode(GL_PROJECTION);

    // macierz rzutowania = macierz jednostkowa
    glLoadIdentity();

    // rzutowanie prostok¹tne
    gluOrtho2D(0, width, 0, height);

    // generowanie sceny 3D
    Display();
}

// obs³uga selekcji obietków

void Selection(int x, int y)
{
    // wielkoœæ bufora selekcji
    const int BUFFER_LENGTH = 64;

    // bufor selekcji
    GLuint select_buffer[BUFFER_LENGTH];

    // przygotowanie bufora selekcji
    glSelectBuffer(BUFFER_LENGTH, select_buffer);

    // pobranie obszaru rozmiaru renderingu
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // szerokoœæ i wysokoœæ obszaru renderingu
    int width = viewport[2];
    int height = viewport[3];

    // wybór macierzy rzutowania
    glMatrixMode(GL_PROJECTION);

    // od³o¿enie macierzy rzutowania na stos
    glPushMatrix();

    // macierz rzutowania = macierz jednostkowa
    glLoadIdentity();

    // parametry bry³y obcinania - jednostkowa kostka dooko³a punktu wskaŸnika
    // myszy (x,y) rozci¹gaj¹cej siê na dwa piksele w poziomie i w pionie
    gluPickMatrix(x, height - y, 2, 2, viewport);

    // rzutowanie prostok¹tne
    gluOrtho2D(0, width, 0, height);

    // w³¹czenie trybu selekcji
    glRenderMode(GL_SELECT);

    // generowanie sceny 3D
    Display();

    // zliczenie iloœci rekordów trafieñ, powrót do domyœlnego trybu renderingu
    GLint hits = glRenderMode(GL_RENDER);

    // wybór macierzy rzutowania
    glMatrixMode(GL_PROJECTION);

    // zdjêcie macierzy rzutowania ze stosu
    glPopMatrix();

    // domyœlnie w wyniku selekcji nie wybrano ¿adnego punktu kontrolnego
    select_object = NONE;

    // w wyniku selekcji wybrano co najmniej jeden obiekt
    // dla uproszczenia sprawdzamy tylko pierwszy obiekt na stosie
    if (hits > 0)
        if (select_buffer[3] > CURVE)
            select_object = select_buffer[3];

}

// obs³uga przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        // obs³uga selekcji obiektów
        Selection(x, y);
        glutPostRedisplay();

        // zapamiêtanie stanu lewego przycisku myszki
        button_state = state;

        // zapamiêtanie po³o¿enia kursora myszki
        if (state == GLUT_DOWN)
        {
            button_x = x;
            button_y = y;
        }
    }
}

// obs³uga ruchu kursora myszki

void MouseMotion(int x, int y)
{
    // sprawdzenie czy wskaŸnik myszy nie znajduje siê poza obszarem okna
    if (x > 0 && x < glutGet(GLUT_WINDOW_WIDTH) && y > 0 && y < glutGet(GLUT_WINDOW_HEIGHT))
        if (button_state == GLUT_DOWN)
        {
            // zmiana wspó³rzêdnych punktów
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

            // aktualizacja po³o¿enia myszki
            button_x = x;
            button_y = y;

            // wyœwietlenie sceny
            glutPostRedisplay();
        }
}

// obs³uga menu podrêcznego

void Menu(int value)
{
    switch (value)
    {
        // wyjœcie
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

    // rozmiary g³ównego okna programu
    glutInitWindowSize(500, 500);

    // utworzenie g³ównego okna programu
#ifdef WIN32

    glutCreateWindow("Krzywa Béziera");
#else

    glutCreateWindow("Krzywa Beziera");
#endif

    // do³¹czenie funkcji generuj¹cej scenê 3D
    glutDisplayFunc(Display);

    // do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
    glutReshapeFunc(Reshape);

    // obs³uga przycisków myszki
    glutMouseFunc(MouseButton);

    // obs³uga ruchu kursora myszki
    glutMotionFunc(MouseMotion);

    // utworzenie menu podrêcznego
    glutCreateMenu(Menu);

    // menu g³ówne
    glutCreateMenu(Menu);
#ifdef WIN32

    glutAddMenuEntry("Wyjœcie", EXIT);
#else

    glutAddMenuEntry("Wyjscie", EXIT);
#endif

    // okreœlenie przycisku myszki obs³uguj¹cej menu podrêczne
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // wprowadzenie programu do obs³ugi pêtli komunikatów
    glutMainLoop();
    return 0;
}
