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

// ============================================================
// MAX RECORD SIZE
// ============================================================
const int MAX_SIZE = 300;

// ============================================================
// STRUCTS
// ============================================================

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

// ============================================================
// PERFORMANCE UTILITIES
// ============================================================

void resetPerformance(Performance &p)
{
    p.comparisons = 0;
    p.movements = 0;
    p.iterations = 0;
    p.executionTime = 0;
}

// ============================================================
// RECORD GENERATION
// ============================================================

Patient createPatient(int number)
{
    Patient p;
    p.patientNo      = number;
    p.patientID      = 10000 + ((number * 137) % 5000);
    p.age            = 1 + ((number * 13) % 90);
    p.emergencyLevel = 1 + ((number * 7) % 5);
    p.admissionDay   = 1 + ((number * 11) % 30);
    return p;
}

void generateAverageRecords(Patient record[], int size)
{
    int i;
    for(i = 0; i < size; i++)
        record[i] = createPatient(i + 1);
}

void generateBestRecords(Patient record[], int size)
{
    // Best case: already sorted by patientID ascending
    int i;
    for(i = 0; i < size; i++)
    {
        record[i].patientNo      = i + 1;
        record[i].patientID      = 10000 + (i + 1);   // strictly increasing
        record[i].age            = 1 + (((i + 1) * 13) % 90);
        record[i].emergencyLevel = 1 + (((i + 1) * 7) % 5);
        record[i].admissionDay   = 1 + (((i + 1) * 11) % 30);
    }
}

void generateWorstRecords(Patient record[], int size)
{
    // Worst case: reverse sorted by patientID
    int i, value;
    value = size;
    for(i = 0; i < size; i++)
    {
        record[i].patientNo      = i + 1;
        record[i].patientID      = 10000 + value;
        record[i].age            = 1 + ((value * 13) % 90);
        record[i].emergencyLevel = 1 + ((value * 7) % 5);
        record[i].admissionDay   = 1 + ((value * 11) % 30);
        value--;
    }
}

void copyRecords(Patient source[], Patient destination[], int size)
{
    int i;
    for(i = 0; i < size; i++)
        destination[i] = source[i];
}

// ============================================================
// DISPLAY UTILITIES
// ============================================================

void displayRecords(Patient record[], int size)
{
    int i;
    cout << "No\tPatient ID\tAge\tEmergency Level\tAdmission Day" << endl;
    cout << "------------------------------------------------------------" << endl;
    for(i = 0; i < size; i++)
    {
        cout << record[i].patientNo    << "\t"
             << record[i].patientID    << "\t\t"
             << record[i].age          << "\t"
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
        cout << record[i].patientNo    << "\t"
             << record[i].patientID    << "\t\t"
             << record[i].age          << "\t"
             << record[i].emergencyLevel << "\t\t"
             << record[i].admissionDay << endl;
    }
}

void displayPerformance(const char algorithmName[], Performance p)
{
    cout << endl;
    cout << "Algorithm: " << algorithmName << endl;
    cout << "Execution Time : " << p.executionTime << " microseconds" << endl;
    cout << "Comparisons    : " << p.comparisons   << endl;
    cout << "Movements      : " << p.movements     << endl;
    cout << "Iterations     : " << p.iterations    << endl;
}

// ============================================================
// SWAP HELPER
// ============================================================

void swapPatient(Patient &a, Patient &b, Performance &p)
{
    Patient temp;
    temp = a;
    a    = b;
    b    = temp;
    p.movements += 3;
}

// ============================================================
// HEAP SORT  (sorts by patientID ascending)
// ============================================================

// Heapify subtree rooted at index i, heap size n
void heapify(Patient record[], int n, int i, Performance &p)
{
    int largest, left, right;

    largest = i;
    left    = 2 * i + 1;
    right   = 2 * i + 2;

    p.iterations++;

    if(left < n)
    {
        p.comparisons++;
        if(record[left].patientID > record[largest].patientID)
            largest = left;
    }

    if(right < n)
    {
        p.comparisons++;
        if(record[right].patientID > record[largest].patientID)
            largest = right;
    }

    if(largest != i)
    {
        swapPatient(record[i], record[largest], p);
        heapify(record, n, largest, p);
    }
}

Performance heapSort(Patient record[], int size)
{
    Performance p;
    clock_t startTime, endTime;
    int i;

    resetPerformance(p);
    startTime = clock();

    // Build max-heap
    for(i = size / 2 - 1; i >= 0; i--)
    {
        p.iterations++;
        heapify(record, size, i, p);
    }

    // Extract elements from heap one by one
    for(i = size - 1; i > 0; i--)
    {
        p.iterations++;
        swapPatient(record[0], record[i], p);   // move current root to end
        heapify(record, i, 0, p);               // heapify reduced heap
    }

    endTime = clock();
    p.executionTime = double(endTime - startTime) * 1000000.0 / CLOCKS_PER_SEC;

    return p;
}

