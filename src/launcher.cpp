#include <iostream>
#include <cstdlib>

int main() {
    while (true) {
        std::cout << "\n=== ДЗ-1 (Вариант 14) ===\n";
        std::cout << "1 - Запустить задачу 1 (ML) + визуализации\n";
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
            std::cout << "Запуск задачи 1 (ML): C++ + визуализации...\n";

            // ВАЖНО:
            // - Запускаем из ML-part1, чтобы data/ и results/ работали как раньше.
            // - Затем запускаем Python-скрипты, которые строят графики.
            //
            // Если вдруг python-скрипты упадут из-за отсутствия matplotlib и т.п.,
            // программа продолжит работу и покажет код завершения.
            int code = std::system(
                "cd ML-part1 && "
                "../build/bin/project1 && "
                "python3 py/plot_correlation_heatmap.py && "
                "python3 py/plot_eigenvalues.py && "
                "python3 py/plot_factor_loadings.py"
            );

            std::cout << "Код завершения: " << code << "\n";
        }
        else if (choice == 2) {
            std::cout << "Запуск задачи 2 (3DES)...\n";

            // Запускаем из 3DES-part2, чтобы data/ и results/ совпадали с ожидаемыми путями.
            int code = std::system("cd 3DES-part2 && ../build/bin/three_des");

            std::cout << "Код завершения: " << code << "\n";
        }
        else {
            std::cout << "Неверный выбор.\n";
        }
    }
}
