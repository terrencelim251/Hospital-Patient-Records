/*Member 1
Student Name:  Terrence Lim Jun Jay
Student ID:  242DT243F0
*/

/*Member 2
Student Name:  Yong Zong Yan
Student ID:  242DT2422R
*/

/*Member 3
Student Name:  Joshua Tan Hon Liang
Student ID:  242DT243LZ
*/

/*Member 4
Student Name:  
Student ID:  242DT2422J
*/

#include<iostream>
#include<ctime>
using namespace std;

struct Patient
{
    int patientNo;
    int patientID;
    int age;
    int emergencyLevel;
    int admissionDay;
};

struct Performance
{
    long comparisons;
    long movements;
    long iterations;
    double executionTime;
};

void resetPerformance(Performance &p)
{
    p.comparisons = 0;
    p.movements = 0;
    p.iterations = 0;
    p.executionTime = 0;
}

Patient createPatient(int number)
{
    Patient p;

    p.patientNo = number;
    p.patientID = 10000 + ((number * 137) % 5000);
    p.age = 1 + ((number * 13) % 90);
    p.emergencyLevel = 1 + ((number * 7) % 5);
    p.admissionDay = 1 + ((number * 11) % 30);

    return p;
}

void generateAverageRecords(Patient record[], int size)
{
    int i;

    for(i = 0; i < size; i++)
    {
        record[i] = createPatient(i + 1);
    }
}

void generateBestRecords(Patient record[], int size)
{
    int i;

    for(i = 0; i < size; i++)
    {
        record[i].patientNo = i + 1;
        record[i].patientID = 10000 + i + 1;
        record[i].age = 1 + (((i + 1) * 13) % 90);
        record[i].emergencyLevel = 1 + (((i + 1) * 7) % 5);
        record[i].admissionDay = 1 + (((i + 1) * 11) % 30);
    }
}

void generateWorstRecords(Patient record[], int size)
{
    int i, value;

    value = size;

    for(i = 0; i < size; i++)
    {
        record[i].patientNo = i + 1;
        record[i].patientID = 10000 + value;
        record[i].age = 1 + ((value * 13) % 90);
        record[i].emergencyLevel = 1 + ((value * 7) % 5);
        record[i].admissionDay = 1 + ((value * 11) % 30);
        value--;
    }
}

void copyRecords(Patient source[], Patient destination[], int size)
{
    int i;

    for(i = 0; i < size; i++)
    {
        destination[i] = source[i];
    }
}

void displayRecords(Patient record[], int size)
{
    int i;

    cout << "No\tPatient ID\tAge\tEmergency Level\tAdmission Day" << endl;
    cout << "------------------------------------------------------------" << endl;

    for(i = 0; i < size; i++)
    {
        cout << record[i].patientNo << "\t"
             << record[i].patientID << "\t\t"
             << record[i].age << "\t"
             << record[i].emergencyLevel << "\t\t"
             << record[i].admissionDay << endl;
    }
}

void displayFirstRecords(Patient record[], int size, int limit)
{
    int i;

    cout << "No\tPatient ID\tAge\tEmergency Level\tAdmission Day" << endl;
    cout << "------------------------------------------------------------" << endl;

    for(i = 0; i < size && i < limit; i++)
    {
        cout << record[i].patientNo << "\t"
             << record[i].patientID << "\t\t"
             << record[i].age << "\t"
             << record[i].emergencyLevel << "\t\t"
             << record[i].admissionDay << endl;
    }
}

void swapPatient(Patient &a, Patient &b, Performance &p)
{
    Patient temp;

    temp = a;
    a = b;
    b = temp;

    p.movements = p.movements + 3;
}

