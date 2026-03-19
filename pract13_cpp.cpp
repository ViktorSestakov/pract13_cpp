#include <iostream>
#include <windows.h>

using namespace std;

string names[10]{ "Иван", "Роман", "Пётр", "Николай", "Олег", "Александр", "Тимофей", "Гога", "Игнат", "Антон" };

HANDLE pThread[10];
HANDLE bThread;

const char* PlayerEvent = "PlayerEvent";

HANDLE pEvent = CreateEventA(NULL, FALSE, FALSE, PlayerEvent);

struct Bayum {
	long health = 9000000000;	// здоровье босса
	int resist = 44;			// споротивление атакам
	int damage = 73843;			// урон обычной атаки
	int specialDamage = 150000;	// урон спецатаки
	int attackCooldown = 5;		// время между обычными атакми (сек)
	int specialCooldown = 10;	// время между спецатаками (сек)
};

struct Player {
	long health = 500000;		// здоровье игрока
	int damage = 12000;			// урон обычной атаки
	int specialDamage = 30000;	// урон спецатакаи
	int attckCooldown = 2;		// время между обычными атаками (сек)
	int specialCooldown = 5;	// время между спецатаками (сек)
	int defense = 20;			// защита (уменьшение урона)
	int dodgeChance = 15;		// шанс уклонения от атаки (%)
	char name[64];				// имя игрока
};

struct GamePlayPlayer
{
	Player pl;
	int colvoPlayers;
};

Player players[10];
Bayum boss;

void Lose() {

}

void Win() {

}

void GamePlay(GamePlayPlayer gpl) {
	int attackPlayer = gpl.pl.damage * (100 - boss.resist) / 100;
	int spAttackPlayer = gpl.pl.specialDamage * (1 - (boss.resist * 0.01));
	
	int attackBoss = boss.damage * (1 - (gpl.pl.defense * 0.01));
	int spAttackBoss = boss.specialDamage * 0.05 * (gpl.colvoPlayers - 1);
	
	// обычная атака игрока
	if (gpl.pl.health > 0) {
		boss.health -= attackPlayer;
		Sleep(gpl.pl.attckCooldown * 1000);
	}

	// спец атака игрока
	if (gpl.pl.health > 0) {
		boss.health -= spAttackPlayer;
		Sleep(gpl.pl.specialCooldown * 1000);
	}

	// обычная атака босса
	if (boss.health > 0) {
		gpl.pl.health -= attackBoss;
		Sleep(boss.attackCooldown * 1000);
	}

	// спец атак босса
	if (boss.health > 0) {
		if (gpl.colvoPlayers > 1) {
			gpl.pl.health -= spAttackBoss;
		}
		else {
			gpl.pl.health -= boss.specialDamage;
		}
		Sleep(boss.specialCooldown * 1000);
	}
}

void CreateThreadPlayer(int colvo) {
	for (int i = 0; i < colvo; i++)
	{
		GamePlayPlayer player;
		player.colvoPlayers = colvo;

		strcpy_s(players[i].name, names[i].c_str());
		player.pl = players[i];

		pThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GamePlay, &player, 0, 0);

		if (pThread[i] == NULL)
		{
			cout << "Не удалось создать поток!" << endl;
			return;
		}

		if (pEvent == NULL) {
			cout << "Не удалось создать поток!" << endl;
			return;
		}
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	
	int vibor = 0;

	cout << "Добро пожаловать на битву с боссом!" << endl;
	cout << "Сколько будет человек? (от 1 до 10) - ";
	cin >> vibor;
	
	if (vibor > 0 && vibor < 11) {
		CreateThreadPlayer(vibor);
	}
	else {
		cout << "Не верно выбрано кол-во человек!" << endl;
	}

	Sleep(5000);

	return 0;
}
