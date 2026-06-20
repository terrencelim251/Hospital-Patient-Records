#include <iostream>   // for cin / cout
#include <fstream>    // for reading/writing .txt files (ifstream / ofstream)
#include <cstring>    // for C-style string functions like strcpy(), strcmp()
#include <cstdlib>    // for system("cls") and other system functions
#include <ctime>      // for getting the current system time

using namespace std;

// -------------------- SECTION 2: Global constants --------------------
// Keeping "magic numbers" in one place makes the program easier to change later
const int MAX_NAME_LEN     = 50;     // max length for a name
const int MAX_PASS_LEN     = 20;     // max length for a password
const int MAX_TEXT_LEN     = 100;    // max length for general text fields (address, diagnosis)
const int MAX_PATIENTS     = 200;    // max number of patients the system can hold at once
const int MAX_APPOINTMENTS = 200;    // max number of appointments the system can hold at once

// File names for the 5 required .txt files
const char FILE_PATIENTS[]     = "patients.txt";       // all patient records
const char FILE_STAFF[]        = "staff.txt";           // all staff/admin records
const char FILE_APPOINTMENTS[] = "appointments.txt";    // all appointment records
const char FILE_LOGIN_LOG[]    = "login_log.txt";       // login/logout activity log
const char FILE_SUMMARY[]      = "summary_report.txt";  // saved summary reports


/*
  struct Date
  ------------
  A simple "box" that just groups 3 related numbers together: day, month, year.
  We use struct (not class) here because there is nothing private to protect,
  it is just plain data being bundled together.
*/
struct Date {
    int day;
    int month;
    int year;

    // Default constructor - gives a sensible default value instead of garbage data
    Date() {
        day = 1;
        month = 1;
        year = 2026;
    }

    // Constructor with parameters, so we can create a Date directly with values
    Date(int d, int m, int y) {
        day = d;
        month = m;
        year = y;
    }

    // Turns the date into a text string for saving/printing, format: DD/MM/YYYY
    // sprintf() is a normal C function, not part of STL, so it is allowed
    void toString(char* outStr) const {
        sprintf(outStr, "%02d/%02d/%04d", day, month, year);
    }
};

/*
  struct ContactInfo
  --------------------
  Both Patient and StaffMember need a phone number and address, so this group
  of fields is pulled out into its own struct and reused, instead of writing
  the same two variables twice in two different classes.
*/
struct ContactInfo {
    char phone[20];
    char address[MAX_TEXT_LEN];

    ContactInfo() {
        strcpy(phone, "N/A");
        strcpy(address, "N/A");
    }
};


/*
  Base class 1: Person
  ----------------------
  Both Patient and StaffMember are, at their core, "a person" - they all have a
  name, IC number, age, gender, contact info, and a login username/password.
  Instead of writing these fields twice, we put them once in a base class
  called Person, and let Patient and StaffMember inherit from it.
  This is the whole point of inheritance: write shared fields once, reuse them.
*/
class Person {
protected:
    // protected (not private) on purpose: child classes need direct access,
    // but outside code/classes still cannot touch these fields directly
    char   name[MAX_NAME_LEN];
    char   icNumber[20];     // IC number / passport number
    int    age;
    char   gender;           // 'M' or 'F'
    ContactInfo contact;
    char   username[MAX_NAME_LEN];
    char   password[MAX_PASS_LEN];

public:
    // Default constructor
    Person() {
        strcpy(name, "Unknown");
        strcpy(icNumber, "N/A");
        age = 0;
        gender = 'M';
        strcpy(username, "guest");
        strcpy(password, "0000");
    }

    // Constructor with parameters - this is one of the function "overloads"
    // (same constructor name, different parameter list)
    Person(const char* n, const char* ic, int a, char g,
           const char* user, const char* pass) {
        strcpy(name, n);
        strcpy(icNumber, ic);
        age = a;
        gender = g;
        strcpy(username, user);
        strcpy(password, pass);
    }

    // Virtual destructor.
    // Why does it need to be virtual?
    // If we delete an object through a "Person pointer" that is actually
    // pointing to a Patient object, a non-virtual destructor would only run
    // the Person cleanup code and SKIP the Patient-specific cleanup code.
    // Making it virtual tells C++: "check what this object really is first,
    // then run the correct (most specific) destructor."
    virtual ~Person() {
        // Person itself does not allocate any extra memory right now,
        // but the destructor is still declared virtual so the chain works.
    }

    // Virtual function - will be "overridden" later inside Patient / StaffMember
    virtual void displayInfo() const {
        cout << "Name: " << name << " | IC: " << icNumber
             << " | Age: " << age << " | Gender: " << gender << endl;
    }