Performance shellSort(Patient record[], int size)
{
    Performance p;
    clock_t startTime, endTime;
    int gap, i, j, stop;
    Patient temp;

    resetPerformance(p);
    startTime = clock();

    gap = size / 2;

    while(gap > 0)
    {
        p.iterations++;

        for(i = gap; i < size; i++)
        {
            p.iterations++;
            temp = record[i];
            p.movements++;
            j = i;
            stop = 0;

            while(j >= gap && stop == 0)
            {
                p.iterations++;
                p.comparisons++;

                if(record[j - gap].patientID > temp.patientID)
                {
                    record[j] = record[j - gap];
                    p.movements++;
                    j = j - gap;
                }
                else
                {
                    stop = 1;
                }
            }

            record[j] = temp;
            p.movements++;
        }

        gap = gap / 2;
    }

    endTime = clock();
    p.executionTime = double(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    return p;
}

Performance combSort(Patient record[], int size)
{
    Performance p;
    clock_t startTime, endTime;
    int gap, i, swapped;

    resetPerformance(p);
    startTime = clock();

    gap = size;
    swapped = 1;

    while(gap != 1 || swapped == 1)
    {
        p.iterations++;
        gap = (gap * 10) / 13;

        if(gap < 1)
            gap = 1;

        swapped = 0;

        for(i = 0; i + gap < size; i++)
        {
            p.iterations++;
            p.comparisons++;

            if(record[i].patientID > record[i + gap].patientID)
            {
                swapPatient(record[i], record[i + gap], p);
                swapped = 1;
            }
        }
    }

    endTime = clock();
    p.executionTime = double(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    return p;
}

void displayPerformance(const char algorithmName[], Performance p)
{
    cout << endl;
    cout << "Algorithm: " << algorithmName << endl;
    cout << "Execution Time: " << p.executionTime << " microseconds" << endl;
    cout << "Comparisons: " << p.comparisons << endl;
    cout << "Movements: " << p.movements << endl;
    cout << "Iterations: " << p.iterations << endl;
}

void compareAlgorithms(Patient original[], int size, const char caseName[])
{
    Patient shellRecord[500];
    Patient combRecord[500];
    Performance shellResult, combResult;

    copyRecords(original, shellRecord, size);
    copyRecords(original, combRecord, size);

    shellResult = shellSort(shellRecord, size);
    combResult = combSort(combRecord, size);

    cout << endl;
    cout << "============================================================" << endl;
    cout << "Performance Comparison - " << caseName << endl;
    cout << "Dataset Size: " << size << " Patient Records" << endl;
    cout << "============================================================" << endl;

    cout << "Algorithm\tTime(us)\tComparisons\tMovements\tIterations" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "Shell Sort\t" << shellResult.executionTime << "\t\t"
         << shellResult.comparisons << "\t\t"
         << shellResult.movements << "\t\t"
         << shellResult.iterations << endl;

    cout << "Comb Sort\t" << combResult.executionTime << "\t\t"
         << combResult.comparisons << "\t\t"
         << combResult.movements << "\t\t"
         << combResult.iterations << endl;

    cout << endl;
    cout << "Ranking Output:" << endl;

    if(shellResult.executionTime < combResult.executionTime)
    {
        cout << "1. Shell Sort" << endl;
        cout << "2. Comb Sort" << endl;
    }
    else if(combResult.executionTime < shellResult.executionTime)
    {
        cout << "1. Comb Sort" << endl;
        cout << "2. Shell Sort" << endl;
    }
    else
    {
        if(shellResult.iterations < combResult.iterations)
        {
            cout << "1. Shell Sort" << endl;
            cout << "2. Comb Sort" << endl;
        }
        else if(combResult.iterations < shellResult.iterations)
        {
            cout << "1. Comb Sort" << endl;
            cout << "2. Shell Sort" << endl;
        }
        else
        {
            cout << "Both algorithms have similar performance." << endl;
        }
    }
}

void bestAverageWorstTest(int size)
{
    Patient record[500];

    generateBestRecords(record, size);
    compareAlgorithms(record, size, "Best Case - Already Sorted Patient ID");

    generateAverageRecords(record, size);
    compareAlgorithms(record, size, "Average Case - Mixed Patient ID");

    generateWorstRecords(record, size);
    compareAlgorithms(record, size, "Worst Case - Reverse Patient ID");
}

void shellSortStepDemo()
{
    Patient record[10];
    Performance p;
    int size, gap, i, j;
    Patient temp;

    size = 10;
    generateAverageRecords(record, size);
    resetPerformance(p);

    cout << endl;
    cout << "Step-by-Step Shell Sort Demonstration" << endl;
    cout << "Before Sorting:" << endl;
    displayFirstRecords(record, size, 10);

    gap = size / 2;

    while(gap > 0)
    {
        cout << endl;
        cout << "Gap = " << gap << endl;

        for(i = gap; i < size; i++)
        {
            temp = record[i];
            j = i;

            while(j >= gap && record[j - gap].patientID > temp.patientID)
            {
                record[j] = record[j - gap];
                j = j - gap;
            }

            record[j] = temp;
        }

        displayFirstRecords(record, size, 10);
        gap = gap / 2;
    }

    cout << endl;
    cout << "Final Sorted Result:" << endl;
    displayFirstRecords(record, size, 10);
}

void combSortStepDemo()
{
    Patient record[10];
    Performance p;
    int size, gap, i, swapped, pass;

    size = 10;
    generateAverageRecords(record, size);
    resetPerformance(p);

    cout << endl;
    cout << "Step-by-Step Comb Sort Demonstration" << endl;
    cout << "Before Sorting:" << endl;
    displayFirstRecords(record, size, 10);

    gap = size;
    swapped = 1;
    pass = 1;

    while(gap != 1 || swapped == 1)
    {
        gap = (gap * 10) / 13;

        if(gap < 1)
            gap = 1;

        swapped = 0;

        for(i = 0; i + gap < size; i++)
        {
            if(record[i].patientID > record[i + gap].patientID)
            {
                swapPatient(record[i], record[i + gap], p);
                swapped = 1;
            }
        }

        cout << endl;
        cout << "Pass " << pass << ", Gap = " << gap << endl;
        displayFirstRecords(record, size, 10);
        pass++;
    }

    cout << endl;
    cout << "Final Sorted Result:" << endl;
    displayFirstRecords(record, size, 10);
}

void categoriseRecords(int size)
{
    Patient record[500];
    int level, i, count;

    generateAverageRecords(record, size);

    cout << endl;
    cout << "Categorised Patient Records by Emergency Level" << endl;
    cout << "Dataset Size: " << size << endl;

    for(level = 1; level <= 5; level++)
    {
        cout << endl;
        cout << "Emergency Level " << level << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << "No\tPatient ID\tAge\tAdmission Day" << endl;

        count = 0;

        for(i = 0; i < size; i++)
        {
            if(record[i].emergencyLevel == level)
            {
                cout << record[i].patientNo << "\t"
                     << record[i].patientID << "\t\t"
                     << record[i].age << "\t"
                     << record[i].admissionDay << endl;
                count++;
            }
        }

        cout << "Total patients in emergency level " << level << ": " << count << endl;
    }
}

void showAlgorithmInformation()
{
    cout << endl;
    cout << "================ Algorithm Information ================" << endl;

    cout << endl;
    cout << "Shell Sort" << endl;
    cout << "Best Case Time Complexity: O(n log n) normally, depends on gap sequence" << endl;
    cout << "Average Case Time Complexity: Around O(n^1.5) for simple gap sequence" << endl;
    cout << "Worst Case Time Complexity: O(n^2)" << endl;
    cout << "Stability: Not stable" << endl;
    cout << "Memory Usage: Low memory usage because it sorts inside the same array" << endl;
    cout << "Implementation Difficulty: Medium" << endl;
    cout << "Suitable Situation: Suitable when the system needs faster sorting than simple insertion sort" << endl;
    cout << "Not Suitable Situation: Not suitable when stable sorting is required" << endl;

    cout << endl;
    cout << "Comb Sort" << endl;
    cout << "Best Case Time Complexity: O(n log n) approximately" << endl;
    cout << "Average Case Time Complexity: O(n^2 / 2^p) approximately" << endl;
    cout << "Worst Case Time Complexity: O(n^2)" << endl;
    cout << "Stability: Not stable" << endl;
    cout << "Memory Usage: Low memory usage because it sorts inside the same array" << endl;
    cout << "Implementation Difficulty: Easy to Medium" << endl;
    cout << "Suitable Situation: Suitable for improving bubble-sort style sorting by removing far-apart disorder first" << endl;
    cout << "Not Suitable Situation: Not suitable when guaranteed O(n log n) performance is required" << endl;
}

void showReportNotes()
{
    cout << endl;
    cout << "================ Report Notes ================" << endl;

    cout << endl;
    cout << "System Title:" << endl;
    cout << "Hospital Patient Records Sorting System" << endl;

    cout << endl;
    cout << "Real-World Problem:" << endl;
    cout << "A hospital stores many patient records. Sorting records by Patient ID can help staff arrange and find records more easily." << endl;

    cout << endl;
    cout << "Selected Algorithms:" << endl;
    cout << "1. Shell Sort" << endl;
    cout << "2. Comb Sort" << endl;

    cout << endl;
    cout << "Why these algorithms are selected:" << endl;
    cout << "Both algorithms are approved advanced sorting algorithms. They are manually implemented in this program without using sort()." << endl;

    cout << endl;
    cout << "Sorting Key:" << endl;
    cout << "Patient ID" << endl;

    cout << endl;
    cout << "Dataset Used:" << endl;
    cout << "100 patient records, 300 patient records, and 500 patient records." << endl;

    cout << endl;
    cout << "Performance Items Measured:" << endl;
    cout << "Execution time, number of comparisons, number of movements, and number of iterations." << endl;

    cout << endl;
    cout << "How data size affects performance:" << endl;
    cout << "When the number of patient records increases, both algorithms need more comparisons, movements, and iterations." << endl;

    cout << endl;
    cout << "Conclusion Guide:" << endl;
    cout << "The better algorithm is decided by comparing execution time and operation counts in the output table." << endl;
}

void sortByShell(int size)
{
    Patient record[500];
    Performance result;

    generateAverageRecords(record, size);
    result = shellSort(record, size);

    cout << endl;
    cout << "Sorted Patient Records by Patient ID using Shell Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Shell Sort", result);
}

void sortByComb(int size)
{
    Patient record[500];
    Performance result;

    generateAverageRecords(record, size);
    result = combSort(record, size);

    cout << endl;
    cout << "Sorted Patient Records by Patient ID using Comb Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Comb Sort", result);
}

