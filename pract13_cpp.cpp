#include <iostream>
#include <windows.h>

using namespace std;

string names[10] {"Иван", "Роман", "Пётр", "Николай", "Олег", "Александр", "Тимофей", "Гога", "Игнат", "Антон"};

HANDLE pThread[10];
HANDLE bThread;

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

void PlayersPlay() {
	cout << "123" << endl;
}

void CreateThreadPlayer(int colvo) {
	for (int i = 0; i < colvo; i++)
	{
		pThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PlayersPlay, NULL, 0, 0);

		if (pThread[i] == NULL)
		{
			cout << "Не удалось создать поток!" << endl;
			return;
		}
	}
}

void BossPlay() {

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

	return 0;
}