#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct Expense {
    int id{};
    std::string date;        // YYYY-MM-DD
    double amount{};
    std::string category;
    std::string description;
};

class ExpenseTracker {
public:
    explicit ExpenseTracker(std::filesystem::path data_file);

    void load();
    void save() const;

    void add_expense();
    void list_expenses();
    void search_expenses();
    void summary();

    void menu_loop();

private:
    std::filesystem::path data_file_;
    std::vector<Expense> expenses_;

    static std::string prompt(const std::string& msg);
    static bool is_valid_date(const std::string& s);
    static bool parse_amount(const std::string& s, double& out);

    int next_id() const;

    static void print_table(const std::vector<Expense>& rows);

    static void prompt_date_range(std::string& start_date, std::string& end_date);
    static std::vector<Expense> filter_by_date(const std::vector<Expense>& rows,
                                               const std::string& start_date,
                                               const std::string& end_date);
};
