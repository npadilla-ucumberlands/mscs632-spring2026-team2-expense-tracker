from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import List, Optional, Dict


# Project root is ../../ from python/src/
DATA_FILE = Path(__file__).resolve().parents[2] / "data" / "expenses.tsv"


@dataclass
class Expense:
    id: int
    date: str         # ISO YYYY-MM-DD
    amount: float
    category: str
    description: str


def _prompt(msg: str) -> str:
    return input(msg).strip()


def _parse_date(date_str: str) -> Optional[str]:
    """Return normalized ISO date (YYYY-MM-DD) or None if invalid."""
    try:
        dt = datetime.strptime(date_str.strip(), "%Y-%m-%d")
        return dt.strftime("%Y-%m-%d")
    except ValueError:
        return None


def _parse_amount(amount_str: str) -> Optional[float]:
    """Return float amount or None if invalid or negative."""
    try:
        val = float(amount_str.strip())
        if val < 0:
            return None
        return val
    except ValueError:
        return None


def _ensure_data_file() -> None:
    DATA_FILE.parent.mkdir(parents=True, exist_ok=True)
    if not DATA_FILE.exists():
        DATA_FILE.write_text("id\tdate\tamount\tcategory\tdescription\n", encoding="utf-8")


def load_expenses() -> List[Expense]:
    _ensure_data_file()
    expenses: List[Expense] = []

    lines = DATA_FILE.read_text(encoding="utf-8").splitlines()
    if not lines:
        _ensure_data_file()
        return expenses

    # Skip header if present
    start_idx = 1 if lines[0].lower().startswith("id\tdate\tamount") else 0

    for line in lines[start_idx:]:
        if not line.strip():
            continue
        parts = line.split("\t")
        if len(parts) < 5:
            continue
        try:
            eid = int(parts[0])
            date = parts[1]
            amount = float(parts[2])
            category = parts[3]
            description = "\t".join(parts[4:])  # in case tabs slipped in
            expenses.append(Expense(eid, date, amount, category, description))
        except ValueError:
            continue

    return expenses


def save_expenses(expenses: List[Expense]) -> None:
    _ensure_data_file()
    lines = ["id\tdate\tamount\tcategory\tdescription"]
    for e in expenses:
        # Keep TSV safe
        category = e.category.replace("\t", " ").strip()
        description = e.description.replace("\t", " ").strip()
        lines.append(f"{e.id}\t{e.date}\t{e.amount:.2f}\t{category}\t{description}")
    DATA_FILE.write_text("\n".join(lines) + "\n", encoding="utf-8")


def _next_id(expenses: List[Expense]) -> int:
    return (max((e.id for e in expenses), default=0) + 1)


def add_expense(expenses: List[Expense]) -> None:
    print("\nAdd Expense")
    print("-----------")

    date_in = _prompt("Date (YYYY-MM-DD): ")
    date_norm = _parse_date(date_in)
    if date_norm is None:
        print("Error: Invalid date. Use YYYY-MM-DD (example: 2026-02-20).")
        return

    amt_in = _prompt("Amount (non-negative): ")
    amt = _parse_amount(amt_in)
    if amt is None:
        print("Error: Invalid amount. Enter a non-negative number (example: 12.50).")
        return

    category = _prompt("Category: ")
    if not category:
        print("Error: Category cannot be empty.")
        return

    description = _prompt("Description: ")
    if not description:
        print("Error: Description cannot be empty.")
        return

    eid = _next_id(expenses)
    expenses.append(Expense(eid, date_norm, amt, category, description))
    print(f"Added expense with ID {eid}.")


def _prompt_date_range() -> tuple[Optional[str], Optional[str]]:
    """
    Returns (start_date, end_date) as ISO strings or None.
    Blank means no filter.
    """
    start_in = _prompt("Start date (YYYY-MM-DD) [blank for none]: ")
    start = None
    if start_in:
        start = _parse_date(start_in)
        if start is None:
            print("Error: invalid start date.")
            return (None, None)

    end_in = _prompt("End date (YYYY-MM-DD) [blank for none]: ")
    end = None
    if end_in:
        end = _parse_date(end_in)
        if end is None:
            print("Error: invalid end date.")
            return (None, None)

    if start and end and start > end:
        print("Error: start date cannot be after end date.")
        return (None, None)

    return (start, end)


