#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <iostream>
#include <ctime>
using namespace std;

int         width = 55;
int         height = 35;
const float maxSpeed = 1.8f;
const int   delay = 50;

HANDLE hOut;

struct pad {
    float x, y;
    int size;
    float ox, oy;
    float vx;

    pad(int _x, int _y, int _size) {
        x = float(_x);
        y = float(_y);
        size = _size;
        ox = float(_x);
        oy = float(_y);
    }

    void draw() {
        COORD p;

        p.X = int(ox) - size;
        p.Y = int(oy);

        SetConsoleCursorPosition(hOut, p);

        for (int i = -size; i < size; ++i)
            cout << ' ';

        p.X = int(x) - size;
        p.Y = int(y);

        SetConsoleCursorPosition(hOut, p);

        SetConsoleTextAttribute(hOut, 10);

        for (int i = -size; i < size; ++i)
            cout << '=';

        SetConsoleTextAttribute(hOut, 15);

        ox = x;
        oy = y;
    }
};

struct ball {
    float x, y, ox, oy;
    float vx, vy;

    ball(int _x, int _y) {
        x = float(_x);
        y = float(_y);
        ox = float(x);
        oy = float(y);
        vx = vy = 0;
    }

    void draw() {
        COORD p;

        p.X = int(ox);
        p.Y = int(oy);

        SetConsoleCursorPosition(hOut, p);

        cout << ' ';

        p.X = int(x);
        p.Y = int(y);

        SetConsoleCursorPosition(hOut, p);
        SetConsoleTextAttribute(hOut, 12);

        cout << '*';

        SetConsoleTextAttribute(hOut, 15);

        ox = x;
        oy = y;

    }
};

void reset(pad& you, pad& comp, ball& b, bool& started, int& you_score, int& comp_score) {
    b.x = float(width / 2 - 1);
    b.y = float(height / 2);

    you.x = comp.x = float(width / 2);
    comp.vx = 0.0f;

    b.vx = float(rand() % 3) - 1.5f;
    b.vy = 1;

    started = false;

    COORD p;

    p.X = 0;
    p.Y = height;

    SetConsoleCursorPosition(hOut, p);
    cout << "\n    Your score: " << you_score << "\tComp score: " << comp_score << '\n';
}

int main() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    srand(unsigned int(time(NULL)));

    SMALL_RECT DisplayArea = { 0, 0, width, height + 3 };
    SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);

    // Hide Console Cursor

    CONSOLE_CURSOR_INFO ConCurInf;

    ConCurInf.dwSize = 10;
    ConCurInf.bVisible = false;

    SetConsoleCursorInfo(hOut, &ConCurInf);

    // Draw border

    SetConsoleTextAttribute(hOut, 15);

    for (int i = 0; i < width - 1; ++i)
        cout << '-';

    for (int i = 0; i < height - 1; ++i) {
        cout << "\n|";

        for (int i = 0; i < width - 3; ++i)
            cout << ' ';

        cout << "|";
    }

    cout << '\n';

    for (int i = 0; i < width - 1; ++i)
        cout << '-';

    cout << "\n    Your score: 0\tComp score: 0\n";

    for (int i = 0; i < width - 1; ++i)
        cout << '-';

    // End drawing border

    bool started = false;

    pad you(width / 2, height - 2, 4);
    pad comp(width / 2, 2, 4);

    ball b(width / 2 - 1, height / 2);

    int you_score = 0, comp_score = 0;

    // Ball's start velocity, always starts towards you
    b.vx = float(rand() % 3) - 1.5f;
    b.vy = 1;

    comp.vx = 0;

    while (true) {

        if (started) {

            // Arrow keys
            if (GetAsyncKeyState(VK_LEFT)) {
                you.x -= 2;
            }
            else if (GetAsyncKeyState(VK_RIGHT)) {
                you.x += 2;
            }

            // Move comp.x towards the ball
            comp.vx -= float(comp.x - b.x) / 10.0f;
            comp.vx *= 0.83f;

            // Apply comp's velocity to current location
            comp.x += comp.vx;

            // Add player boundaries
            if (you.x < you.size + 1) {
                you.x = float(you.size + 1);
            }
            else if (you.x > width - you.size - 2) {
                you.x = float(width - you.size - 2);
            }

            // Add comp boundaries
            if (comp.x < comp.size + 1) {
                comp.x = float(comp.size + 1);
            }
            else if (comp.x > width - comp.size - 2) {
                comp.x = float(width - comp.size - 2);
            }

            // Make sure ball doesn't exceed it's maximum speed
            if (b.vx < -maxSpeed) {
                b.vx = -maxSpeed;
            }
            else if (b.vx > maxSpeed) {
                b.vx = maxSpeed;
            }

            // Apply velocity to balls current location
            b.x += b.vx;
            b.y += b.vy;

            // Check if ball is hitting your pad
            if (b.y >= you.y - 1 && b.y <= you.y + 1) {
                if (b.x > you.x - you.size && b.x < you.x + you.size) {
                    b.vy *= -1;
                    b.vx += float(b.x - you.x) / 3;
                    b.y = you.y - 1;
                }
            }

            // Check if ball is hitting the computer pad
            if (b.y <= comp.y + 1 && b.y >= comp.y - 1) {
                if (b.x > comp.x - comp.size && b.x < comp.x + comp.size) {
                    b.vy *= -1;
                    b.vx += float(b.x - comp.x) / 3;
                    b.y = comp.y + 1;
                }
            }

            // Check if ball has gone out of boundaries (a player won/lost)
            if (b.y >= height) {
                ++comp_score;
                reset(you, comp, b, started, you_score, comp_score);
            }
            else if (b.y <= 0) {
                ++you_score;
                reset(you, comp, b, started, you_score, comp_score);
            }

            if (b.x <= 1) {
                b.vx *= -1;
                b.x = 1.0f;
            }
            else if (b.x >= width - 3) {
                b.vx *= -1;
                b.x = float(width - 3);
            }
        }
        else {
            // Wait for user to press the space bar
            COORD p;

            p.X = int(width / 2 - 16);
            p.Y = int(height / 1.5f);

            SetConsoleCursorPosition(hOut, p);

            cout << "Press the space bar to start . .";

            if (GetAsyncKeyState(VK_SPACE)) {
                COORD p;

                p.X = int(width / 2 - 16);
                p.Y = int(height / 1.5f);

                SetConsoleCursorPosition(hOut, p);

                cout << "                                ";
                started = true;
            }
        }

        // Draw all objects
        you.draw();
        comp.draw();
        b.draw();

        // Delay
        Sleep(delay);
    }
}