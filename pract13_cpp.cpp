#include <iostream>
#include <windows.h>

using namespace std;

string names[10]{ "Иван", "Роман", "Пётр", "Николай", "Олег", "Александр", "Тимофей", "Гога", "Игнат", "Антон" };

HANDLE pThread[10];
HANDLE bThread;

HANDLE hMutex; // мьютекс
HANDLE pEvent; // событие (ход игроков)

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

// ---------------- ИГРОК ----------------
DWORD WINAPI GamePlay(LPVOID param)
{
    GamePlayPlayer* gpl = (GamePlayPlayer*)param;

    while (!gameOver)
    {
        // ждём разрешение (event)
        WaitForSingleObject(pEvent, INFINITE);

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
             << " | HP босса: " << boss.health << endl;

        ReleaseMutex(hMutex);

        Sleep(gpl->pl->attckCooldown * 1000);

        // разрешаем следующему потоку
        SetEvent(pEvent);
    }

    return 0;
}

// ---------------- БОСС ----------------
DWORD WINAPI BossPlay(LPVOID param)
{
    int colvo = *(int*)param;

    while (!gameOver)
    {
        Sleep(boss.attackCooldown * 1000);

        WaitForSingleObject(hMutex, INFINITE);

        if (boss.health <= 0)
        {
            gameOver = true;
            ReleaseMutex(hMutex);
            break;
        }

        // атака случайного игрока
        int target = rand() % colvo;

        if (players[target].health > 0)
        {
            int attackBoss = boss.damage * (100 - players[target].defense) / 100;
            players[target].health -= attackBoss;

            cout << "БОСС ударил " << players[target].name
                 << " на " << attackBoss
                 << " | HP игрока: " << players[target].health << endl;
        }

        ReleaseMutex(hMutex);

        Sleep(boss.specialCooldown * 1000);

        WaitForSingleObject(hMutex, INFINITE);

        if (boss.health <= 0)
        {
            gameOver = true;
            ReleaseMutex(hMutex);
            break;
        }

        cout << "БОСС использует массовую атаку!" << endl;

        for (int i = 0; i < colvo; i++)
        {
            if (players[i].health <= 0) continue;

            int damage;

            if (colvo > 1)
                damage = boss.specialDamage * (1 - 0.05 * (colvo - 1));
            else
                damage = boss.specialDamage;

            players[i].health -= damage;

            cout << players[i].name << " получил " << damage
                 << " | HP: " << players[i].health << endl;
        }

        ReleaseMutex(hMutex);
    }

    return 0;
}

// ---------------- СОЗДАНИЕ ПОТОКОВ ----------------
void CreateThreadPlayer(int colvo)
{
    for (int i = 0; i < colvo; i++)
    {
        strcpy_s(players[i].name, names[i].c_str());

        GamePlayPlayer* player = new GamePlayPlayer;
        player->pl = &players[i];
        player->colvoPlayers = colvo;

        pThread[i] = CreateThread(NULL, 0, GamePlay, player, 0, 0);

        if (pThread[i] == NULL)
        {
            cout << "Ошибка создания потока игрока\n";
            return;
        }
    }

    bThread = CreateThread(NULL, 0, BossPlay, &colvo, 0, 0);
}

// ---------------- MAIN ----------------
int main()
{
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    hMutex = CreateMutex(NULL, FALSE, NULL);

    // auto-reset event (по одному игроку за раз)
    pEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

    int vibor = 0;

    cout << "Сколько игроков? (1-10): ";
    cin >> vibor;

    if (vibor < 1 || vibor > 10)
    {
        cout << "Ошибка!\n";
        return 0;
    }

    CreateThreadPlayer(vibor);

    WaitForMultipleObjects(vibor, pThread, TRUE, INFINITE);
    WaitForSingleObject(bThread, INFINITE);

    cout << "\n=== БОЙ ОКОНЧЕН ===\n";

    if (boss.health <= 0)
        cout << "Игроки победили!\n";
    else
        cout << "Босс победил!\n";

    cout << "\nУрон игроков:\n";
    for (int i = 0; i < vibor; i++)
    {
        cout << players[i].name << " -> " << players[i].totalDamage << endl;
    }

    return 0;
}
