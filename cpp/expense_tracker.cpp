#include <iostream>
#include <string>

int main() {
    while (true) {
        std::cout << "\n===== Expense Tracker (C++) =====\n";
        std::cout << "1) Add expense\n";
        std::cout << "2) List expenses\n";
        std::cout << "3) Search expenses\n";
        std::cout << "4) Summary\n";
        std::cout << "5) Save\n";
        std::cout << "6) Load\n";
        std::cout << "7) Quit\n";
        std::cout << "Choose an option (1-7): ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "7") {
            std::cout << "Goodbye!\n";
            break;
        } else if (choice == "1" || choice == "2" || choice == "3" ||
                   choice == "4" || choice == "5" || choice == "6") {
            std::cout << "Not implemented yet — scheduled for Day 2 core implementation.\n";
        } else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
