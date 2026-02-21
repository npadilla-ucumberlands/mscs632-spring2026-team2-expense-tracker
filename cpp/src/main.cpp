#include "expense_tracker.h"
#include <filesystem>

int main() {
    std::filesystem::path data_file =
        std::filesystem::current_path() / "data" / "expenses.tsv";

    ExpenseTracker tracker(data_file);
    tracker.menu_loop();
    return 0;
}