def _filter_by_date(expenses: List[Expense], start: Optional[str], end: Optional[str]) -> List[Expense]:
    out: List[Expense] = []
    for e in expenses:
        if start and e.date < start:
            continue
        if end and e.date > end:
            continue
        out.append(e)
    return out


def list_expenses(expenses: List[Expense]) -> None:
    print("\nList Expenses")
    print("-------------")
    if not expenses:
        print("No expenses found.")
        return

    start, end = _prompt_date_range()
    if start is None and end is None and (_prompt is None):  # never true; keeps lint calm
        pass

    # If user entered invalid dates, _prompt_date_range returns (None, None) but already printed error.
    # We still list all only if both blanks were intended. So ask if they want to continue:
    if (start, end) == (None, None):
        # Could be blanks OR invalid; re-ask if they want filtering.
        # We'll proceed listing all (safe default).
        filtered = expenses[:]
    else:
        filtered = _filter_by_date(expenses, start, end)

    if not filtered:
        print("\nNo expenses match your filters.")
        return

    filtered.sort(key=lambda x: (x.date, x.id))

    print("\nID  Date       Amount   Category   Description")
    print("--  ---------  -------  ---------  ------------------------------")
    for e in filtered:
        amt = f"{e.amount:.2f}"
        print(f"{e.id:<3} {e.date:<10} {amt:>7}  {e.category:<9}  {e.description}")

    print(f"\nShowing {len(filtered)} expense(s).")


def search_expenses(expenses: List[Expense]) -> None:
    print("\nSearch Expenses")
    print("--------------")
    if not expenses:
        print("No expenses found.")
        return

    keyword = _prompt("Keyword (searches Category + Description): ").lower()
    if not keyword:
        print("Error: keyword cannot be empty.")
        return

    start, end = _prompt_date_range()
    base = _filter_by_date(expenses, start, end) if (start or end) else expenses

    matches: List[Expense] = []
    for e in base:
        hay = f"{e.category} {e.description}".lower()
        if keyword in hay:
            matches.append(e)

    if not matches:
        print("\nNo expenses match your search.")
        return

    matches.sort(key=lambda x: (x.date, x.id))

    print("\nID  Date       Amount   Category   Description")
    print("--  ---------  -------  ---------  ------------------------------")
    for e in matches:
        amt = f"{e.amount:.2f}"
        print(f"{e.id:<3} {e.date:<10} {amt:>7}  {e.category:<9}  {e.description}")

    print(f"\nFound {len(matches)} match(es).")


def summary(expenses: List[Expense]) -> None:
    print("\nSummary")
    print("-------")
    if not expenses:
        print("No expenses found.")
        return

    start, end = _prompt_date_range()
    subset = _filter_by_date(expenses, start, end) if (start or end) else expenses

    if not subset:
        print("\nNo expenses match your filters.")
        return

    total = sum(e.amount for e in subset)
    count = len(subset)
    avg = total / count if count else 0.0

    by_cat: Dict[str, float] = {}
    for e in subset:
        by_cat[e.category] = by_cat.get(e.category, 0.0) + e.amount

    print(f"\nCount: {count}")
    print(f"Total: {total:.2f}")
    print(f"Average: {avg:.2f}")

    print("\nTotals by Category")
    print("------------------")
    for cat, amt in sorted(by_cat.items(), key=lambda kv: (-kv[1], kv[0].lower())):
        print(f"{cat:<15} {amt:.2f}")


def menu_loop() -> None:
    expenses = load_expenses()

    while True:
        print("\n===== Expense Tracker (Python) =====")
        print("1) Add expense")
        print("2) List expenses")
        print("3) Search expenses")
        print("4) Summary")
        print("5) Save")
        print("6) Load")
        print("7) Quit")

        choice = _prompt("Choose an option (1-7): ")

        if choice == "1":
            add_expense(expenses)
        elif choice == "2":
            list_expenses(expenses)
        elif choice == "3":
            search_expenses(expenses)
        elif choice == "4":
            summary(expenses)
        elif choice == "5":
            save_expenses(expenses)
            print(f"Saved to {DATA_FILE}.")
        elif choice == "6":
            expenses = load_expenses()
            print(f"Loaded from {DATA_FILE}.")
        elif choice == "7":
            print("Goodbye!")
            break
        else:
            print("Invalid choice. Try again.")


if __name__ == "__main__":
    menu_loop()
