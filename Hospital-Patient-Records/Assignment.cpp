/*
 * Hospital Patient Records Sorting System
 * Subject: TDS4223 Data Structure and Algorithms
 * Algorithms: Heap Sort vs Tim Sort
 * Sorting Key: Patient ID
 * Datasets: 100 records and 500 records
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
Student Name:  TEE QI SHENG
Student ID:    242DT2432Q
*/

/*Member 5
Student Name:  CHEW JIN HONG
Student ID:    242DT241PP
*/


#include <iostream>
#include <ctime>
using namespace std;

// max number of records we support
const int MAX_SIZE   = 500;
const int SMALL_SIZE = 100;
const int LARGE_SIZE = 500;
const int RUN        = 32;

// patient struct to store all patient info
struct Patient
{
    int patientNo;
    int patientID;
    int age;
    int emergencyLevel;
    int admissionDay;
};

// we use this struct to track how well each algorithm performs
struct Performance
{
    long comparisons;
    long movements;
    long iterations;
    double executionTime;
};

// reset all performance values to zero before we start sorting
void resetPerformance(Performance &p)
{
    p.comparisons   = 0;
    p.movements     = 0;
    p.iterations    = 0;
    p.executionTime = 0.0;
}

// create one patient using a number, values are generated using simple math
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

// average case: data is in mixed order
void generateAverageRecords(Patient record[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        record[i] = createPatient(i + 1);
}

// best case: data is already sorted perfectly (ascending)
void generateBestRecords(Patient record[], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		record[i] = createPatient(i+1);
		record[i].patientID = 10000 + i;
	}
}

// worst case: data is reverse sorted (descending)
void generateWorstRecords(Patient record[], int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		record[i] = createPatient(i+1);
		record[i].patientID = 10000 + (size - i);
	}
}

// print the table header
void displayHeader()
{
    cout << "No\tPatient ID\tAge\tEmergency Level\tAdmission Day" << endl;
    cout << "------------------------------------------------------------" << endl;
}

// print one row of patient data
void displayRow(Patient &r)
{
    cout << r.patientNo      << "\t"
         << r.patientID      << "\t\t"
         << r.age            << "\t"
         << r.emergencyLevel << "\t\t"
         << r.admissionDay   << endl;
}

// print all records
void displayRecords(Patient record[], int size)
{
    int i;
    displayHeader();
    for (i = 0; i < size; i++)
        displayRow(record[i]);
}

// print the performance result after sorting
void displayPerformance(const char algorithmName[], Performance p)
{
    cout << endl;
    cout << "Algorithm      : " << algorithmName   << endl;
    cout << "Execution Time : " << p.executionTime << " microseconds" << endl;
    cout << "Comparisons    : " << p.comparisons   << endl;
    cout << "Movements      : " << p.movements     << endl;
    cout << "Iterations     : " << p.iterations    << endl;
}

// swap two patient records, count as 3 movements
void swapPatient(Patient &a, Patient &b, Performance &p)
{
    Patient temp;
    temp = a;
    a    = b;
    b    = temp;
    p.movements += 3;
}

// ============================================================
// HEAP SORT
// ============================================================
// Heap Sort builds a max-heap first so the biggest value is at top.
// Then we swap the top to the end and fix the heap again.
// We repeat until all elements are sorted.
// Time Complexity: O(n log n) for best, average and worst case.
// Space: O(1) in-place, no extra memory needed.

// heapify makes sure the parent is bigger than its children.
// if not, swap and call heapify again on that position.
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

    // phase 1: build max-heap from the middle going left
    for (i = size / 2 - 1; i >= 0; i--)
    {
        p.iterations++;
        heapify(record, size, i, p);
    }

    // phase 2: move root (biggest) to end, then fix heap
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
// TIM SORT
// ============================================================
// Tim Sort is a mix of Insertion Sort and Merge Sort.
// First we split the array into small runs of size 32.
// Each run is sorted using Insertion Sort because it is fast for small data.
// Then we merge all sorted runs using Merge Sort.
// It is very fast when data is already partly sorted.
// Time Complexity: O(n) best case, O(n log n) average and worst case.
// Space: O(n) extra memory needed for merge buffer.