    // Simple getter functions used by other parts of the system (e.g. login check)
    const char* getUsername() const { return username; }
    const char* getPassword() const { return password; }
    const char* getName()     const { return name; }

    // Member 2 added these getters below so the file-saving functions can
    // read every field of a Person (Patient/StaffMember) when writing it
    // out to a .txt file.
    const char* getIC()         const { return icNumber; }
    int         getAge()        const { return age; }
    char        getGenderChar() const { return gender; }
    const char* getPhone()      const { return contact.phone; }
    const char* getAddress()    const { return contact.address; }

    // Member 2 added this setter because the constructors above do not take
    // phone/address as parameters. After loading a Patient/StaffMember back
    // from a text file, we use this to fill in the contact info.
    void setContact(const char* phone, const char* address) {
        strcpy(contact.phone, phone);
        strcpy(contact.address, address);
    }
};

/*
  Base class 2: MedicalRecordBase
  ----------------------------------
  This base class represents the shared features of any "record" in the
  hospital system: every record needs a unique ID, a created date, and a
  status. Appointment (below) will inherit from this class.
*/
class MedicalRecordBase {
protected:
    int  recordID;
    Date createdDate;
    char status[20];   // e.g. "Pending", "Completed", "Cancelled"

public:
    MedicalRecordBase() {
        recordID = 0;
        strcpy(status, "Pending");
    }

    MedicalRecordBase(int id, Date d, const char* st) {
        recordID = id;
        createdDate = d;
        strcpy(status, st);
    }

    virtual ~MedicalRecordBase() {
        // left empty on purpose, but still virtual for safe polymorphic delete
    }

    virtual void displayRecord() const {
        char dateStr[15];
        createdDate.toString(dateStr);
        cout << "Record ID: " << recordID << " | Date: " << dateStr
             << " | Status: " << status << endl;
    }

    int  getRecordID() const { return recordID; }
    const char* getStatus() const { return status; }
    void setStatus(const char* st) { strcpy(status, st); }
};


/*
  Derived class 1: Patient (inherits from Person)
  --------------------------------------------------
*/
class Patient : public Person {
private:
    int  patientID;
    Date admissionDate;
    int  wardNo;
    char diagnosis[MAX_TEXT_LEN];
    char assignedDoctor[MAX_NAME_LEN];
    char patientStatus[20];   // "Admitted" or "Discharged"

public:
    // Constructor 1 (default) - satisfies "at least 1 constructor per module"
    Patient() : Person() {
        patientID = 0;
        wardNo = 0;
        strcpy(diagnosis, "N/A");
        strcpy(assignedDoctor, "N/A");
        strcpy(patientStatus, "Admitted");
    }

    // Constructor 2 (with parameters) - this is constructor overloading
    Patient(int id, const char* n, const char* ic, int a, char g,
            const char* user, const char* pass,
            Date admDate, int ward, const char* diag, const char* doctor)
        : Person(n, ic, a, g, user, pass) {
        patientID = id;
        admissionDate = admDate;
        wardNo = ward;
        strcpy(diagnosis, diag);
        strcpy(assignedDoctor, doctor);
        strcpy(patientStatus, "Admitted");
    }

    // Destructor - satisfies "at least 1 destructor per module"
    ~Patient() {
        // Patient does not hold any extra dynamic memory of its own right now,
        // but we still write the destructor explicitly to show it is called
        // correctly along the inheritance chain.
    }

    // Overriding the base class virtual function displayInfo()
    // (same name, same parameters as the base class, but new behaviour)
    void displayInfo() const {
        char dateStr[15];
        admissionDate.toString(dateStr);
        cout << "[Patient] ID:" << patientID << " | Name:" << name
             << " | Age:" << age << " | Gender:" << gender
             << " | Ward:" << wardNo << " | Doctor:" << assignedDoctor
             << " | Admitted:" << dateStr << " | Status:" << patientStatus
             << endl;
    }

    // ---- getters / setters used later by LinkedList, sorting, searching, file I/O ----
    int  getPatientID()      const { return patientID; }
    int  getWardNo()         const { return wardNo; }
    Date getAdmissionDate()  const { return admissionDate; }
    const char* getDiagnosis()      const { return diagnosis; }
    const char* getAssignedDoctor() const { return assignedDoctor; }
    const char* getPatientStatus()  const { return patientStatus; }

    void setDiagnosis(const char* d)      { strcpy(diagnosis, d); }
    void setAssignedDoctor(const char* d) { strcpy(assignedDoctor, d); }
    void setWardNo(int w)                 { wardNo = w; }
    void setPatientStatus(const char* s)  { strcpy(patientStatus, s); }