void compareByDatasetSize(int size)
{
    Patient record[500];

    generateAverageRecords(record, size);
    compareAlgorithms(record, size, "Average Case - Mixed Patient ID");
}

void menu()
{
    int choice;
    Patient record[500];

    do
    {
        cout << endl;
        cout << "============================================================" << endl;
        cout << " Hospital Patient Records Sorting System" << endl;
        cout << "============================================================" << endl;
        cout << "1. Display 100 Patient Records" << endl;
        cout << "2. Display 500 Patient Records" << endl;
        cout << "3. Sort 100 Records using Shell Sort" << endl;
        cout << "4. Sort 100 Records using Comb Sort" << endl;
        cout << "5. Compare Algorithms using 100 Records" << endl;
        cout << "6. Compare Algorithms using 300 Records" << endl;
        cout << "7. Compare Algorithms using 500 Records" << endl;
        cout << "8. Best, Average, Worst Case Test using 100 Records" << endl;
        cout << "9. Best, Average, Worst Case Test using 300 Records" << endl;
        cout << "10. Best, Average, Worst Case Test using 500 Records" << endl;
        cout << "11. Step-by-Step Shell Sort Demonstration" << endl;
        cout << "12. Step-by-Step Comb Sort Demonstration" << endl;
        cout << "13. Categorise 100 Patient Records by Emergency Level" << endl;
        cout << "14. Categorise 300 Patient Records by Emergency Level" << endl;
        cout << "15. Show Algorithm Information" << endl;
        cout << "16. Show Report Notes" << endl;
        cout << "17. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if(choice == 1)
        {
            generateAverageRecords(record, 100);
            displayRecords(record, 100);
        }
        else if(choice == 2)
        {
            generateAverageRecords(record, 500);
            displayRecords(record, 500);
        }
        else if(choice == 3)
        {
            sortByShell(100);
        }
        else if(choice == 4)
        {
            sortByComb(100);
        }
        else if(choice == 5)
        {
            compareByDatasetSize(100);
        }
        else if(choice == 6)
        {
            compareByDatasetSize(300);
        }
        else if(choice == 7)
        {
            compareByDatasetSize(500);
        }
        else if(choice == 8)
        {
            bestAverageWorstTest(100);
        }
        else if(choice == 9)
        {
            bestAverageWorstTest(300);
        }
        else if(choice == 10)
        {
            bestAverageWorstTest(500);
        }
        else if(choice == 11)
        {
            shellSortStepDemo();
        }
        else if(choice == 12)
        {
            combSortStepDemo();
        }
        else if(choice == 13)
        {
            categoriseRecords(100);
        }
        else if(choice == 14)
        {
            categoriseRecords(300);
        }
        else if(choice == 15)
        {
            showAlgorithmInformation();
        }
        else if(choice == 16)
        {
            showReportNotes();
        }
        else if(choice == 17)
        {
            cout << "Thank you for using the system." << endl;
        }
        else
        {
            cout << "Invalid choice. Please try again." << endl;
        }

    } while(choice != 17);
}

int main()
{
    menu();
    return 0;
}
