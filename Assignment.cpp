/*
 * ============================================================
 *  Hospital Patient Records Sorting System
 *  TDS4223 Data Structure and Algorithms 
 * ============================================================
 *  Algorithms: Heap Sort  vs  Tim Sort
 *  Sorting Key: Patient ID
 *  Datasets: 100 records and 500 records
 * ============================================================
 */

/*Member 1
Student Name:  Terrence Lim Jun Jay
Student ID:    242DT243F0
*/

/*Member 2
Student Name:  Yong Zong Yan
Student ID:    242DT2422R
*/

/*Member 3
Student Name:  Joshua Tan Hon Liang
Student ID:    242DT243LZ
*/

/*Member 4
Student Name:
Student ID:    242DT2422J
*/

#include <iostream>
#include <ctime>
using namespace std;


const int MAX_SIZE   = 500;
const int SMALL_SIZE = 100;
const int LARGE_SIZE = 500;
const int RUN        = 32;   


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
    p.comparisons   = 0;
    p.movements     = 0;
    p.iterations    = 0;
    p.executionTime = 0.0;
}


Patient createPatient(int number)
{
    Patient p;
    p.patientNo      = number;
    p.patientID      = 10000 + ((number * 137) % 5000);
    p.age            = 1 + ((number * 13) % 90);
    p.emergencyLevel = 1 + ((number * 7)  % 5);
    p.admissionDay   = 1 + ((number * 11) % 30);
    return p;
}

/* Average case: pseudo-random order */
void generateAverageRecords(Patient record[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        record[i] = createPatient(i + 1);
}

/* Best case: already sorted ascending by patientID */
void generateBestRecords(Patient record[], int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        record[i].patientNo      = i + 1;
        record[i].patientID      = 10000 + (i + 1);
        record[i].age            = 1 + (((i + 1) * 13) % 90);
        record[i].emergencyLevel = 1 + (((i + 1) * 7)  % 5);
        record[i].admissionDay   = 1 + (((i + 1) * 11) % 30);
    }
}

/* Worst case: reverse sorted by patientID */
void generateWorstRecords(Patient record[], int size)
{
    int i, value;
    value = size;
    for (i = 0; i < size; i++)
    {
        record[i].patientNo      = i + 1;
        record[i].patientID      = 10000 + value;
        record[i].age            = 1 + ((value * 13) % 90);
        record[i].emergencyLevel = 1 + ((value * 7)  % 5);
        record[i].admissionDay   = 1 + ((value * 11) % 30);
        value--;
    }
}