    // friend function declaration (1 of the 4 required friend functions)
    friend void printPatientBillingSlip(const Patient& p);
};

/*
  Derived class 2: StaffMember (inherits from Person)
  --------------------------------------------------------
*/
class StaffMember : public Person {
private:
    int  staffID;
    char role[20];     // "Doctor", "Nurse", "Admin"
    Date dateJoined;

public:
    // Constructor 1 (default)
    StaffMember() : Person() {
        staffID = 0;
        strcpy(role, "Staff");
    }

    // Constructor 2 (with parameters) - overload
    StaffMember(int id, const char* n, const char* ic, int a, char g,
                const char* user, const char* pass,
                const char* r, Date joined)
        : Person(n, ic, a, g, user, pass) {
        staffID = id;
        strcpy(role, r);
        dateJoined = joined;
    }

    // Destructor
    ~StaffMember() {
    }

    // Override displayInfo()
    void displayInfo() const {
        char dateStr[15];
        dateJoined.toString(dateStr);
        cout << "[Staff] ID:" << staffID << " | Name:" << name
             << " | Role:" << role << " | Joined:" << dateStr << endl;
    }

    int  getStaffID() const { return staffID; }
    const char* getRole() const { return role; }

    // Member 2 added this getter so the file-saving function can read the
    // join date when writing a StaffMember out to staff.txt
    Date getDateJoined() const { return dateJoined; }

    // friend function declaration
    friend bool verifyStaffLogin(const StaffMember& s, const char* userInput, const char* passInput);
};

/*
  Derived class 3: Appointment (inherits from MedicalRecordBase)
  --------------------------------------------------------------------
  Represents a patient's appointment booking. This is the link between the
  Customer module and the Staff module: when a patient (Customer) books an
  appointment, it is saved into appointments.txt, and Staff/Admin reads the
  SAME file - this is how "record consistency" between both modules is achieved.
*/
class Appointment : public MedicalRecordBase {
private:
    int  patientID;          // links this appointment to a specific patient
    char doctorName[MAX_NAME_LEN];
    Date appointmentDate;
    char timeSlot[10];        // e.g. "10:30AM"
    char reason[MAX_TEXT_LEN];

public:
    Appointment() : MedicalRecordBase() {
        patientID = 0;
        strcpy(doctorName, "N/A");
        strcpy(timeSlot, "N/A");
        strcpy(reason, "N/A");
    }

    Appointment(int recID, int patID, const char* doctor, Date apptDate,
                const char* time, const char* r)
        : MedicalRecordBase(recID, apptDate, "Pending") {
        patientID = patID;
        strcpy(doctorName, doctor);
        appointmentDate = apptDate;
        strcpy(timeSlot, time);
        strcpy(reason, r);
    }

    ~Appointment() {
    }

    // Override displayRecord()
    void displayRecord() const {
        char dateStr[15];
        appointmentDate.toString(dateStr);
        cout << "[Appointment] ID:" << recordID << " | PatientID:" << patientID
             << " | Doctor:" << doctorName << " | Date:" << dateStr
             << " | Time:" << timeSlot << " | Reason:" << reason
             << " | Status:" << status << endl;
    }

    int  getPatientID()  const { return patientID; }
    Date getApptDate()   const { return appointmentDate; }
    const char* getDoctorName() const { return doctorName; }
    const char* getTimeSlot()   const { return timeSlot; }
    const char* getReason()     const { return reason; }

    // friend function declaration
    friend void printAppointmentSlip(const Appointment& a);
};


/*
  PatientNode
  ------------
  One "carriage" of the linked list train. Each carriage carries one Patient,
  and has a hook (the next pointer) connecting it to the next carriage.
  To add a patient, we build a new carriage and hook it on.
  To remove a patient, we unhook that carriage and reconnect the ones
  before and after it.
*/
struct PatientNode {
    Patient data;
    PatientNode* next;

    PatientNode(const Patient& p) {
        data = p;
        next = NULL;
    }
};

/*
  PatientLinkedList
  -------------------
  Manages the whole linked list: adding nodes, removing nodes, displaying all
  patients, counting patients, etc. This uses new / delete to manage memory,
  which satisfies the "at least 2 dynamic memory operations" requirement
  (this implementation actually uses many more than 2).
*/
class PatientLinkedList {
private:
    PatientNode* head;   // points to the first node of the list
    int count;            // how many patients are currently in the list

public:
    // Constructor: the list starts out empty
    PatientLinkedList() {
        head = NULL;
        count = 0;
    }