// ============================================================
// RADIX SORT  (LSD, base-10, sorts by patientID ascending)
// ============================================================

int getMax(Patient record[], int size)
{
    int maxVal, i;
    maxVal = record[0].patientID;
    for(i = 1; i < size; i++)
    {
        if(record[i].patientID > maxVal)
            maxVal = record[i].patientID;
    }
    return maxVal;
}

// One pass of counting sort based on digit at position exp (1, 10, 100, ...)
void countingSortByDigit(Patient record[], int size, int exp, Performance &p)
{
    Patient output[MAX_SIZE];
    int count[10];
    int i, digit;

    // Initialise count array
    for(i = 0; i < 10; i++)
        count[i] = 0;

    // Count occurrences of each digit
    for(i = 0; i < size; i++)
    {
        p.iterations++;
        digit = (record[i].patientID / exp) % 10;
        count[digit]++;
    }

    // Change count[i] to contain actual position
    for(i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build output array (traverse from right for stability)
    for(i = size - 1; i >= 0; i--)
    {
        p.iterations++;
        digit = (record[i].patientID / exp) % 10;
        output[count[digit] - 1] = record[i];
        count[digit]--;
        p.movements++;
    }

    // Copy output back to record
    for(i = 0; i < size; i++)
    {
        p.comparisons++;   // position comparison
        record[i] = output[i];
        p.movements++;
    }
}

Performance radixSort(Patient record[], int size)
{
    Performance p;
    clock_t startTime, endTime;
    int maxVal, exp;

    resetPerformance(p);
    startTime = clock();

    maxVal = getMax(record, size);

    // Do counting sort for every digit
    for(exp = 1; maxVal / exp > 0; exp *= 10)
    {
        p.iterations++;
        countingSortByDigit(record, size, exp, p);
    }

    endTime = clock();
    p.executionTime = double(endTime - startTime) * 1000000.0 / CLOCKS_PER_SEC;

    return p;
}

// ============================================================
// ALGORITHM COMPARISON
// ============================================================

void compareAlgorithms(Patient original[], int size, const char caseName[])
{
    Patient heapRecord[MAX_SIZE];
    Patient radixRecord[MAX_SIZE];
    Performance heapResult, radixResult;

    copyRecords(original, heapRecord,  size);
    copyRecords(original, radixRecord, size);

    heapResult  = heapSort (heapRecord,  size);
    radixResult = radixSort(radixRecord, size);

    cout << endl;
    cout << "============================================================" << endl;
    cout << "Performance Comparison - " << caseName << endl;
    cout << "Dataset Size: " << size << " Patient Records" << endl;
    cout << "============================================================" << endl;

    cout << "Algorithm\tTime(us)\tComparisons\tMovements\tIterations" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "Heap Sort\t" << heapResult.executionTime  << "\t\t"
         << heapResult.comparisons   << "\t\t"
         << heapResult.movements     << "\t\t"
         << heapResult.iterations    << endl;

    cout << "Radix Sort\t" << radixResult.executionTime << "\t\t"
         << radixResult.comparisons  << "\t\t"
         << radixResult.movements    << "\t\t"
         << radixResult.iterations   << endl;

    cout << endl;
    cout << "Ranking Output:" << endl;

    if(heapResult.executionTime < radixResult.executionTime)
    {
        cout << "1. Heap Sort"  << endl;
        cout << "2. Radix Sort" << endl;
    }
    else if(radixResult.executionTime < heapResult.executionTime)
    {
        cout << "1. Radix Sort" << endl;
        cout << "2. Heap Sort"  << endl;
    }
    else
    {
        if(heapResult.iterations < radixResult.iterations)
        {
            cout << "1. Heap Sort"  << endl;
            cout << "2. Radix Sort" << endl;
        }
        else if(radixResult.iterations < heapResult.iterations)
        {
            cout << "1. Radix Sort" << endl;
            cout << "2. Heap Sort"  << endl;
        }
        else
        {
            cout << "Both algorithms have similar performance." << endl;
        }
    }
}

// ============================================================
// BEST / AVERAGE / WORST CASE TEST
// ============================================================

void bestAverageWorstTest(int size)
{
    Patient record[MAX_SIZE];

    generateBestRecords(record, size);
    compareAlgorithms(record, size, "Best Case - Already Sorted Patient ID");

    generateAverageRecords(record, size);
    compareAlgorithms(record, size, "Average Case - Mixed Patient ID");

    generateWorstRecords(record, size);
    compareAlgorithms(record, size, "Worst Case - Reverse Sorted Patient ID");
}

// ============================================================
// STEP-BY-STEP DEMONSTRATIONS
// ============================================================

void heapSortStepDemo()
{
    Patient record[10];
    Performance p;
    int size, i, n;

    size = 10;
    generateAverageRecords(record, size);
    resetPerformance(p);

    cout << endl;
    cout << "Step-by-Step Heap Sort Demonstration (10 records)" << endl;
    cout << "Before Sorting:" << endl;
    displayFirstRecords(record, size, 10);

    // Build max-heap and show
    cout << endl;
    cout << "--- Phase 1: Build Max-Heap ---" << endl;
    for(i = size / 2 - 1; i >= 0; i--)
        heapify(record, size, i, p);
    displayFirstRecords(record, size, 10);

    // Extract elements one by one and show each step
    cout << endl;
    cout << "--- Phase 2: Extract Elements ---" << endl;
    for(n = size - 1; n > 0; n--)
    {
        swapPatient(record[0], record[n], p);
        heapify(record, n, 0, p);
        cout << "After extracting max (heap size = " << n << "):" << endl;
        displayFirstRecords(record, size, 10);
    }

    cout << endl;
    cout << "Final Sorted Result:" << endl;
    displayFirstRecords(record, size, 10);
}

void radixSortStepDemo()
{
    Patient record[10];
    Performance p;
    int size, maxVal, exp, pass;

    size = 10;
    generateAverageRecords(record, size);
    resetPerformance(p);

    cout << endl;
    cout << "Step-by-Step Radix Sort Demonstration (10 records)" << endl;
    cout << "Before Sorting:" << endl;
    displayFirstRecords(record, size, 10);

    maxVal = getMax(record, size);
    pass   = 1;

    for(exp = 1; maxVal / exp > 0; exp *= 10)
    {
        countingSortByDigit(record, size, exp, p);
        cout << endl;
        cout << "Pass " << pass << " (sorted by digit at position " << exp << "):" << endl;
        displayFirstRecords(record, size, 10);
        pass++;
    }

    cout << endl;
    cout << "Final Sorted Result:" << endl;
    displayFirstRecords(record, size, 10);
}

// ============================================================
// CATEGORISE RECORDS BY EMERGENCY LEVEL
// ============================================================

void categoriseRecords(int size)
{
    Patient record[MAX_SIZE];
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
                     << record[i].age       << "\t"
                     << record[i].admissionDay << endl;
                count++;
            }
        }

        cout << "Total patients in emergency level " << level << ": " << count << endl;
    }
}