void copyRecords(Patient source[], Patient dest[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        dest[i] = source[i];
}


void displayHeader()
{
    cout << "No\tPatient ID\tAge\tEmergency Level\tAdmission Day" << endl;
    cout << "------------------------------------------------------------" << endl;
}

void displayRow(Patient &r)
{
    cout << r.patientNo      << "\t"
         << r.patientID      << "\t\t"
         << r.age            << "\t"
         << r.emergencyLevel << "\t\t"
         << r.admissionDay   << endl;
}

void displayRecords(Patient record[], int size)
{
    int i;
    displayHeader();
    for (i = 0; i < size; i++)
        displayRow(record[i]);
}

void displayFirstRecords(Patient record[], int size, int limit)
{
    int i;
    displayHeader();
    for (i = 0; i < size && i < limit; i++)
        displayRow(record[i]);
}

void displayPerformance(const char algorithmName[], Performance p)
{
    cout << endl;
    cout << "Algorithm      : " << algorithmName   << endl;
    cout << "Execution Time : " << p.executionTime << " microseconds" << endl;
    cout << "Comparisons    : " << p.comparisons   << endl;
    cout << "Movements      : " << p.movements     << endl;
    cout << "Iterations     : " << p.iterations    << endl;
}


void swapPatient(Patient &a, Patient &b, Performance &p)
{
    Patient temp;
    temp = a;
    a    = b;
    b    = temp;
    p.movements += 3;
}


//  Time Complexity  : O(n log n) — best, average, and worst
//  Space Complexity : O(1) auxiliary (in-place)

void heapify(Patient record[], int n, int i, Performance &p)
{
    int largest, left, right;

    largest = i;
    left    = 2 * i + 1;
    right   = 2 * i + 2;

    p.iterations++;

    if (left < n)
    {
        p.comparisons++;
        if (record[left].patientID > record[largest].patientID)
            largest = left;
    }

    if (right < n)
    {
        p.comparisons++;
        if (record[right].patientID > record[largest].patientID)
            largest = right;
    }

    if (largest != i)
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

    /* Phase 1: Build max-heap */
    for (i = size / 2 - 1; i >= 0; i--)
    {
        p.iterations++;
        heapify(record, size, i, p);
    }

    /* Phase 2: Extract elements from heap one by one */
    for (i = size - 1; i > 0; i--)
    {
        p.iterations++;
        swapPatient(record[0], record[i], p);
        heapify(record, i, 0, p);
    }

    endTime = clock();
    p.executionTime = (double)(endTime - startTime) * 1000000.0 / CLOCKS_PER_SEC;

    return p;
}

// ============================================================
// TIM SORT  (sorts by patientID ascending)
// ============================================================
//  Time Complexity  : O(n)       best case  (already sorted)
//                     O(n log n) average case
//                     O(n log n) worst case
//  Space Complexity : O(n) auxiliary (merge buffer)


/* Insertion sort a sub-array from left to right (inclusive) */
void insertionSortRun(Patient record[], int left, int right, Performance &p)
{
    int i, j;
    Patient temp;

    for (i = left + 1; i <= right; i++)
    {
        p.iterations++;
        temp = record[i];
        p.movements++;

        j = i - 1;
        while (j >= left)
        {
            p.comparisons++;
            if (record[j].patientID > temp.patientID)
            {
                record[j + 1] = record[j];
                p.movements++;
                j--;
            }
            else
                break;
        }
        record[j + 1] = temp;
        p.movements++;
    }
}

/* Merge two sorted sub-arrays: [left..mid] and [mid+1..right] */
void mergeRuns(Patient record[], int left, int mid, int right,
               Patient buffer[], Performance &p)
{
    int i, j, k;
    int len1;

    len1 = mid - left + 1;

    /* Copy left half into buffer */
    for (i = 0; i < len1; i++)
    {
        buffer[i] = record[left + i];
        p.movements++;
    }

    i = 0;
    j = mid + 1;
    k = left;

    while (i < len1 && j <= right)
    {
        p.iterations++;
        p.comparisons++;
        if (buffer[i].patientID <= record[j].patientID)
        {
            record[k] = buffer[i];
            i++;
        }
        else
        {
            record[k] = record[j];
            j++;
        }
        p.movements++;
        k++;
    }

    while (i < len1)
    {
        record[k] = buffer[i];
        p.movements++;
        i++;
        k++;
    }
    /* remaining record[j..right] already in place */
}

Performance timSort(Patient record[], int size)
{
    Performance p;
    clock_t startTime, endTime;
    Patient buffer[MAX_SIZE];
    int i, left, mid, right, mergeSize;

    resetPerformance(p);
    startTime = clock();

    /* Step 1: Sort individual runs using Insertion Sort */
    for (i = 0; i < size; i += RUN)
    {
        p.iterations++;
        left  = i;
        right = i + RUN - 1;
        if (right >= size)
            right = size - 1;
        insertionSortRun(record, left, right, p);
    }

    /* Step 2: Merge runs bottom-up */
    for (mergeSize = RUN; mergeSize < size; mergeSize *= 2)
    {
        p.iterations++;
        for (left = 0; left < size; left += 2 * mergeSize)
        {
            p.iterations++;
            mid   = left + mergeSize - 1;
            right = left + 2 * mergeSize - 1;

            if (mid >= size - 1)
                break;                      // only one run, nothing to merge
            if (right >= size)
                right = size - 1;

            mergeRuns(record, left, mid, right, buffer, p);
        }
    }

    endTime = clock();
    p.executionTime = (double)(endTime - startTime) * 1000000.0 / CLOCKS_PER_SEC;

    return p;
}


void categoriseRecords(int size)
{
    Patient record[MAX_SIZE];
    int level, i, count;

    generateAverageRecords(record, size);

    cout << endl;
    cout << "Categorised Patient Records by Emergency Level" << endl;
    cout << "Dataset Size: " << size << endl;

    for (level = 1; level <= 5; level++)
    {
        cout << endl;
        cout << "Emergency Level " << level << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << "No\tPatient ID\tAge\tAdmission Day" << endl;

        count = 0;

        for (i = 0; i < size; i++)
        {
            if (record[i].emergencyLevel == level)
            {
                cout << record[i].patientNo   << "\t"
                     << record[i].patientID   << "\t\t"
                     << record[i].age         << "\t"
                     << record[i].admissionDay << endl;
                count++;
            }
        }

        cout << "Total patients in emergency level " << level << ": " << count << endl;
    }
}



void sortByHeap(int size)
{
    Patient record[MAX_SIZE];
    Performance result;

    generateAverageRecords(record, size);
    result = heapSort(record, size);

    cout << endl;
    cout << "Sorted " << size << " Patient Records by Patient ID using Heap Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Heap Sort", result);
}