    // Destructor: free every node before the program ends, to avoid memory leaks
    ~PatientLinkedList() {
        PatientNode* current = head;
        while (current != NULL) {
            PatientNode* temp = current;
            current = current->next;
            delete temp;   // <-- dynamic memory release (delete)
        }
        head = NULL;
    }

    // Add a new patient to the end of the list (new admission)
    void insertAtEnd(const Patient& p) {
        PatientNode* newNode = new PatientNode(p);   // <-- dynamic memory allocation (new)
        if (head == NULL) {
            head = newNode;
        } else {
            PatientNode* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
        count++;
    }

    // Remove a patient by Patient ID (discharge / delete record)
    bool deleteByID(int patientID) {
        PatientNode* current = head;
        PatientNode* previous = NULL;

        while (current != NULL) {
            if (current->data.getPatientID() == patientID) {
                if (previous == NULL) {
                    // the node to delete is the very first node (head)
                    head = current->next;
                } else {
                    // skip over "current" by connecting previous directly to current->next
                    previous->next = current->next;
                }
                delete current;   // <-- dynamic memory release (delete)
                count--;
                return true;
            }
            previous = current;
            current = current->next;
        }
        return false;   // ID not found
    }

    // Walk through the whole list and display every patient
    void displayAll() const {
        if (head == NULL) {
            cout << "No patient records found." << endl;
            return;
        }
        PatientNode* current = head;
        while (current != NULL) {
            current->data.displayInfo();
            current = current->next;
        }
    }

    // Find a patient by ID (linear search through the list - a real Binary
    // Search needs an array, which we build with copyToArray() below)
    Patient* findByID(int patientID) {
        PatientNode* current = head;
        while (current != NULL) {
            if (current->data.getPatientID() == patientID) {
                return &(current->data);
            }
            current = current->next;
        }
        return NULL;
    }

    int getCount() const { return count; }

    // Copy the contents of the linked list into a plain array.
    // Why do we need this?
    // Selection Sort and Binary Search both work on arrays using index numbers
    // (arr[0], arr[1], ...), which is much simpler and faster than walking a
    // linked list step by step. So our strategy is: keep the data in a linked
    // list for everyday add/remove operations, but copy it into an array
    // whenever we need to sort or search - this is a common real-world pattern.
    int copyToArray(Patient arr[], int maxSize) const {
        PatientNode* current = head;
        int i = 0;
        while (current != NULL && i < maxSize) {
            arr[i] = current->data;
            i++;
            current = current->next;
        }
        return i;   // returns how many patients were actually copied
    }
};



// Friend function 1: prints a patient's billing slip, directly accessing private data
void printPatientBillingSlip(const Patient& p) {
    cout << "----------- BILLING SLIP -----------" << endl;
    cout << "Patient ID  : " << p.patientID << endl;   // direct access to private member
    cout << "Name        : " << p.name << endl;        // this is the friend's special access
    cout << "Ward No     : " << p.wardNo << endl;
    cout << "Doctor      : " << p.assignedDoctor << endl;
    cout << "-------------------------------------" << endl;
}

// Friend function 2: verifies staff login by directly checking private username/password
bool verifyStaffLogin(const StaffMember& s, const char* userInput, const char* passInput) {
    return (strcmp(s.username, userInput) == 0 && strcmp(s.password, passInput) == 0);
}

// Friend function 3: prints an appointment slip
void printAppointmentSlip(const Appointment& a) {
    char dateStr[15];
    a.appointmentDate.toString(dateStr);
    cout << "--------- APPOINTMENT SLIP ---------" << endl;
    cout << "Appointment ID : " << a.recordID << endl;
    cout << "Patient ID     : " << a.patientID << endl;
    cout << "Doctor         : " << a.doctorName << endl;
    cout << "Date / Time    : " << dateStr << " " << a.timeSlot << endl;
    cout << "-------------------------------------" << endl;
}

/* Friend function 4 will be added in Stage 3 (Staff module), e.g. a function
   that lets a Staff member approve a ward transfer for a Patient. */


/*
  How Selection Sort works, in plain English:
  We walk through the array from left to right. At each position i, we look
  at every remaining element (from i to the end) and find the "best" one
  based on whichever criteria we picked (name / ward number / patient ID).
  Then we swap that best element into position i. Repeat for every position
  until the whole array is sorted. This algorithm is O(n^2), which is fine
  for a hospital system with a few hundred patients.

  criteria parameter meaning:
    1 = sort by patient name (alphabetical order, A to Z)
    2 = sort by ward number (smallest to largest)
    3 = sort by patient ID (smallest to largest) - used internally to
        prepare the array before calling Binary Search
*/
void selectionSortPatients(Patient arr[], int n, int criteria) {
    for (int i = 0; i < n - 1; i++) {
        int bestIndex = i;   // assume position i already holds the best value for now

        for (int j = i + 1; j < n; j++) {
            bool jIsBetter = false;

            if (criteria == 1) {
                // strcmp returns a negative number when arr[j]'s name comes
                // before arr[bestIndex]'s name alphabetically
                if (strcmp(arr[j].getName(), arr[bestIndex].getName()) < 0) {
                    jIsBetter = true;
                }
            } else if (criteria == 2) {
                if (arr[j].getWardNo() < arr[bestIndex].getWardNo()) {
                    jIsBetter = true;
                }
            } else if (criteria == 3) {
                if (arr[j].getPatientID() < arr[bestIndex].getPatientID()) {
                    jIsBetter = true;
                }
            }

            if (jIsBetter) {
                bestIndex = j;
            }
        }

        // only swap if we actually found something better than position i
        if (bestIndex != i) {
            Patient temp = arr[i];
            arr[i] = arr[bestIndex];
            arr[bestIndex] = temp;
        }
    }
}



/*
  How Binary Search works, in plain English:
  Binary Search only works correctly on an array that is already sorted by
  the same field we are searching on. We keep cutting the search range in
  half: check the middle element - if it's too small, move the lower bound
  up; if it's too big, move the upper bound down; repeat until found or the
  range becomes empty. That is why we always call
  selectionSortPatients(arr, n, 3) [sort by ID] right before calling this.

  Returns the array index of the patient if found, or -1 if not found.
*/
int binarySearchPatientByID(Patient arr[], int n, int targetID) {
    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int midID = arr[mid].getPatientID();

        if (midID == targetID) {
            return mid;             // found it
        } else if (midID < targetID) {
            low = mid + 1;          // target must be in the right half
        } else {
            high = mid - 1;         // target must be in the left half
        }
    }
    return -1;   // not found
}



