#include "expense_tracker.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>

static std::string to_lower(std::string s) {
    for (char& ch : s) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}

static std::vector<std::string> split_tab(const std::string& line) {
    std::vector<std::string> parts;
    std::string cur;
    for (char c : line) {
        if (c == '\t') {
            parts.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    parts.push_back(cur);
    return parts;
}

ExpenseTracker::ExpenseTracker(std::filesystem::path data_file)
    : data_file_(std::move(data_file)) {}

std::string ExpenseTracker::prompt(const std::string& msg) {
    std::cout << msg;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

bool ExpenseTracker::is_valid_date(const std::string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;

    std::tm tm{};
    std::istringstream iss(s);
    iss >> std::get_time(&tm, "%Y-%m-%d");
    if (iss.fail()) return false;

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str() == s;
}

bool ExpenseTracker::parse_amount(const std::string& s, double& out) {
    try {
        size_t idx = 0;
        double val = std::stod(s, &idx);
        if (idx != s.size()) return false;
        if (val < 0.0) return false;
        out = val;
        return true;
    } catch (...) {
        return false;
    }
}

int ExpenseTracker::next_id() const {
    int mx = 0;
    for (const auto& e : expenses_) mx = std::max(mx, e.id);
    return mx + 1;
}

void ExpenseTracker::load() {
    expenses_.clear();

    std::filesystem::create_directories(data_file_.parent_path());

    if (!std::filesystem::exists(data_file_)) {
        std::ofstream out(data_file_);
        out << "id\tdate\tamount\tcategory\tdescription\n";
        return;
    }

    std::ifstream in(data_file_);
    if (!in) {
        std::cout << "Error: Could not open file for reading: " << data_file_ << "\n";
        return;
    }

    std::string line;
    bool first = true;
    while (std::getline(in, line)) {
        if (first) { first = false; continue; }
        if (line.empty()) continue;

        auto cols = split_tab(line);
        if (cols.size() < 5) continue;

        Expense e;
        try {
            e.id = std::stoi(cols[0]);
            e.date = cols[1];
            e.amount = std::stod(cols[2]);
            e.category = cols[3];
            e.description = cols[4];
        } catch (...) {
            continue;
        }

        expenses_.push_back(std::move(e));
    }
}

void ExpenseTracker::save() const {
    std::filesystem::create_directories(data_file_.parent_path());

    std::ofstream out(data_file_);
    if (!out) {
        std::cout << "Error: Could not open file for writing: " << data_file_ << "\n";
        return;
    }

    out << "id\tdate\tamount\tcategory\tdescription\n";

    std::vector<Expense> rows = expenses_;
    std::sort(rows.begin(), rows.end(), [](const Expense& a, const Expense& b) {
        if (a.date != b.date) return a.date < b.date;
        return a.id < b.id;
    });

    out << std::fixed << std::setprecision(2);
    for (const auto& e : rows) {
        out << e.id << "\t" << e.date << "\t" << e.amount << "\t"
            << e.category << "\t" << e.description << "\n";
    }
}

void ExpenseTracker::prompt_date_range(std::string& start_date, std::string& end_date) {
    start_date = prompt("Start date (YYYY-MM-DD) [blank for none]: ");
    end_date   = prompt("End date (YYYY-MM-DD) [blank for none]: ");

    if (!start_date.empty() && !is_valid_date(start_date)) {
        std::cout << "Error: Invalid start date. Use YYYY-MM-DD (example: 2026-02-20).\n";
        start_date.clear();
        end_date.clear();
        return;
    }
    if (!end_date.empty() && !is_valid_date(end_date)) {
        std::cout << "Error: Invalid end date. Use YYYY-MM-DD (example: 2026-02-20).\n";
        start_date.clear();
        end_date.clear();
        return;
    }
    if (!start_date.empty() && !end_date.empty() && start_date > end_date) {
        std::cout << "Error: start date cannot be after end date.\n";
        start_date.clear();
        end_date.clear();
        return;
    }
}

std::vector<Expense> ExpenseTracker::filter_by_date(const std::vector<Expense>& rows,
                                                    const std::string& start_date,
                                                    const std::string& end_date) {
    std::vector<Expense> out;
    out.reserve(rows.size());
    for (const auto& e : rows) {
        if (!start_date.empty() && e.date < start_date) continue;
        if (!end_date.empty() && e.date > end_date) continue;
        out.push_back(e);
    }
    return out;
}

void ExpenseTracker::print_table(const std::vector<Expense>& rows) {
    std::cout << "\nID  Date       Amount   Category   Description\n";
    std::cout << "--  ---------- -------  ---------  ------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);

    for (const auto& e : rows) {
        std::ostringstream amt;
        amt << std::fixed << std::setprecision(2) << e.amount;

        std::cout << std::setw(2) << e.id << "  "
                  << std::setw(10) << e.date << " "
                  << std::setw(7) << amt.str() << "  "
                  << std::setw(9) << e.category << "  "
                  << e.description << "\n";
    }
}

void ExpenseTracker::add_expense() {
    std::cout << "\nAdd Expense\n";
    std::cout << "-----------\n";

    std::string date = prompt("Date (YYYY-MM-DD): ");
    if (!is_valid_date(date)) {
        std::cout << "Error: Invalid date. Use YYYY-MM-DD (example: 2026-02-20).\n";
        return;
    }

    std::string amount_str = prompt("Amount (non-negative): ");
    double amount = 0.0;
    if (!parse_amount(amount_str, amount)) {
        std::cout << "Error: Invalid amount. Enter a non-negative number (example: 12.50).\n";
        return;
    }

    std::string category = prompt("Category: ");
    if (category.empty()) {
        std::cout << "Error: Category cannot be empty.\n";
        return;
    }

    std::string description = prompt("Description: ");
    if (description.empty()) {
        std::cout << "Error: Description cannot be empty.\n";
        return;
    }

    Expense e;
    e.id = next_id();
    e.date = date;
    e.amount = amount;
    e.category = category;
    e.description = description;

    expenses_.push_back(std::move(e));
    std::cout << "Added expense with ID " << expenses_.back().id << ".\n";
}

void ExpenseTracker::list_expenses() {
    std::cout << "\nList Expenses\n";
    std::cout << "------------\n";

    if (expenses_.empty()) {
        std::cout << "No expenses found.\n";
        return;
    }

    std::string start, end;
    prompt_date_range(start, end);

    std::vector<Expense> filtered =
        (start.empty() && end.empty()) ? expenses_ : filter_by_date(expenses_, start, end);

    if (filtered.empty()) {
        std::cout << "\nNo expenses match your filters.\n";
        return;
    }

    std::sort(filtered.begin(), filtered.end(), [](const Expense& a, const Expense& b) {
        if (a.date != b.date) return a.date < b.date;
        return a.id < b.id;
    });

    print_table(filtered);
    std::cout << "\nShowing " << filtered.size() << " expense(s).\n";
}

void ExpenseTracker::search_expenses() {
    std::cout << "\nSearch Expenses\n";
    std::cout << "--------------\n";

    if (expenses_.empty()) {
        std::cout << "No expenses found.\n";
        return;
    }

    std::string keyword = prompt("Keyword (searches Category + Description): ");
    keyword = to_lower(keyword);
    if (keyword.empty()) {
        std::cout << "Error: keyword cannot be empty.\n";
        return;
    }

    std::string start, end;
    prompt_date_range(start, end);

    std::vector<Expense> base =
        (start.empty() && end.empty()) ? expenses_ : filter_by_date(expenses_, start, end);

    std::vector<Expense> matches;
    for (const auto& e : base) {
        std::string hay = to_lower(e.category + " " + e.description);
        if (hay.find(keyword) != std::string::npos) matches.push_back(e);
    }

    if (matches.empty()) {
        std::cout << "\nNo expenses match your search.\n";
        return;
    }

    std::sort(matches.begin(), matches.end(), [](const Expense& a, const Expense& b) {
        if (a.date != b.date) return a.date < b.date;
        return a.id < b.id;
    });

    print_table(matches);
    std::cout << "\nFound " << matches.size() << " match(es).\n";
}

void ExpenseTracker::summary() {
    std::cout << "\nSummary\n";
    std::cout << "-------\n";

    if (expenses_.empty()) {
        std::cout << "No expenses found.\n";
        return;
    }

    std::string start, end;
    prompt_date_range(start, end);

    std::vector<Expense> subset =
        (start.empty() && end.empty()) ? expenses_ : filter_by_date(expenses_, start, end);

    if (subset.empty()) {
        std::cout << "\nNo expenses match your filters.\n";
        return;
    }

    const int count = static_cast<int>(subset.size());
    const double total = std::accumulate(subset.begin(), subset.end(), 0.0,
        [](double acc, const Expense& e) { return acc + e.amount; });

    const double avg = (count > 0) ? (total / count) : 0.0;

    std::map<std::string, double> by_cat;
    for (const auto& e : subset) by_cat[e.category] += e.amount;

    std::vector<std::pair<std::string, double>> cats(by_cat.begin(), by_cat.end());
    std::sort(cats.begin(), cats.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second; // amount desc
        return to_lower(a.first) < to_lower(b.first);
    });

    std::cout << "\nCount: " << count << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total: " << total << "\n";
    std::cout << "Average: " << avg << "\n";

    std::cout << "\nTotals by Category\n";
    std::cout << "------------------\n";
    for (const auto& [cat, amt] : cats) {
        std::cout << std::left << std::setw(15) << cat << " " << amt << "\n";
    }
}

void ExpenseTracker::menu_loop() {
    load();

    while (true) {
        std::cout << "\n==== Expense Tracker (C++) ====\n";
        std::cout << "1) Add expense\n";
        std::cout << "2) List expenses\n";
        std::cout << "3) Search expenses\n";
        std::cout << "4) Summary\n";
        std::cout << "5) Save\n";
        std::cout << "6) Load\n";
        std::cout << "7) Quit\n";

        std::string choice = prompt("Choose an option (1-7): ");

        if (choice == "1") {
            add_expense();
        } else if (choice == "2") {
            list_expenses();
        } else if (choice == "3") {
            search_expenses();
        } else if (choice == "4") {
            summary();
        } else if (choice == "5") {
            save();
            std::cout << "Saved to " << data_file_ << ".\n";
        } else if (choice == "6") {
            load();
            std::cout << "Loaded from " << data_file_ << ".\n";
        } else if (choice == "7") {
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}