void sortByTim(int size)
{
    Patient record[MAX_SIZE];
    Performance result;

    generateAverageRecords(record, size);
    result = timSort(record, size);

    cout << endl;
    cout << "Sorted " << size << " Patient Records by Patient ID using Tim Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Tim Sort", result);
}

void menu()
{
    int choice;
    Patient record[MAX_SIZE];

    do
    {
        cout << endl;
        cout << "============================================================" << endl;
        cout << " Hospital Patient Records Sorting System" << endl;
        cout << " Algorithms: Heap Sort  vs  Tim Sort" << endl;
        cout << "============================================================" << endl;
        cout << "1.  Display 100 Patient Records" << endl;
        cout << "2.  Display 500 Patient Records" << endl;
        cout << "3.  Sort 100 Records using Heap Sort" << endl;
        cout << "4.  Sort 100 Records using Tim Sort" << endl;
        cout << "5.  Sort 500 Records using Heap Sort" << endl;
        cout << "6.  Sort 500 Records using Tim Sort" << endl;
        cout << "7.  Categorise 100 Patient Records by Emergency Level" << endl;
        cout << "8.  Categorise 500 Patient Records by Emergency Level" << endl;
        cout << "9.  Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            generateAverageRecords(record, SMALL_SIZE);
            displayRecords(record, SMALL_SIZE);
        }
        else if (choice == 2)
        {
            generateAverageRecords(record, LARGE_SIZE);
            displayRecords(record, LARGE_SIZE);
        }
        else if (choice == 3)
        {
            sortByHeap(SMALL_SIZE);
        }
        else if (choice == 4)
        {
            sortByTim(SMALL_SIZE);
        }
        else if (choice == 5)
        {
            sortByHeap(LARGE_SIZE);
        }
        else if (choice == 6)
        {
            sortByTim(LARGE_SIZE);
        }
        else if (choice == 7)
        {
            categoriseRecords(SMALL_SIZE);
        }
        else if (choice == 8)
        {
            categoriseRecords(LARGE_SIZE);
        }
        else if (choice == 9)
        {
            cout << "Thank you for using the Hospital Patient Records Sorting System." << endl;
        }
        else
        {
            cout << "Invalid choice. Please enter a number between 1 and 9." << endl;
        }

    } while (choice != 9);
}


int main()
{
    menu();
    return 0;
}