/*
  FileException
  ---------------
  A small hand-written exception class (not using the STL <stdexcept>
  header). We throw an object of this class whenever a file cannot be
  opened, and catch it with try/catch blocks in the Staff and Customer
  modules (added in later stages).
*/
class FileException {
private:
    char message[200];
public:
    FileException(const char* msg) {
        strcpy(message, msg);
    }
    const char* what() const {
        return message;
    }
};

// ---------------------------------------------------------------------------
// PATIENTS file (patients.txt)
// One line per patient, fields separated by '|':
// id|name|ic|age|gender|username|password|phone|address|admDay|admMonth|admYear|ward|diagnosis|doctor|status
// ---------------------------------------------------------------------------
void savePatientsToFile(const PatientLinkedList& list, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open patients file for writing.");
    }

    Patient temp[MAX_PATIENTS];
    int total = list.copyToArray(temp, MAX_PATIENTS);

    for (int i = 0; i < total; i++) {
        Date d = temp[i].getAdmissionDate();
        outFile << temp[i].getPatientID() << "|"
                << temp[i].getName() << "|"
                << temp[i].getIC() << "|"
                << temp[i].getAge() << "|"
                << temp[i].getGenderChar() << "|"
                << temp[i].getUsername() << "|"
                << temp[i].getPassword() << "|"
                << temp[i].getPhone() << "|"
                << temp[i].getAddress() << "|"
                << d.day << "|" << d.month << "|" << d.year << "|"
                << temp[i].getWardNo() << "|"
                << temp[i].getDiagnosis() << "|"
                << temp[i].getAssignedDoctor() << "|"
                << temp[i].getPatientStatus() << "\n";
    }
    outFile.close();
}

void loadPatientsFromFile(PatientLinkedList& list, const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        // It's fine if the file does not exist yet (e.g. first time the
        // program runs) - just leave the list empty instead of crashing.
        return;
    }

    char line[600];
    while (inFile.getline(line, 600)) {
        if (strlen(line) == 0) continue;   // skip blank lines

        char* token;
        token = strtok(line, "|");  int id = atoi(token);
        token = strtok(NULL, "|");  char name[MAX_NAME_LEN]; strcpy(name, token);
        token = strtok(NULL, "|");  char ic[20]; strcpy(ic, token);
        token = strtok(NULL, "|");  int age = atoi(token);
        token = strtok(NULL, "|");  char gender = token[0];
        token = strtok(NULL, "|");  char username[MAX_NAME_LEN]; strcpy(username, token);
        token = strtok(NULL, "|");  char password[MAX_PASS_LEN]; strcpy(password, token);
        token = strtok(NULL, "|");  char phone[20]; strcpy(phone, token);
        token = strtok(NULL, "|");  char address[MAX_TEXT_LEN]; strcpy(address, token);
        token = strtok(NULL, "|");  int admDay = atoi(token);
        token = strtok(NULL, "|");  int admMonth = atoi(token);
        token = strtok(NULL, "|");  int admYear = atoi(token);
        token = strtok(NULL, "|");  int ward = atoi(token);
        token = strtok(NULL, "|");  char diagnosis[MAX_TEXT_LEN]; strcpy(diagnosis, token);
        token = strtok(NULL, "|");  char doctor[MAX_NAME_LEN]; strcpy(doctor, token);
        token = strtok(NULL, "|");  char pStatus[20]; strcpy(pStatus, token);

        Date admDate(admDay, admMonth, admYear);
        Patient p(id, name, ic, age, gender, username, password,
                   admDate, ward, diagnosis, doctor);
        p.setContact(phone, address);
        p.setPatientStatus(pStatus);

        list.insertAtEnd(p);
    }
    inFile.close();
}