// ============================================================
// SORT AND DISPLAY HELPERS
// ============================================================

void sortByHeap(int size)
{
    Patient record[MAX_SIZE];
    Performance result;

    generateAverageRecords(record, size);
    result = heapSort(record, size);

    cout << endl;
    cout << "Sorted Patient Records by Patient ID using Heap Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Heap Sort", result);
}

void sortByRadix(int size)
{
    Patient record[MAX_SIZE];
    Performance result;

    generateAverageRecords(record, size);
    result = radixSort(record, size);

    cout << endl;
    cout << "Sorted Patient Records by Patient ID using Radix Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Radix Sort", result);
}

void compareByDatasetSize(int size)
{
    Patient record[MAX_SIZE];
    generateAverageRecords(record, size);
    compareAlgorithms(record, size, "Average Case - Mixed Patient ID");
}

// ============================================================
// ALGORITHM INFORMATION
// ============================================================

void showAlgorithmInformation()
{
    cout << endl;
    cout << "================ Algorithm Information ================" << endl;

    cout << endl;
    cout << "Heap Sort" << endl;
    cout << "Best Case Time Complexity    : O(n log n)" << endl;
    cout << "Average Case Time Complexity : O(n log n)" << endl;
    cout << "Worst Case Time Complexity   : O(n log n)" << endl;
    cout << "Stability                    : Not stable" << endl;
    cout << "Memory Usage                 : In-place, O(1) extra space" << endl;
    cout << "Implementation Difficulty    : Medium" << endl;
    cout << "Suitable Situation           : Guaranteed O(n log n) performance needed" << endl;
    cout << "Not Suitable Situation       : When stable sort is required" << endl;

    cout << endl;
    cout << "Radix Sort" << endl;
    cout << "Best Case Time Complexity    : O(nk)  where k = number of digits" << endl;
    cout << "Average Case Time Complexity : O(nk)" << endl;
    cout << "Worst Case Time Complexity   : O(nk)" << endl;
    cout << "Stability                    : Stable (LSD implementation)" << endl;
    cout << "Memory Usage                 : O(n + b) extra space (b = base, 10 here)" << endl;
    cout << "Implementation Difficulty    : Medium" << endl;
    cout << "Suitable Situation           : Large datasets with integer keys of fixed digit count" << endl;
    cout << "Not Suitable Situation       : When keys have very large range or are non-integer" << endl;
}