// insertion sort one run from index left to right
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
        // shift bigger elements to the right to make space for temp
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

// merge two sorted runs back into the array
void mergeRuns(Patient record[], int left, int mid, int right,
               Patient buffer[], Performance &p)
{
    int i, j, k;
    int len1;

    len1 = mid - left + 1;

    // copy left run into buffer first
    for (i = 0; i < len1; i++)
    {
        buffer[i] = record[left + i];
        p.movements++;
    }

    i = 0;
    j = mid + 1;
    k = left;

    // compare buffer and right run, put the smaller one back
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

    // copy any leftover from buffer
    while (i < len1)
    {
        record[k] = buffer[i];
        p.movements++;
        i++;
        k++;
    }
}

Performance timSort(Patient record[], int size)
{
    Performance p;
    clock_t startTime, endTime;
    Patient buffer[MAX_SIZE];
    int i, left, mid, right, mergeSize;

    resetPerformance(p);
    startTime = clock();

    // step 1: sort each run of 32 elements using insertion sort
    for (i = 0; i < size; i += RUN)
    {
        p.iterations++;
        left  = i;
        right = i + RUN - 1;
        if (right >= size)
            right = size - 1;
        insertionSortRun(record, left, right, p);
    }

    // step 2: merge sorted runs, doubling size each time: 32, 64, 128...
    for (mergeSize = RUN; mergeSize < size; mergeSize *= 2)
    {
        p.iterations++;
        for (left = 0; left < size; left += 2 * mergeSize)
        {
            p.iterations++;
            mid   = left + mergeSize - 1;
            right = left + 2 * mergeSize - 1;

            if (mid >= size - 1)
                break;
            if (right >= size)
                right = size - 1;

            mergeRuns(record, left, mid, right, buffer, p);
        }
    }

    endTime = clock();
    p.executionTime = (double)(endTime - startTime) * 1000000.0 / CLOCKS_PER_SEC;

    return p;
}

// group patients by emergency level and display each group
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

// sort and display records using heap sort
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

// sort and display records using tim sort
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

void sortBestCase(int size)
{
    Patient record[MAX_SIZE];
    Performance result;

    generateBestRecords(record, size);
    result = heapSort(record, size);
    cout << "\nBest Case - Sorted " << size << " Records using Heap Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Heap Sort", result);

    generateBestRecords(record, size);
    result = timSort(record, size);
    cout << "\nBest Case - Sorted " << size << " Records using Tim Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Tim Sort", result);
}

void sortWorstCase(int size)
{
    Patient record[MAX_SIZE];
    Performance result;

    generateWorstRecords(record, size);
    result = heapSort(record, size);
    cout << "\nWorst Case - Sorted " << size << " Records using Heap Sort" << endl;
    displayRecords(record, size);
    displayPerformance("Heap Sort", result);

    generateWorstRecords(record, size);
    result = timSort(record, size);
    cout << "\nWorst Case - Sorted " << size << " Records using Tim Sort" << endl;
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
        cout << "9.  Sort 100 Records - Best Case (Heap Sort vs Tim Sort)" << endl;
		cout << "10. Sort 500 Records - Best Case (Heap Sort vs Tim Sort)" << endl;
		cout << "11. Sort 100 Records - Worst Case (Heap Sort vs Tim Sort)" << endl;
		cout << "12. Sort 500 Records - Worst Case (Heap Sort vs Tim Sort)" << endl;
		cout << "13. Exit" << endl;
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
		    sortBestCase(SMALL_SIZE);
		}
		else if (choice == 10)
		{
		    sortBestCase(LARGE_SIZE);
		}
		else if (choice == 11)
		{
		    sortWorstCase(SMALL_SIZE);
		}
		else if (choice == 12)
		{
		    sortWorstCase(LARGE_SIZE);
		}
		else if (choice == 13)
		{
		    cout << "Thank you for using the Hospital Patient Records Sorting System." << endl;
		}
        else
        {
            cout << "Invalid choice. Please enter a number between 1 and 9." << endl;
        }

    } while (choice != 13);
}

int main()
{
    menu();
    return 0;
}