// ---------------------------------------------------------------------------
// STAFF file (staff.txt)
// Format: id|name|ic|age|gender|username|password|phone|address|role|joinDay|joinMonth|joinYear
// Staff records are kept in a plain array (not a linked list) - the project
// only requires ONE dynamic non-primitive structure overall, and we already
// used a linked list for Patients, so a simple array is enough here.
// ---------------------------------------------------------------------------
void saveStaffToFile(StaffMember arr[], int n, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open staff file for writing.");
    }
    for (int i = 0; i < n; i++) {
        Date d = arr[i].getDateJoined();
        outFile << arr[i].getStaffID() << "|"
                << arr[i].getName() << "|"
                << arr[i].getIC() << "|"
                << arr[i].getAge() << "|"
                << arr[i].getGenderChar() << "|"
                << arr[i].getUsername() << "|"
                << arr[i].getPassword() << "|"
                << arr[i].getPhone() << "|"
                << arr[i].getAddress() << "|"
                << arr[i].getRole() << "|"
                << d.day << "|" << d.month << "|" << d.year << "\n";
    }
    outFile.close();
}

int loadStaffFromFile(StaffMember arr[], int maxSize, const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return 0;   // no staff file yet, that's fine
    }

    char line[600];
    int count = 0;
    while (inFile.getline(line, 600) && count < maxSize) {
        if (strlen(line) == 0) continue;

        char* token;
        token = strtok(line, "|");  int id = atoi(token);
        token = strtok(NULL, "|");  char name[MAX_NAME_LEN]; strcpy(name, token);
        token = strtok(NULL, "|");  char ic[20]; strcpy(ic, token);
        token = strtok(NULL, "|");  int age = atoi(token);
        token = strtok(NULL, "|");  char gender = token[0];
        token = strtok(NULL, "|");  char username[MAX_NAME_LEN]; strcpy(username, token);
        token = strtok(NULL, "|");  char password[MAX_PASS_LEN]; strcpy(password, token);
        token = strtok(NULL, "|");  char phone[20]; strcpy(phone, token);
        token = strtok(NULL, "|");  char address[MAX_TEXT_LEN]; strcpy(address, token);
        token = strtok(NULL, "|");  char role[20]; strcpy(role, token);
        token = strtok(NULL, "|");  int jDay = atoi(token);
        token = strtok(NULL, "|");  int jMonth = atoi(token);
        token = strtok(NULL, "|");  int jYear = atoi(token);

        Date joined(jDay, jMonth, jYear);
        StaffMember s(id, name, ic, age, gender, username, password, role, joined);
        s.setContact(phone, address);

        arr[count] = s;
        count++;
    }
    inFile.close();
    return count;
}

// ---------------------------------------------------------------------------
// APPOINTMENTS file (appointments.txt)
// Format: recordID|patientID|doctorName|day|month|year|timeSlot|reason|status
// This is the file BOTH modules read/write - when a Customer books an
// appointment it goes in here, and Staff reads the same file, which is how
// "record consistency" between the two modules is achieved.
// ---------------------------------------------------------------------------
void saveAppointmentsToFile(Appointment arr[], int n, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open appointments file for writing.");
    }
    for (int i = 0; i < n; i++) {
        Date d = arr[i].getApptDate();
        outFile << arr[i].getRecordID() << "|"
                << arr[i].getPatientID() << "|"
                << arr[i].getDoctorName() << "|"
                << d.day << "|" << d.month << "|" << d.year << "|"
                << arr[i].getTimeSlot() << "|"
                << arr[i].getReason() << "|"
                << arr[i].getStatus() << "\n";
    }
    outFile.close();
}

