#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>   
#include <Windows.h>

struct Person
{
    char name[30];
    int age;
    char phone[15];
};
struct Point
{
    int x;
    int y;
};
typedef struct Point Point;

#define KEY_ESC 27
#define KEY_UP (256 + 72)
#define KEY_DOWN (256 + 80)
#define KEY_LEFT (256 + 75)
#define KEY_RIGHT (256 + 77)
#define EAST 1
#define WEST 2
#define SOUTH 3
#define NORTH 4
#define QUIT 0
#define PASS 2

void GotoXY(int x, int y);
void SetCursorInvisible();
int GetKey(void);
double GetElapsedTime(clock_t start_time);
void Initialize(void);      // 초기화, 우주선 위치, 지구 위치, 별 개수, 별 위치
void DrawSpace(void);       // 화면 그리기
int SetDirection(int getkey);
void SetSpaceShipPosition(int direction, Point *ship);
int checkAccident(Point user, Point obstacle);
int checkOutBox(Point user);

void Input(struct Person* person);
void Output(struct Person person);

int stage = 1;

Point spaceship;            // 우주선 위치
int direction;              // 현재 방향 (EAST, WEST, SOUTH, NORTH)
double speed;               // 현재 속도 0.1(초마다 이동), stage 증가(-0.01)

Point earth;                // 지구의 위치
int star_count = 10;        // 별의 개수
Point* stars = NULL;        // 별들의 위치

int nextStage = PASS;

int main(void)
{
    int selectProblem;
    struct Person me;
    srand(time(NULL));
    printf("[1] 구조체 Person 사용하기 [2] 우주선 조정 게임 만들기 : "); scanf("%d", &selectProblem);
    system("mode CON COLS=80 LINES=25");
    switch (selectProblem) {
    case 1:
        Input(&me);
        Output(me);
        break;
    case 2:
        SetCursorInvisible();

        while (1)
        {
            Initialize();
            DrawSpace();
            clock_t start_time = clock();
            while (1)
            {
                if (_kbhit())  // 방향키 입력 처리
                {
                    direction = SetDirection(GetKey());
                }

                if (GetElapsedTime(start_time) >= speed)  // 이동 처리
                {
                    // 현재 방향으로 한 칸 이동

                    SetSpaceShipPosition(direction, &spaceship);
                    // 지구 도착 검사 => 성공 => while 문 탈출
                    if (checkAccident(spaceship, earth)) {
                        nextStage = PASS;
                        break;
                    }
                    // 충돌 검사 => 충돌 => while 문 탈출
                    for (int i = 0; i < star_count; i++) {
                        if (checkAccident(spaceship, stars[i])) {
                            nextStage = QUIT;
                            break;
                        }
                    }
                    if (checkOutBox(spaceship)) {
                        nextStage = QUIT;
                    }

                    if (nextStage == QUIT) {
                        break;
                    }
                    start_time = clock();
                }
            }
            if (nextStage == QUIT) {
                break;
            }
            stage++;
            // 성공으로 끝났다면 stage 1 증가 후 바깥쪽 while 문 계속 실행
            // 실패로 끝났다면 프로그램 종료
        }
    }       
    return 0;
}

void ClearInputBuffer(void) {
    while (getchar() != '\n'){}
}

void Input(struct Person* person) {
    ClearInputBuffer();
    printf("<<< 인적 정보 입력받기 >>>\n");
    printf(">>> 이름 입력 : "); gets(person->name);
    printf(">>> 나이 입력 : "); scanf("%d", &person->age); ClearInputBuffer();
    printf(">>> 전화번호 입력 : "); fgets(person->phone, sizeof(person->phone), stdin);
}

void Output(struct Person person) {
    printf("\n<<< 인적 정보 출력하기 >>>\n");
    printf(">>> 이름 출력 : %s\n", person.name);
    printf(">>> 나이 출력 : %d\n", person.age);
    printf(">>> 전화번호 : %s\n", person.phone);
}

void Initialize(void)
{
    spaceship.x = 0;
    spaceship.y = 0;
    direction = EAST;
    if (stage == 1)
        speed = 0.1;
    else
        speed = speed - 0.01;
    if (speed < 0.03)
        speed = 0.03;

    earth.x = rand() % (79 - 60 + 1) + 60;
    earth.y = rand() % (24 - 20 + 1) + 20;

    if (stage == 1)
        star_count = 10;
    else
    {
        star_count += 5;
        free(stars);
    }

    stars = malloc(sizeof(Point) * star_count);
    for (int i = 0; i < star_count; i++)
    {
        stars[i].x = rand() % 80;
        stars[i].y = rand() % 25;
    }
}

void DrawSpace(void)
{
    system("cls");
    GotoXY(spaceship.x, spaceship.y);
    printf("@");

    GotoXY(earth.x, earth.y);
    printf("$");

    for (int i = 0; i < star_count; i++)
    {
        GotoXY(stars[i].x, stars[i].y);
        printf("*");
    }
}

void GotoXY(int x, int y)
{   // COORD 구조체 변수를 통해 이동할 위치 설정
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void SetCursorInvisible()
{
    CONSOLE_CURSOR_INFO ci = { 100, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

int GetKey(void)
{
    int ch = _getch();

    if (ch == 0 || ch == 224)
        // 방향키의 경우 0 또는 224의 값이 먼저 입력됨
        ch = 256 + _getch();
    // 그 다음에 해당 방향키에 따라 72(Up), 
    // 80(Down), 75(Left), 77(Right) 값이 입력됨
    return ch;
}

double GetElapsedTime(clock_t start_time)
{
    clock_t current_time = clock();
    return ((double)(current_time - start_time) / CLOCKS_PER_SEC);
}

int SetDirection(int getkey) {
    switch (getkey) {
    case KEY_DOWN:
        return SOUTH;
        break;
    case KEY_UP:
        return NORTH;
        break;
    case KEY_LEFT:
        return WEST;
        break;
    case KEY_RIGHT:
        return EAST;
        break;
    }
}

void SetSpaceShipPosition(int direction, Point *ship) {
    switch (direction) {
    case EAST:
        GotoXY(ship->x, ship->y);
        printf(" ");
        ship->x += 1;
        GotoXY(ship->x, ship->y);
        printf("@");
        break;
    case WEST:
        GotoXY(ship->x, ship->y);
        printf(" ");
        ship->x -= 1;
        GotoXY(ship->x, ship->y);
        printf("@");
        break;
    case NORTH:
        GotoXY(ship->x, ship->y);
        printf(" ");
        ship->y -= 1;
        GotoXY(ship->x, ship->y);
        printf("@");
        break;
    case SOUTH:
        GotoXY(ship->x, ship->y);
        printf(" ");
        ship->y += 1;
        GotoXY(ship->x, ship->y);
        printf("@");
        break;
    }
}

int checkAccident(Point user, Point obstacle) {
    if (user.x == obstacle.x && user.y == obstacle.y) {
        return 1;
    }
    return 0;
}

int checkOutBox(Point user) {
    if (user.x > 80 || user.x < -1 || user.y > 25 || user.y < -1) {
        return 1;
    }
    return 0;
}
