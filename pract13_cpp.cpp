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

void GamePlay(GamePlayPlayer gpl)
{
	while (!gameOver)
	{
		WaitForSingleObject(pEvent, INFINITE);

		WaitForSingleObject(hMutex, INFINITE);

		if (boss.health <= 0 || gpl.pl->health <= 0)
		{
			ReleaseMutex(hMutex);
			break;
		}

		int attackPlayer = gpl.pl->damage * (100 - boss.resist) / 100;
		boss.health -= attackPlayer;
		gpl.pl->totalDamage += attackPlayer;

		cout << gpl.pl->name << " ударил босса на " << attackPlayer << " | HP босса: " << boss.health << endl;

		ReleaseMutex(hMutex);
		Sleep(gpl.pl->attckCooldown * 1000);

		SetEvent(pEvent);
	}
}

void BossPlay(int colvo)
{
	int alive = 0;

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

		int index = rand() % colvo;

		if (players[index].health > 0)
		{
			int attackBoss = boss.damage * (100 - players[index].defense) / 100;
			players[index].health -= attackBoss;

			cout << "БОСС ударил " << players[index].name << " на " << attackBoss << " | HP игрока: " << players[index].health << endl;
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

		cout << "БОСС использует специальную атаку!" << endl;

		for (int i = 0; i < colvo; i++)
		{
			int damage;

			if (colvo > 1)
				damage = boss.specialDamage * (1 - 0.05 * (colvo - 1));
			else
				damage = boss.specialDamage;

			if (players[i].health >= damage) {
				players[i].health -= damage;

				cout << players[i].name << " получил " << damage << " | HP: " << players[i].health << endl;
			}
			else {
				alive++;
			}
		}

		if (alive == colvo) {
			gameOver = true;
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

		pThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GamePlay, (void*)player, 0, 0);

		if (pThread[i] == NULL)
		{
			cout << "Ошибка создания потока игрока" << endl;
			return;
		}
	}

	bThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BossPlay, (void*)colvo, 0, 0);
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
		cout << "Ошибка! Выберите число в диапазоне от 1 до 10" << endl;
		return 0;
	}

	CreateThreadPlayer(vibor);

	WaitForMultipleObjects(vibor, pThread, TRUE, INFINITE);
	WaitForSingleObject(bThread, INFINITE);

	system("cls");
	cout << "БОЙ ОКОНЧЕН!" << endl;

	if (boss.health <= 0)
		cout << "Игроки победили!" << endl;
	else
		cout << "Босс победил!" << endl;

	cout << "Урон игроков:" << endl;
	for (int i = 0; i < vibor; i++)
	{
		cout << players[i].name << " -> " << players[i].totalDamage << endl;
	}

	return 0;
}
