#include <stdio.h>

// All functions written BEFORE main()

void inputMarks(int marks[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter marks of student %d: ", i + 1);
        scanf("%d", &marks[i]);
    }
}

void displayMarks(int marks[], int n) {
    printf("\nMarks of Students:\n");
    for (int i = 0; i < n; i++) {
        printf("Student %d: %d (Grade: %c)\n", i + 1, marks[i],
               (marks[i] >= 90) ? 'A' :
               (marks[i] >= 75) ? 'B' :
               (marks[i] >= 60) ? 'C' :
               (marks[i] >= 35) ? 'D' : 'F');
    }
}

int findHighest(int marks[], int n) {
    int high = marks[0];
    for (int i = 1; i < n; i++) {
        if (marks[i] > high)
            high = marks[i];
    }
    return high;
}

int findLowest(int marks[], int n) {
    int low = marks[0];
    for (int i = 1; i < n; i++) {
        if (marks[i] < low)
            low = marks[i];
    }
    return low;
}

float findAverage(int marks[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += marks[i];
    return (float)sum / n;
}

int countPass(int marks[], int n) {
    int count = 0;
    for (int i = 0; i < n; i++)
        if (marks[i] >= 35)
            count++;
    return count;
}

void sortMarks(int marks[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (marks[j] > marks[j + 1]) {
                int temp = marks[j];
                marks[j] = marks[j + 1];
                marks[j + 1] = temp;
            }
        }
    }
}

// ================== MAIN FUNCTION ==================

int main() {
    int n, choice;

    printf("Enter number of students: ");
    scanf("%d", &n);

    int marks[n];

    do {
        printf("\n==== STUDENT MARKS ANALYZER ====\n");
        printf("1. Enter Marks\n");
        printf("2. Display Marks\n");
        printf("3. Highest Mark\n");
        printf("4. Lowest Mark\n");
        printf("5. Average Marks\n");
        printf("6. Count Pass/Fail\n");
        printf("7. Sort Marks\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                inputMarks(marks, n);
                break;

            case 2:
                displayMarks(marks, n);
                break;

            case 3:
                printf("Highest Mark = %d\n", findHighest(marks, n));
                break;

            case 4:
                printf("Lowest Mark = %d\n", findLowest(marks, n));
                break;

            case 5:
                printf("Average Marks = %.2f\n", findAverage(marks, n));
                break;

            case 6: {
                int pass = countPass(marks, n);
                printf("Pass Count = %d\n", pass);
                printf("Fail Count = %d\n", n - pass);
                break;
            }

            case 7:
                sortMarks(marks, n);
                printf("Marks sorted successfully!\n");
                displayMarks(marks, n);
                break;

            case 8:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 8);

    return 0;
}
