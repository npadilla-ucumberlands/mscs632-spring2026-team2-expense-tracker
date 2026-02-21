def main():
    while True:
        print("\n===== Expense Tracker (Python) =====")
        print("1) Add expense (TODO Day 2)")
        print("2) List expenses (TODO Day 2)")
        print("3) Search expenses (TODO Day 2)")
        print("4) Summary (TODO Day 2)")
        print("5) Save (TODO Day 2)")
        print("6) Load (TODO Day 2)")
        print("7) Quit")

        choice = input("Choose an option (1-7): ").strip()

        if choice == "7":
            print("Goodbye!")
            break
        elif choice in {"1", "2", "3", "4", "5", "6"}:
            print("Not implemented yet — scheduled for Day 2 core implementation.")
        else:
            print("Invalid choice. Try again.")


if __name__ == "__main__":
    main()

