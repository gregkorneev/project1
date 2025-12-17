#include <iostream>
#include <cstdlib>

int main() {
    while (true) {
        std::cout << "\n=== ДЗ-1 (Вариант 14) ===\n";
        std::cout << "1 - Запустить задачу 1 (ML)\n";
        std::cout << "2 - Запустить задачу 2 (3DES)\n";
        std::cout << "0 - Выход\n";
        std::cout << "Выбор: ";

        int choice = -1;
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Выход.\n";
            return 0;
        }

        if (choice == 1) {
            std::cout << "Запуск задачи 1 (ML)...\n";
            // Важно: сперва перейти в ML-part1 (чтобы data/ работали),
            // потом запустить бинарник по абсолютному пути от корня проекта:
            std::system("cd ML-part1 && ../build/bin/project1");
        } else if (choice == 2) {
            std::cout << "Запуск задачи 2 (3DES)...\n";
            std::system("cd 3DES-part2 && ../build/bin/three_des");
        } else {
            std::cout << "Неверный выбор.\n";
        }
    }
}