int loadAppointmentsFromFile(Appointment arr[], int maxSize, const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return 0;
    }

    char line[600];
    int count = 0;
    while (inFile.getline(line, 600) && count < maxSize) {
        if (strlen(line) == 0) continue;

        char* token;
        token = strtok(line, "|");  int recID = atoi(token);
        token = strtok(NULL, "|");  int patID = atoi(token);
        token = strtok(NULL, "|");  char doctor[MAX_NAME_LEN]; strcpy(doctor, token);
        token = strtok(NULL, "|");  int day = atoi(token);
        token = strtok(NULL, "|");  int month = atoi(token);
        token = strtok(NULL, "|");  int year = atoi(token);
        token = strtok(NULL, "|");  char timeSlot[10]; strcpy(timeSlot, token);
        token = strtok(NULL, "|");  char reason[MAX_TEXT_LEN]; strcpy(reason, token);
        token = strtok(NULL, "|");  char status[20]; strcpy(status, token);

        Date apptDate(day, month, year);
        Appointment a(recID, patID, doctor, apptDate, timeSlot, reason);
        a.setStatus(status);

        arr[count] = a;
        count++;
    }
    inFile.close();
    return count;
}

// ---------------------------------------------------------------------------
// LOGIN LOG file (login_log.txt)
// Every login/logout appends ONE new line - we never overwrite this file,
// so it keeps growing into a full history (this is why we open it in
// ios::app "append" mode instead of the normal write mode).
// ---------------------------------------------------------------------------
void appendLoginLog(const char* username, const char* role, const char* action) {
    ofstream logFile(FILE_LOGIN_LOG, ios::app);
    if (!logFile.is_open()) {
        throw FileException("Could not open login log file.");
    }

    time_t now = time(0);          // get the current system time
    char* timeText = ctime(&now);  // turn it into readable text, e.g. "Thu Jun 18 ..."

    // ctime() puts a '\n' at the end of the string by default - remove it so
    // our log line stays on a single line
    int len = strlen(timeText);
    if (len > 0 && timeText[len - 1] == '\n') {
        timeText[len - 1] = '\0';
    }

    logFile << "[" << timeText << "] " << username << " (" << role << ") - " << action << "\n";
    logFile.close();
}

// ---------------------------------------------------------------------------
// SUMMARY REPORT file (summary_report.txt)
// Required behaviour from the guideline: the report must be displayed AND
// saved to a .txt file, and the system must also be able to read it back
// from that file and display it again.
// ---------------------------------------------------------------------------
void generateAndSavePatientSummary(const PatientLinkedList& list, const char* filename) {
    Patient temp[MAX_PATIENTS];
    int total = list.copyToArray(temp, MAX_PATIENTS);

    int admittedCount = 0;
    int dischargedCount = 0;
    for (int i = 0; i < total; i++) {
        if (strcmp(temp[i].getPatientStatus(), "Admitted") == 0) {
            admittedCount++;
        } else {
            dischargedCount++;
        }
    }

    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open summary report file for writing.");
    }

    outFile << "===== HOSPITAL PATIENT SUMMARY REPORT =====\n";
    outFile << "Total Patients     : " << total << "\n";
    outFile << "Currently Admitted : " << admittedCount << "\n";
    outFile << "Discharged         : " << dischargedCount << "\n";
    outFile << "=============================================\n";
    outFile.close();

    // Also show the same report on screen right away
    cout << "===== HOSPITAL PATIENT SUMMARY REPORT =====" << endl;
    cout << "Total Patients     : " << total << endl;
    cout << "Currently Admitted : " << admittedCount << endl;
    cout << "Discharged         : " << dischargedCount << endl;
    cout << "=============================================" << endl;
}

void loadAndDisplaySummary(const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "No summary report has been generated yet." << endl;
        return;
    }

    cout << "\n----- Reading saved summary report back from file -----" << endl;
    char line[200];
    while (inFile.getline(line, 200)) {
        cout << line << endl;
    }
    inFile.close();
}