// ============================================================
// REPORT NOTES
// ============================================================

void showReportNotes()
{
    cout << endl;
    cout << "================ Report Notes ================" << endl;

    cout << endl;
    cout << "System Title:" << endl;
    cout << "Hospital Patient Records Sorting System" << endl;

    cout << endl;
    cout << "Real-World Problem:" << endl;
    cout << "A hospital stores many patient records. Sorting by Patient ID helps staff find and manage records efficiently." << endl;

    cout << endl;
    cout << "Selected Algorithms:" << endl;
    cout << "1. Heap Sort" << endl;
    cout << "2. Radix Sort" << endl;

    cout << endl;
    cout << "Why these algorithms were selected:" << endl;
    cout << "Both are advanced sorting algorithms from the approved list." << endl;
    cout << "They are manually implemented without using sort() or any STL algorithm." << endl;

    cout << endl;
    cout << "Sorting Key:" << endl;
    cout << "Patient ID" << endl;

    cout << endl;
    cout << "Datasets Used:" << endl;
    cout << "100 patient records and 300 patient records." << endl;

    cout << endl;
    cout << "Performance Items Measured:" << endl;
    cout << "Execution time, number of comparisons, number of movements, and number of iterations." << endl;

    cout << endl;
    cout << "How data size affects performance:" << endl;
    cout << "Heap Sort: increases as O(n log n) - moderate growth." << endl;
    cout << "Radix Sort: increases as O(nk) - near-linear growth when digit count k is small." << endl;

    cout << endl;
    cout << "Conclusion Guide:" << endl;
    cout << "Compare execution time and operation counts from the output table to decide which is better for each case." << endl;
}

// ============================================================
// MAIN MENU
// ============================================================

void menu()
{
    int choice;
    Patient record[MAX_SIZE];

    do
    {
        cout << endl;
        cout << "============================================================" << endl;
        cout << " Hospital Patient Records Sorting System" << endl;
        cout << "============================================================" << endl;
        cout << "1.  Display 100 Patient Records" << endl;
        cout << "2.  Display 300 Patient Records" << endl;
        cout << "3.  Sort 100 Records using Heap Sort" << endl;
        cout << "4.  Sort 100 Records using Radix Sort" << endl;
        cout << "5.  Compare Algorithms using 100 Records" << endl;
        cout << "6.  Compare Algorithms using 300 Records" << endl;
        cout << "7.  Best, Average, Worst Case Test using 100 Records" << endl;
        cout << "8.  Best, Average, Worst Case Test using 300 Records" << endl;
        cout << "9.  Step-by-Step Heap Sort Demonstration" << endl;
        cout << "10. Step-by-Step Radix Sort Demonstration" << endl;
        cout << "11. Categorise 100 Patient Records by Emergency Level" << endl;
        cout << "12. Categorise 300 Patient Records by Emergency Level" << endl;
        cout << "13. Show Algorithm Information" << endl;
        cout << "14. Show Report Notes" << endl;
        cout << "15. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if(choice == 1)
        {
            generateAverageRecords(record, 100);
            displayRecords(record, 100);
        }
        else if(choice == 2)
        {
            generateAverageRecords(record, 300);
            displayRecords(record, 300);
        }
        else if(choice == 3)
        {
            sortByHeap(100);
        }
        else if(choice == 4)
        {
            sortByRadix(100);
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
            bestAverageWorstTest(100);
        }
        else if(choice == 8)
        {
            bestAverageWorstTest(300);
        }
        else if(choice == 9)
        {
            heapSortStepDemo();
        }
        else if(choice == 10)
        {
            radixSortStepDemo();
        }
        else if(choice == 11)
        {
            categoriseRecords(100);
        }
        else if(choice == 12)
        {
            categoriseRecords(300);
        }
        else if(choice == 13)
        {
            showAlgorithmInformation();
        }
        else if(choice == 14)
        {
            showReportNotes();
        }
        else if(choice == 15)
        {
            cout << "Thank you for using the system." << endl;
        }
        else
        {
            cout << "Invalid choice. Please try again." << endl;
        }

    } while(choice != 15);
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    menu();
    return 0;
}