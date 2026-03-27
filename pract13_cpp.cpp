#include <iostream>
#include <windows.h>

using namespace std;

string names[10]{ "Иван", "Роман", "Пётр", "Николай", "Олег", "Александр", "Тимофей", "Гога", "Игнат", "Антон" };

HANDLE pThread[10];
HANDLE bThread;

HANDLE hMutex;
HANDLE pEvent;

struct Bayum {
    long long health = 9000000000;
    int resist = 44;
    int damage = 73843;
    int specialDamage = 150000;
    int attackCooldown = 5;
    int specialCooldown = 10;
};

struct Player {
    long health = 500000;
    int damage = 12000;
    int specialDamage = 30000;
    int attckCooldown = 2;
    int specialCooldown = 5;
    int defense = 20;
    int dodgeChance = 15;
    char name[64];
    long totalDamage = 0;
};

struct GamePlayPlayer {
    Player* pl;
    int colvoPlayers;
};

Player players[10];
Bayum boss;

bool gameOver = false;
int notalive = 0;

void GamePlay(GamePlayPlayer* gpl)
{
    while (true)
    {
        WaitForSingleObject(pEvent, INFINITE);

        if (gameOver)
            break;

        WaitForSingleObject(hMutex, INFINITE);

        if (boss.health <= 0 || gpl->pl->health <= 0)
        {
            ReleaseMutex(hMutex);
            break;
        }

        int attackPlayer = gpl->pl->damage * (100 - boss.resist) / 100;
        boss.health -= attackPlayer;
        gpl->pl->totalDamage += attackPlayer;

        cout << gpl->pl->name << " ударил босса на " << attackPlayer
             << " - HP босса: " << boss.health << endl;

        ReleaseMutex(hMutex);

        Sleep(gpl->pl->attckCooldown * 1000);

        SetEvent(pEvent);
    }

    // считаем умерших корректно
    WaitForSingleObject(hMutex, INFINITE);
    if (gpl->pl->health <= 0)
    {
        notalive++;
    }
    ReleaseMutex(hMutex);
}

void BossPlay(int colvo)
{
    while (true)
    {
        Sleep(boss.attackCooldown * 1000);

        WaitForSingleObject(hMutex, INFINITE);

        if (boss.health <= 0)
        {
            gameOver = true;
            ReleaseMutex(hMutex);
            SetEvent(pEvent);
            break;
        }

        int index = rand() % colvo;

        if (players[index].health > 0)
        {
            if (rand() % 100 < players[index].dodgeChance)
            {
                cout << players[index].name << " уклонился от атаки" << endl;
            }
            else
            {
                int attackBoss = boss.damage * (100 - players[index].defense) / 100;
                players[index].health -= attackBoss;

                cout << "БОСС ударил " << players[index].name << " на "
                     << attackBoss << " - HP игрока: "
                     << players[index].health << endl;
            }
        }

        ReleaseMutex(hMutex);

        Sleep(boss.specialCooldown * 1000);

        WaitForSingleObject(hMutex, INFINITE);

        if (notalive >= colvo)
        {
            gameOver = true;
            ReleaseMutex(hMutex);
            SetEvent(pEvent);
            break;
        }

        if (boss.health <= 0)
        {
            gameOver = true;
            ReleaseMutex(hMutex);
            SetEvent(pEvent);
            break;
        }

        cout << "БОСС использует специальную атаку!" << endl;

        for (int i = 0; i < colvo; i++)
        {
            int damage;

            if (colvo > 1)
                damage = boss.specialDamage * (1 - 0.05 * (colvo - 1));
            else
                damage = boss.specialDamage;

            if (players[i].health > 0)
            {
                players[i].health -= damage;

                cout << players[i].name << " получил "
                     << damage << " - HP: "
                     << players[i].health << endl;
            }
        }

        ReleaseMutex(hMutex);
    }
}

void CreateThreadPlayer(int colvo)
{
    for (int i = 0; i < colvo; i++)
    {
        strcpy_s(players[i].name, names[i].c_str());

        GamePlayPlayer* player = new GamePlayPlayer;
        player->pl = &players[i];
        player->colvoPlayers = colvo;

        pThread[i] = CreateThread(NULL, 0,
            (LPTHREAD_START_ROUTINE)GamePlay,
            (LPVOID)player, 0, 0);

        if (pThread[i] == NULL)
        {
            cout << "Ошибка создания потока игрока" << endl;
            return;
        }
    }

    bThread = CreateThread(NULL, 0,
        (LPTHREAD_START_ROUTINE)BossPlay,
        (LPVOID)colvo, 0, 0);
}

int main()
{
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    hMutex = CreateMutex(NULL, FALSE, NULL);
    pEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

    int vibor = 0;

    cout << "Сколько игроков? (1-10): ";
    cin >> vibor;

    if (vibor < 1 || vibor > 10)
    {
        cout << "Ошибка! Выберите число от 1 до 10" << endl;
        return 0;
    }

    CreateThreadPlayer(vibor);

    WaitForMultipleObjects(vibor, pThread, TRUE, INFINITE);

    SetEvent(pEvent); // гарантируем пробуждение

    WaitForSingleObject(bThread, INFINITE);

    system("cls");

    cout << "БОЙ ОКОНЧЕН!" << endl;

    if (boss.health <= 0)
        cout << "Игроки победили!" << endl;
    else
        cout << "Босс победил!" << endl;

    // сортировка по урону
    for (int i = 0; i < vibor - 1; i++)
    {
        for (int j = 0; j < vibor - i - 1; j++)
        {
            if (players[j].totalDamage < players[j + 1].totalDamage)
            {
                Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }

    int colvopl = (vibor < 3) ? vibor : 3;

    cout << endl << "Топ игроки по урону:" << endl;

    for (int i = 0; i < colvopl; i++)
    {
        cout << players[i].name << " -> "
             << players[i].totalDamage << endl;
    }

    return 0;
}