int main() {

    // try/catch wraps the whole test so that any FileException thrown by the
    // save/load functions is caught here instead of crashing the program
    try {
        PatientLinkedList patientList;

        // 5 sample patients, added in a deliberately mixed-up order so that
        // sorting actually has something to do
        patientList.insertAtEnd(Patient(1003, "Wong Mei Ling", "920202-10-1111", 28, 'F',
                                          "wong92", "pass789", Date(1, 3, 2026), 2, "Asthma", "Dr. Lim"));
        patientList.insertAtEnd(Patient(1001, "Ahmad Bin Ali", "990101-10-1234", 35, 'M',
                                          "ahmad99", "pass123", Date(15, 3, 2026), 5, "Fever", "Dr. Tan"));
        patientList.insertAtEnd(Patient(1004, "Kumar Raj", "880808-08-8888", 40, 'M',
                                          "kumar88", "passabc", Date(2, 3, 2026), 1, "Diabetes", "Dr. Tan"));
        patientList.insertAtEnd(Patient(1002, "Siti Aminah", "950505-08-5678", 30, 'F',
                                          "siti95", "pass456", Date(16, 3, 2026), 3, "Flu", "Dr. Lee"));
        patientList.insertAtEnd(Patient(1005, "Lee Chong Wei", "910303-14-3333", 33, 'M',
                                          "lee91", "passxyz", Date(3, 3, 2026), 4, "Sprain", "Dr. Lim"));

        // copy the linked list into a plain array so we can sort/search it
        Patient arr[MAX_PATIENTS];
        int total = patientList.copyToArray(arr, MAX_PATIENTS);

        // ---- TEST 1: Selection Sort by name ----
        selectionSortPatients(arr, total, 1);
        cout << "-- Sorted by NAME --" << endl;
        for (int i = 0; i < total; i++) {
            arr[i].displayInfo();
        }

        // ---- TEST 2: Selection Sort by ward number ----
        selectionSortPatients(arr, total, 2);
        cout << "\n-- Sorted by WARD NUMBER --" << endl;
        for (int i = 0; i < total; i++) {
            arr[i].displayInfo();
        }

        // ---- TEST 3: Binary Search by Patient ID ----
        selectionSortPatients(arr, total, 3);   // must sort by ID first
        int searchID = 1004;
        int foundIndex = binarySearchPatientByID(arr, total, searchID);
        cout << "\n-- Binary Search for Patient ID " << searchID << " --" << endl;
        if (foundIndex != -1) {
            cout << "Found at index " << foundIndex << ":" << endl;
            arr[foundIndex].displayInfo();
        } else {
            cout << "Patient not found." << endl;
        }

        // ---- TEST 4: Save patients to file, then load them back ----
        savePatientsToFile(patientList, FILE_PATIENTS);
        cout << "\nPatients saved to " << FILE_PATIENTS << endl;

        PatientLinkedList loadedList;
        loadPatientsFromFile(loadedList, FILE_PATIENTS);
        cout << "\n-- Patients loaded back from file (" << loadedList.getCount() << " records) --" << endl;
        loadedList.displayAll();

        // ---- TEST 5: Summary report (save, then read back) ----
        generateAndSavePatientSummary(loadedList, FILE_SUMMARY);
        loadAndDisplaySummary(FILE_SUMMARY);

        // ---- TEST 6: Login log ----
        appendLoginLog("ahmad99", "Patient", "LOGIN");
        cout << "\nLogin log entry written to " << FILE_LOGIN_LOG << endl;

        // ---- TEST 7: Staff file save/load ----
        StaffMember staffArr[10];
        staffArr[0] = StaffMember(2001, "Dr. Tan Wei Jian", "850101-10-5555", 45, 'M',
                                   "drtan", "staffpass1", "Doctor", Date(1, 1, 2020));
        staffArr[1] = StaffMember(2002, "Nurse Farah", "930202-08-2222", 31, 'F',
                                   "farah93", "staffpass2", "Nurse", Date(5, 6, 2022));
        saveStaffToFile(staffArr, 2, FILE_STAFF);

        StaffMember loadedStaff[10];
        int staffCount = loadStaffFromFile(loadedStaff, 10, FILE_STAFF);
        cout << "\n-- Staff loaded back from file (" << staffCount << " records) --" << endl;
        for (int i = 0; i < staffCount; i++) {
            loadedStaff[i].displayInfo();
        }

        // ---- TEST 8: Appointment file save/load ----
        Appointment apptArr[10];
        apptArr[0] = Appointment(3001, 1001, "Dr. Tan", Date(20, 3, 2026), "09:00AM", "Follow-up checkup");
        saveAppointmentsToFile(apptArr, 1, FILE_APPOINTMENTS);

        Appointment loadedAppts[10];
        int apptCount = loadAppointmentsFromFile(loadedAppts, 10, FILE_APPOINTMENTS);
        cout << "\n-- Appointments loaded back from file (" << apptCount << " records) --" << endl;
        for (int i = 0; i < apptCount; i++) {
            loadedAppts[i].displayRecord();
        }

    } catch (FileException& e) {
        // This is the kind of try/catch the guideline requires - it stops a
        // file error from crashing the whole program
        cout << "File error occurred: " << e.what() << endl;
    }

    return 0;
}
