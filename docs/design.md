# Deliverable 1 – Initial Planning and Design
Course: MSCS-632-M20 – Advanced Programming Languages  
Semester: Spring 2026  
Team: Team #2  Sandeep Parupalli, Nikhil Kannekanti & Nasser Hasan Padilla
Project: Cross-Language Expense Tracker (Python + C++)  
Repository: mscs632-spring2026-team2-expense-tracker

---

## 1. Project Overview
This project implements an Expense Tracker application in both Python and C++. The purpose is to compare how each language supports common application functionality (data storage, filtering/search, summarization) while highlighting language-specific strengths.

No UI is required per instructor guidance. Instead, each implementation will provide core logic through an ExpenseTracker module/class and a small driver program to run test scenarios and print results.

---

## 2. Core Functional Requirements
Both implementations must support the same core features:

1. Store expenses with fields: date, amount, category, description (plus an auto-generated ID).
2. Filter/search expenses by:
   - category
   - date range (start date to end date)
3. Summary reporting:
   - total expenses overall
   - totals grouped by category

---

## 3. Data Model
### 3.1 Expense Fields
- id: integer (auto-increment)
- date: YYYY-MM-DD (Python stored as datetime.date; C++ stored as string with standardized format)
- amount: float/double (> 0)
- category: non-empty string
- description: optional string

### 3.2 Validation Rules
- Date must be valid and formatted as YYYY-MM-DD.
- Amount must be numeric and greater than 0.
- Category must be non-empty.

---

## 4. Architecture (No UI)
Driver (main) → ExpenseTracker → In-memory Storage

### Components
- Expense model:
  - Python: dict-based expense records
  - C++: struct (or class) Expense with typed fields
- ExpenseTracker:
  - add expense
  - list/retrieve expenses
  - filter by category
  - filter by date range
  - compute summaries
- Driver program:
  - adds sample expenses
  - calls functions
  - prints results for verification and presentation demo

---

## 5. Python Implementation Plan
### Storage & Features
- Use list of dictionaries for expenses: list[dict]
- Use datetime for parsing and comparisons.

### API (planned)
- add_expense(date_str, amount, category, description) -> int
- get_all_expenses() -> list
- filter_by_category(category) -> list
- filter_by_date_range(start_date_str, end_date_str) -> list
- summary_total() -> float
- summary_by_category() -> dict[str, float]

### Language-Specific Focus
- Dictionaries and dynamic typing
- datetime parsing/comparisons
- Exception handling for validation

---

## 6. C++ Implementation Plan
### Storage & Features
- Define struct/class Expense
- Store with std::vector<Expense>
- Summary by category via std::unordered_map<string, double>

### API (planned)
- int addExpense(string date, double amount, string category, string description);
- vector<Expense> getAllExpenses() const;
- vector<Expense> filterByCategory(string category) const;
- vector<Expense> filterByDateRange(string start, string end) const;
- double summaryTotal() const;
- unordered_map<string, double> summaryByCategory() const;

### Language-Specific Focus
- Structs/classes and strong typing
- STL containers and iteration
- Memory management via RAII/value semantics (no manual new/delete)

---

## 7. Task Assignment (3 Members)
- Member 1 Sandeep Parupalli (Python Lead): Python tracker module + driver program, datetime parsing, validation.
- Member 2 Nikhil Kannekanti (C++ Lead): C++ tracker class/structs + driver program, STL summary map, validation scaffolding.
- Member 3 Nasser Hasan Padilla (Docs/Integrator): Repo structure, docs completion, parity checklist, test plan, merge coordination.

---

## 8. Timeline (Milestones)
### Day 1 (Friday)
- Repo created and structured
- Data model and APIs finalized
- Design documentation completed
- Skeleton code files created (stubs + driver placeholders)

### Day 2 (Saturday)
- Implement core logic in both languages (add/list/filter/summary)
- Run acceptance tests via driver programs
- Fix major bugs and ensure feature parity

### Day 3 (Sunday)
- Code cleanup and comments
- Write 2–3 page APA comparison report with code snippets
- Prepare 5–10 minute presentation demo using driver outputs
- Final push to GitHub and include link in report

---

## 9. Anticipated Challenges & Mitigations
- Python: dynamic typing requires explicit validation (mitigate with consistent checks and exceptions).
- C++: date parsing/comparison more verbose (mitigate by standardizing YYYY-MM-DD and implementing a helper parser).
- Keeping parity: maintain feature_parity.md and test_plan.md to ensure both implementations behave consistently.
