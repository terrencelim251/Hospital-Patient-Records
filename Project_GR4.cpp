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
const int MAX_STAFF        = 50;     // max number of staff/admin accounts

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
        // nothing to clean up here, just keeping the destructor virtual
        // so the child class destructor still gets called properly
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
    int  wardNumber;
    char diagnosis[MAX_TEXT_LEN];
    char assignedDoctor[MAX_NAME_LEN];
    char patientStatus[20];   // "Admitted" or "Discharged"

public:
    // Constructor 1 (default) - satisfies "at least 1 constructor per module"
    Patient() : Person() {
        patientID = 0;
        wardNumber = 0;
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
        wardNumber = ward;
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
             << " | Ward:" << wardNumber << " | Doctor:" << assignedDoctor
             << " | Admitted:" << dateStr << " | Status:" << patientStatus
             << endl;
    }

    // ---- getters / setters used later by LinkedList, sorting, searching, file I/O ----
    int  getPatientID()      const { return patientID; }
    int  getWardNumber()         const { return wardNumber; }
    Date getAdmissionDate()  const { return admissionDate; }
    const char* getDiagnosis()      const { return diagnosis; }
    const char* getAssignedDoctor() const { return assignedDoctor; }
    const char* getPatientStatus()  const { return patientStatus; }

    void setDiagnosis(const char* d)      { strcpy(diagnosis, d); }
    void setAssignedDoctor(const char* d) { strcpy(assignedDoctor, d); }
    void setWardNumber(int w)                 { wardNumber = w; }
    void setPatientStatus(const char* s)  { strcpy(patientStatus, s); }

    // friend function declaration (1 of the 4 required friend functions)
    friend void printPatientBillingSlip(const Patient& p);

    // friend function declaration (4th friend function - needs access to
    // BOTH Patient's private wardNumber AND StaffMember's private role, which
    // is why it must be declared friend in both classes)
    friend bool transferPatientWard(Patient& p, const class StaffMember& approver, int newWard);
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

    // friend function declaration (4th friend function, declared in BOTH
    // Patient and StaffMember because it touches private data of both)
    friend bool transferPatientWard(class Patient& p, const StaffMember& approver, int newWard);
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

    // Member 4 added these two setters so a Customer can edit their own
    // appointment (change the date/time, or change the reason for visit)
    void setApptDateTime(Date newDate, const char* newTime) {
        appointmentDate = newDate;
        strcpy(timeSlot, newTime);
    }
    void setReason(const char* newReason) {
        strcpy(reason, newReason);
    }

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

    // Member 4 added this: search by username instead of ID, used for
    // Patient login (a patient logs in with username, not with their ID)
    Patient* findByUsername(const char* username) {
        PatientNode* current = head;
        while (current != NULL) {
            if (strcmp(current->data.getUsername(), username) == 0) {
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
    int copyToArray(Patient patientArray[], int maxSize) const {
        PatientNode* current = head;
        int i = 0;
        while (current != NULL && i < maxSize) {
            patientArray[i] = current->data;
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
    cout << "Ward No     : " << p.wardNumber << endl;
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

/* Friend function 4: lets an approving Staff member (must be a Doctor or
   Admin) move a Patient to a different ward. This needs friend access to
   BOTH classes: Patient's private wardNumber, and StaffMember's private role. */
bool transferPatientWard(Patient& p, const StaffMember& approver, int newWard) {
    // only Doctor or Admin roles may approve a ward transfer
    if (strcmp(approver.role, "Doctor") != 0 && strcmp(approver.role, "Admin") != 0) {
        return false;
    }
    p.wardNumber = newWard;   // direct write to Patient's private member, allowed because friend
    return true;
}


/*
  This function sorts the patient array using Selection Sort.
  Selection Sort just means: look through the list, find the smallest
  (or best) one, put it at the front, then look at what is left and do
  the same thing again, over and over until everything is sorted.

  criteria meaning:
    1 = sort by patient name
    2 = sort by ward number
    3 = sort by patient ID (we use this one before doing Binary Search)
*/
void selectionSortPatients(Patient patientArray[], int totalPatients, int criteria) {
    for (int i = 0; i < totalPatients - 1; i++) {
        int bestIndex = i;   // for now we think position i is the best one

        for (int j = i + 1; j < totalPatients; j++) {
            bool jIsBetter = false;

            if (criteria == 1) {
                // strcmp gives a negative number if the name at j comes
                // first in alphabet order
                if (strcmp(patientArray[j].getName(), patientArray[bestIndex].getName()) < 0) {
                    jIsBetter = true;
                }
            } else if (criteria == 2) {
                if (patientArray[j].getWardNumber() < patientArray[bestIndex].getWardNumber()) {
                    jIsBetter = true;
                }
            } else if (criteria == 3) {
                if (patientArray[j].getPatientID() < patientArray[bestIndex].getPatientID()) {
                    jIsBetter = true;
                }
            }

            if (jIsBetter == true) {
                bestIndex = j;
            }
        }

        // swap only if we found someone better than position i
        if (bestIndex != i) {
            Patient temporaryPatient = patientArray[i];
            patientArray[i] = patientArray[bestIndex];
            patientArray[bestIndex] = temporaryPatient;
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
int binarySearchPatientByID(Patient patientArray[], int totalPatients, int targetID) {
    int low = 0;
    int high = totalPatients - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int midID = patientArray[mid].getPatientID();

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
                << temp[i].getWardNumber() << "|"
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
void saveStaffToFile(StaffMember staffArray[], int numberOfStaffToSave, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open staff file for writing.");
    }
    for (int i = 0; i < numberOfStaffToSave; i++) {
        Date d = staffArray[i].getDateJoined();
        outFile << staffArray[i].getStaffID() << "|"
                << staffArray[i].getName() << "|"
                << staffArray[i].getIC() << "|"
                << staffArray[i].getAge() << "|"
                << staffArray[i].getGenderChar() << "|"
                << staffArray[i].getUsername() << "|"
                << staffArray[i].getPassword() << "|"
                << staffArray[i].getPhone() << "|"
                << staffArray[i].getAddress() << "|"
                << staffArray[i].getRole() << "|"
                << d.day << "|" << d.month << "|" << d.year << "\numberOfStaffToSave";
    }
    outFile.close();
}

int loadStaffFromFile(StaffMember staffArray[], int maxSize, const char* filename) {
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

        staffArray[count] = s;
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
void saveAppointmentsToFile(Appointment appointmentArray[], int numberOfAppointmentsToSave, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open appointments file for writing.");
    }
    for (int i = 0; i < numberOfAppointmentsToSave; i++) {
        Date d = appointmentArray[i].getApptDate();
        outFile << appointmentArray[i].getRecordID() << "|"
                << appointmentArray[i].getPatientID() << "|"
                << appointmentArray[i].getDoctorName() << "|"
                << d.day << "|" << d.month << "|" << d.year << "|"
                << appointmentArray[i].getTimeSlot() << "|"
                << appointmentArray[i].getReason() << "|"
                << appointmentArray[i].getStatus() << "\n";
    }
    outFile.close();
}

int loadAppointmentsFromFile(Appointment appointmentArray[], int maxSize, const char* filename) {
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

        appointmentArray[count] = a;
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

    // Member 4 note: there are now THREE possible statuses, not just two -
    // "Registered" (self-registered but not yet admitted to a ward),
    // "Admitted", and "Discharged" - so we count all three separately
    int registeredCount = 0;
    int admittedCount = 0;
    int dischargedCount = 0;
    for (int i = 0; i < total; i++) {
        if (strcmp(temp[i].getPatientStatus(), "Admitted") == 0) {
            admittedCount++;
        } else if (strcmp(temp[i].getPatientStatus(), "Registered") == 0) {
            registeredCount++;
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
    outFile << "Registered Only    : " << registeredCount << "\n";
    outFile << "Currently Admitted : " << admittedCount << "\n";
    outFile << "Discharged         : " << dischargedCount << "\n";
    outFile << "=============================================\n";
    outFile.close();

    // Also show the same report on screen right away
    cout << "===== HOSPITAL PATIENT SUMMARY REPORT =====" << endl;
    cout << "Total Patients     : " << total << endl;
    cout << "Registered Only    : " << registeredCount << endl;
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


class InputException {
private:
    char message[200];
public:
    InputException(const char* msg) {
        strcpy(message, msg);
    }
    const char* what() const {
        return message;
    }
};


// Works out the next free Patient ID by looking at the highest ID currently
// in the list and adding 1 (very simple "auto increment" done by hand)
int generateNextPatientID(const PatientLinkedList& list) {
    Patient temp[MAX_PATIENTS];
    int total = list.copyToArray(temp, MAX_PATIENTS);
    int maxID = 1000;
    for (int i = 0; i < total; i++) {
        if (temp[i].getPatientID() > maxID) {
            maxID = temp[i].getPatientID();
        }
    }
    return maxID + 1;
}

// ---- Registration ----
void registerStaff(StaffMember staffArray[], int& numberOfStaff, int maxSize) {
    if (numberOfStaff >= maxSize) {
        cout << "Staff list is full, cannot register more." << endl;
        return;
    }

    char name[MAX_NAME_LEN], ic[20], username[MAX_NAME_LEN], password[MAX_PASS_LEN];
    char role[20], phone[20], address[MAX_TEXT_LEN];
    int age;
    char gender;

    cout << "\n-- Staff Registration --" << endl;
    cout << "Full Name: ";
    cin.ignore(1000, '\n');   // flush leftover newline from the previous cin >> choice
    cin.getline(name, MAX_NAME_LEN);
    cout << "IC Number: "; cin.getline(ic, 20);
    cout << "Age: "; cin >> age;
    cout << "Gender (M/F): "; cin >> gender;
    cout << "Role (Doctor/Nurse/Admin): "; cin >> role;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;
    cin.ignore(1000, '\n');
    cout << "Phone: "; cin.getline(phone, 20);
    cout << "Address: "; cin.getline(address, MAX_TEXT_LEN);

    // make sure the username is not already taken by another staff member
    for (int i = 0; i < numberOfStaff; i++) {
        if (strcmp(staffArray[i].getUsername(), username) == 0) {
            cout << "Username already taken. Registration cancelled." << endl;
            return;
        }
    }

    int newID = 2000 + numberOfStaff + 1;
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date joined(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);

    StaffMember newStaff(newID, name, ic, age, gender, username, password, role, joined);
    newStaff.setContact(phone, address);

    staffArray[numberOfStaff] = newStaff;
    numberOfStaff++;

    saveStaffToFile(staffArray, numberOfStaff, FILE_STAFF);
    cout << "Registration successful! Your Staff ID is " << newID << endl;
}

// ---- Login ----
// Returns the array index of the staff member if login is correct, or -1 if not
int staffLoginPrompt(StaffMember staffArray[], int numberOfStaff) {
    char username[MAX_NAME_LEN], password[MAX_PASS_LEN];
    cout << "\n-- Staff Login --" << endl;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;

    for (int i = 0; i < numberOfStaff; i++) {
        if (verifyStaffLogin(staffArray[i], username, password)) {
            cout << "Login successful. Welcome, " << staffArray[i].getName()
                 << " (" << staffArray[i].getRole() << ")" << endl;
            return i;
        }
    }
    return -1;
}

// ---- Add New Patient Record ----
void addPatientRecord(PatientLinkedList& list) {
    char name[MAX_NAME_LEN], ic[20], username[MAX_NAME_LEN], password[MAX_PASS_LEN];
    char diagnosis[MAX_TEXT_LEN], doctor[MAX_NAME_LEN], phone[20], address[MAX_TEXT_LEN];
    int age, ward;
    char gender;

    cout << "\n-- Add New Patient --" << endl;
    cin.ignore(1000, '\n');
    cout << "Full Name: "; cin.getline(name, MAX_NAME_LEN);
    cout << "IC Number: "; cin.getline(ic, 20);
    cout << "Age: "; cin >> age;
    cout << "Gender (M/F): "; cin >> gender;
    cout << "Ward Number (1-10): "; cin >> ward;

    // basic input validation - this is where the try/catch error handling
    // required by the guideline actually gets used in this module
    if (ward < 1 || ward > 10) {
        throw InputException("Ward number must be between 1 and 10.");
    }

    cin.ignore(1000, '\n');
    cout << "Diagnosis: "; cin.getline(diagnosis, MAX_TEXT_LEN);
    cout << "Assigned Doctor: "; cin.getline(doctor, MAX_NAME_LEN);
    cout << "Username (for patient login): "; cin >> username;
    cout << "Password: "; cin >> password;
    cin.ignore(1000, '\n');
    cout << "Phone: "; cin.getline(phone, 20);
    cout << "Address: "; cin.getline(address, MAX_TEXT_LEN);

    int newID = generateNextPatientID(list);
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date admDate(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);

    Patient newPatient(newID, name, ic, age, gender, username, password,
                        admDate, ward, diagnosis, doctor);
    newPatient.setContact(phone, address);

    list.insertAtEnd(newPatient);

    // save immediately so the Customer module (running as a separate
    // session) always sees the latest data - this is the "record
    // consistency" requirement from the guideline
    savePatientsToFile(list, FILE_PATIENTS);
    cout << "Patient added successfully! Patient ID: " << newID << endl;
}

// ---- Edit Patient Record ----
void editPatientRecord(PatientLinkedList& list) {
    int id;
    cout << "\n-- Edit Patient Record --" << endl;
    cout << "Enter Patient ID to edit: ";
    cin >> id;

    Patient* targetPatient = list.findByID(id);
    if (targetPatient == NULL) {
        throw InputException("Patient ID not found.");
    }

    cout << "Current record:" << endl;
    targetPatient->displayInfo();

    int editChoice;
    cout << "\nWhat do you want to edit?" << endl;
    cout << "1. Diagnosis" << endl;
    cout << "2. Assigned Doctor" << endl;
    cout << "3. Ward Number" << endl;
    cout << "4. Patient Status (Admitted/Discharged)" << endl;
    cout << "Enter choice: ";
    cin >> editChoice;
    cin.ignore(1000, '\n');

    if (editChoice == 1) {
        char newDiag[MAX_TEXT_LEN];
        cout << "New Diagnosis: "; cin.getline(newDiag, MAX_TEXT_LEN);
        targetPatient->setDiagnosis(newDiag);
    } else if (editChoice == 2) {
        char newDoc[MAX_NAME_LEN];
        cout << "New Assigned Doctor: "; cin.getline(newDoc, MAX_NAME_LEN);
        targetPatient->setAssignedDoctor(newDoc);
    } else if (editChoice == 3) {
        int newWard;
        cout << "New Ward Number: "; cin >> newWard;
        targetPatient->setWardNumber(newWard);
    } else if (editChoice == 4) {
        char newStatus[20];
        cout << "New Status (Admitted/Discharged): "; cin >> newStatus;
        targetPatient->setPatientStatus(newStatus);
    } else {
        throw InputException("Invalid edit option.");
    }

    savePatientsToFile(list, FILE_PATIENTS);
    cout << "Patient record updated successfully." << endl;
}

// ---- Delete Patient Record ----
void deletePatientRecord(PatientLinkedList& list) {
    int id;
    cout << "\n-- Delete Patient Record --" << endl;
    cout << "Enter Patient ID to delete: ";
    cin >> id;

    bool removed = list.deleteByID(id);
    if (removed == false) {
        throw InputException("Patient ID not found, nothing deleted.");
    }

    savePatientsToFile(list, FILE_PATIENTS);
    cout << "Patient record deleted successfully." << endl;
}

// ---- Search Patient Record (2 criteria: by ID and by Name) ----
void searchPatientMenu(const PatientLinkedList& list) {
    int searchChoice;
    cout << "\n-- Search Patient --" << endl;
    cout << "1. Search by Patient ID (Binary Search)" << endl;
    cout << "2. Search by Name (Linear Search)" << endl;
    cout << "Enter choice: ";
    cin >> searchChoice;

    Patient patientArray[MAX_PATIENTS];
    int total = list.copyToArray(patientArray, MAX_PATIENTS);

    if (searchChoice == 1) {
        int id;
        cout << "Enter Patient ID: ";
        cin >> id;
        selectionSortPatients(patientArray, total, 3);   // Binary Search needs the array sorted by ID first
        int idx = binarySearchPatientByID(patientArray, total, id);
        if (idx == -1) {
            throw InputException("No patient found with that ID.");
        }
        patientArray[idx].displayInfo();

    } else if (searchChoice == 2) {
        char name[MAX_NAME_LEN];
        cin.ignore(1000, '\n');
        cout << "Enter Patient Name (or part of it): ";
        cin.getline(name, MAX_NAME_LEN);

        bool found = false;
        for (int i = 0; i < total; i++) {
            // strstr() is a plain C string function (checks if "name" is a
            // substring of patientArray[i]'s name) - not an STL search function
            if (strstr(patientArray[i].getName(), name) != NULL) {
                patientArray[i].displayInfo();
                found = true;
            }
        }
        if (found == false) {
            throw InputException("No patient found with that name.");
        }
    } else {
        throw InputException("Invalid search option.");
    }
}

// ---- Sort Patient Records (2 criteria: by Name and by Ward Number) ----
void sortPatientsMenu(PatientLinkedList& list) {
    int sortChoice;
    cout << "\n-- Sort Patient Records --" << endl;
    cout << "1. Sort by Name" << endl;
    cout << "2. Sort by Ward Number" << endl;
    cout << "Enter choice: ";
    cin >> sortChoice;

    if (sortChoice != 1 && sortChoice != 2) {
        throw InputException("Invalid sort option.");
    }

    Patient patientArray[MAX_PATIENTS];
    int total = list.copyToArray(patientArray, MAX_PATIENTS);
    selectionSortPatients(patientArray, total, sortChoice);

    cout << "\n-- Sorted Result --" << endl;
    for (int i = 0; i < total; i++) {
        patientArray[i].displayInfo();
    }
}

// ---- Summary Report (display + save to txt + read back from txt) ----
void staffSummaryReportMenu(const PatientLinkedList& list) {
    generateAndSavePatientSummary(list, FILE_SUMMARY);
    cout << "\nView the saved report from file as well? (Y/N): ";
    char ans;
    cin >> ans;
    if (ans == 'Y' || ans == 'y') {
        loadAndDisplaySummary(FILE_SUMMARY);
    }
}

// ---- Appointment management (extra feature: keeps Staff aware of bookings
// made by Customers, which is part of the "record consistency" requirement) ----
void viewAllAppointments(Appointment appointmentArray[], int numberOfAppointments) {
    cout << "\n-- All Appointments --" << endl;
    if (numberOfAppointments == 0) {
        cout << "No appointments found." << endl;
        return;
    }
    for (int i = 0; i < numberOfAppointments; i++) {
        appointmentArray[i].displayRecord();
    }
}

void updateAppointmentStatusMenu(Appointment appointmentArray[], int numberOfAppointments) {
    int recID;
    cout << "\n-- Update Appointment Status --" << endl;
    cout << "Enter Appointment ID: ";
    cin >> recID;

    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getRecordID() == recID) {
            char newStatus[20];
            cout << "New Status (Pending/Confirmed/Completed/Cancelled): ";
            cin >> newStatus;
            appointmentArray[i].setStatus(newStatus);
            saveAppointmentsToFile(appointmentArray, numberOfAppointments, FILE_APPOINTMENTS);
            cout << "Appointment status updated." << endl;
            return;
        }
    }
    throw InputException("Appointment ID not found.");
}

// ---- Ward Transfer (uses the 4th friend function, transferPatientWard) ----
void approveWardTransferMenu(PatientLinkedList& list, const StaffMember& approver) {
    int id, newWard;
    cout << "\n-- Approve Ward Transfer --" << endl;
    cout << "Enter Patient ID: ";
    cin >> id;
    cout << "New Ward Number: ";
    cin >> newWard;

    Patient* targetPatient = list.findByID(id);
    if (targetPatient == NULL) {
        throw InputException("Patient ID not found.");
    }

    bool success = transferPatientWard(*targetPatient, approver, newWard);
    if (success == false) {
        cout << "Only Admin or Doctor roles can approve a ward transfer." << endl;
        return;
    }

    savePatientsToFile(list, FILE_PATIENTS);
    cout << "Ward transfer approved. Patient is now in Ward " << newWard << "." << endl;
}

// ---- Main Staff menu loop (shown after a successful login) ----
void staffMainMenu(PatientLinkedList& patientList, StaffMember& currentStaff,
                    Appointment appointmentArray[], int numberOfAppointments) {
    int choice = -1;

    do {
        cout << "\n========== STAFF MENU (" << currentStaff.getName() << ") ==========" << endl;
        cout << "1. Add New Patient Record" << endl;
        cout << "2. Edit Patient Record" << endl;
        cout << "3. Delete Patient Record" << endl;
        cout << "4. Display All Patient Records" << endl;
        cout << "5. Search Patient Record" << endl;
        cout << "6. Sort Patient Records" << endl;
        cout << "7. Generate / View Summary Report" << endl;
        cout << "8. View All Appointments" << endl;
        cout << "9. Update Appointment Status" << endl;
        cout << "10. Approve Ward Transfer" << endl;
        cout << "0. Logout" << endl;
        cout << "Enter choice: ";

        // try/catch around each menu action: if anything goes wrong (bad
        // input, record not found, file error) we print a friendly message
        // and go straight back to the menu instead of crashing the program
        try {
            cin >> choice;

            switch (choice) {
                case 1:  addPatientRecord(patientList); break;
                case 2:  editPatientRecord(patientList); break;
                case 3:  deletePatientRecord(patientList); break;
                case 4:  patientList.displayAll(); break;
                case 5:  searchPatientMenu(patientList); break;
                case 6:  sortPatientsMenu(patientList); break;
                case 7:  staffSummaryReportMenu(patientList); break;
                case 8:  viewAllAppointments(appointmentArray, numberOfAppointments); break;
                case 9:  updateAppointmentStatusMenu(appointmentArray, numberOfAppointments); break;
                case 10: approveWardTransferMenu(patientList, currentStaff); break;
                case 0:  cout << "Logging out..." << endl; break;
                default: cout << "Invalid choice, please try again." << endl;
            }
        } catch (InputException& e) {
            cout << "Input error: " << e.what() << endl;
        } catch (FileException& e) {
            cout << "File error: " << e.what() << endl;
        }

    } while (choice != 0);
}

// ---- Entry point for the whole Staff/Admin module (Register or Login) ----
void staffModuleEntry(PatientLinkedList& patientList, StaffMember staffArray[], int& numberOfStaff,
                       Appointment appointmentArray[], int numberOfAppointments) {
    int choice;
    cout << "\n========== STAFF / ADMIN MODULE ==========" << endl;
    cout << "1. Register new staff account" << endl;
    cout << "2. Login" << endl;
    cout << "3. Back to main menu" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        registerStaff(staffArray, numberOfStaff, MAX_STAFF);
    }

    if (choice == 1 || choice == 2) {
        int loggedInIndex = staffLoginPrompt(staffArray, numberOfStaff);
        if (loggedInIndex != -1) {
            appendLoginLog(staffArray[loggedInIndex].getUsername(), staffArray[loggedInIndex].getRole(), "LOGIN");
            staffMainMenu(patientList, staffArray[loggedInIndex], appointmentArray, numberOfAppointments);
            appendLoginLog(staffArray[loggedInIndex].getUsername(), staffArray[loggedInIndex].getRole(), "LOGOUT");
        } else {
            cout << "Login failed. Returning to main menu." << endl;
        }
    } else {
        cout << "Returning to main menu." << endl;
    }
}



// Works out the next free Appointment ID, same idea as generateNextPatientID()
int generateNextAppointmentID(Appointment appointmentArray[], int numberOfAppointments) {
    int maxID = 3000;
    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getRecordID() > maxID) {
            maxID = appointmentArray[i].getRecordID();
        }
    }
    return maxID + 1;
}

// ---- Registration ----
// A patient registering themselves only creates a basic profile - ward,
// diagnosis and doctor are left as "N/A" with status "Registered" because
// those details only get filled in later by Staff when the patient is
// actually admitted (see editPatientRecord() in the Staff module, Stage 3).
void registerPatient(PatientLinkedList& list) {
    char name[MAX_NAME_LEN], ic[20], username[MAX_NAME_LEN], password[MAX_PASS_LEN];
    char phone[20], address[MAX_TEXT_LEN];
    int age;
    char gender;

    cout << "\n-- Patient Registration --" << endl;
    cin.ignore(1000, '\n');
    cout << "Full Name: "; cin.getline(name, MAX_NAME_LEN);
    cout << "IC Number: "; cin.getline(ic, 20);
    cout << "Age: "; cin >> age;
    cout << "Gender (M/F): "; cin >> gender;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;
    cin.ignore(1000, '\n');
    cout << "Phone: "; cin.getline(phone, 20);
    cout << "Address: "; cin.getline(address, MAX_TEXT_LEN);

    if (list.findByUsername(username) != NULL) {
        cout << "Username already taken. Registration cancelled." << endl;
        return;
    }

    int newID = generateNextPatientID(list);
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date today(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);

    Patient newPatient(newID, name, ic, age, gender, username, password,
                        today, 0, "N/A", "N/A");
    newPatient.setContact(phone, address);
    newPatient.setPatientStatus("Registered");   // not yet admitted to a ward

    list.insertAtEnd(newPatient);
    savePatientsToFile(list, FILE_PATIENTS);

    cout << "Registration successful! Your Patient ID is " << newID << endl;
    cout << "Please visit the hospital counter to complete your admission." << endl;
}

// ---- Login ----
// Returns a pointer to the patient's record inside the linked list if the
// username/password is correct, or NULL if not. We search by username
// because patients log in with a username, not with their numeric ID.
Patient* patientLoginPrompt(PatientLinkedList& list) {
    char username[MAX_NAME_LEN], password[MAX_PASS_LEN];
    cout << "\n-- Patient Login --" << endl;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;

    Patient* targetPatient = list.findByUsername(username);

    // check step by step instead of putting everything in one if condition
    if (targetPatient == NULL) {
        return NULL;
    }

    if (strcmp(targetPatient->getPassword(), password) != 0) {
        return NULL;
    }

    cout << "Login successful. Welcome, " << targetPatient->getName() << "!" << endl;
    return targetPatient;
}

// ---- View own profile ----
void displayMyProfile(const Patient& patient) {
    cout << "\n-- My Profile --" << endl;
    patient.displayInfo();
}

// ---- Add New Record: Book a new appointment ----
void bookAppointment(Appointment appointmentArray[], int& numberOfAppointments, int maxArraySize, int patientID) {
    if (numberOfAppointments >= maxArraySize) {
        cout << "Appointment list is full, cannot book more right now." << endl;
        return;
    }

    char doctor[MAX_NAME_LEN], timeSlot[10], reason[MAX_TEXT_LEN];
    int day, month, year;

    cout << "\n-- Book New Appointment --" << endl;
    cin.ignore(1000, '\n');
    cout << "Preferred Doctor: "; cin.getline(doctor, MAX_NAME_LEN);
    cout << "Date (day month year, e.g. 25 6 2026): ";
    cin >> day >> month >> year;

    // check the day, month, year one by one instead of one big if condition
    bool dateIsBad = false;
    if (day < 1 || day > 31) {
        dateIsBad = true;
    }
    if (month < 1 || month > 12) {
        dateIsBad = true;
    }
    if (year < 2026) {
        dateIsBad = true;
    }
    if (dateIsBad == true) {
        throw InputException("Invalid date entered.");
    }

    cin.ignore(1000, '\n');
    cout << "Preferred Time (e.g. 10:30AM): "; cin.getline(timeSlot, 10);
    cout << "Reason for visit: "; cin.getline(reason, MAX_TEXT_LEN);

    int newID = generateNextAppointmentID(appointmentArray, numberOfAppointments);
    Date apptDate(day, month, year);
    Appointment newAppt(newID, patientID, doctor, apptDate, timeSlot, reason);

    appointmentArray[numberOfAppointments] = newAppt;
    numberOfAppointments++;

    // save immediately, so Staff sees this new booking right away the next
    // time they open the Staff module (this is "record consistency" again)
    saveAppointmentsToFile(appointmentArray, numberOfAppointments, FILE_APPOINTMENTS);
    cout << "Appointment booked successfully! Appointment ID: " << newID << endl;
}

// ---- Edit/Update Record: change date/time or reason on YOUR OWN booking ----
void editOwnAppointment(Appointment appointmentArray[], int numberOfAppointments, int patientID) {
    int apptID;
    cout << "\n-- Edit My Appointment --" << endl;
    cout << "Enter your Appointment ID: ";
    cin >> apptID;

    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getRecordID() == apptID) {
            // security check: a patient may only edit their OWN bookings,
            // never someone else's, even if they guess the right ID
            if (appointmentArray[i].getPatientID() != patientID) {
                throw InputException("You can only edit your own appointments.");
            }

            int editChoice;
            cout << "1. Change Date/Time" << endl;
            cout << "2. Change Reason for Visit" << endl;
            cout << "Enter choice: ";
            cin >> editChoice;

            if (editChoice == 1) {
                int day, month, year;
                char timeSlot[10];
                cout << "New Date (day month year): "; cin >> day >> month >> year;
                cout << "New Time: "; cin >> timeSlot;
                appointmentArray[i].setApptDateTime(Date(day, month, year), timeSlot);
            } else if (editChoice == 2) {
                char reason[MAX_TEXT_LEN];
                cin.ignore(1000, '\n');
                cout << "New Reason: "; cin.getline(reason, MAX_TEXT_LEN);
                appointmentArray[i].setReason(reason);
            } else {
                throw InputException("Invalid edit option.");
            }

            saveAppointmentsToFile(appointmentArray, numberOfAppointments, FILE_APPOINTMENTS);
            cout << "Appointment updated successfully." << endl;
            return;
        }
    }
    throw InputException("Appointment not found.");
}

// ---- Delete Record: cancel YOUR OWN booking ----
void cancelOwnAppointment(Appointment appointmentArray[], int& numberOfAppointments, int patientID) {
    int apptID;
    cout << "\n-- Cancel My Appointment --" << endl;
    cout << "Enter Appointment ID to cancel: ";
    cin >> apptID;

    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getRecordID() == apptID) {
            if (appointmentArray[i].getPatientID() != patientID) {
                throw InputException("You can only cancel your own appointments.");
            }
            // shift every element after position i back by one, to close the gap
            for (int j = i; j < numberOfAppointments - 1; j++) {
                appointmentArray[j] = appointmentArray[j + 1];
            }
            numberOfAppointments--;
            saveAppointmentsToFile(appointmentArray, numberOfAppointments, FILE_APPOINTMENTS);
            cout << "Appointment cancelled successfully." << endl;
            return;
        }
    }
    throw InputException("Appointment not found.");
}

// ---- Display Record (2 modes: all bookings / only active bookings) ----
void displayOwnAppointmentsMenu(Appointment appointmentArray[], int numberOfAppointments, int patientID) {
    int choice;
    cout << "\n-- Display My Bookings --" << endl;
    cout << "1. Display All My Bookings" << endl;
    cout << "2. Display Only Active Bookings (Pending/Confirmed)" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    bool found = false;
    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getPatientID() != patientID) {
            continue;   // skip bookings that belong to other patients
        }
        if (choice == 2) {
            // check if this booking is "active" by comparing the status text
            // one by one, instead of doing it in one line
            bool isActive = false;
            if (strcmp(appointmentArray[i].getStatus(), "Pending") == 0) {
                isActive = true;
            }
            if (strcmp(appointmentArray[i].getStatus(), "Confirmed") == 0) {
                isActive = true;
            }
            if (isActive == false) {
                continue;
            }
        }
        appointmentArray[i].displayRecord();
        found = true;
    }
    if (found == false) {
        cout << "No matching bookings found." << endl;
    }
}

// ---- Search Record (2 criteria: by Appointment ID, by Doctor Name) ----
void searchOwnAppointmentsMenu(Appointment appointmentArray[], int numberOfAppointments, int patientID) {
    int choice;
    cout << "\n-- Search My Bookings --" << endl;
    cout << "1. Search by Appointment ID" << endl;
    cout << "2. Search by Doctor Name" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    bool found = false;

    if (choice == 1) {
        int apptID;
        cout << "Enter Appointment ID: ";
        cin >> apptID;
        for (int i = 0; i < numberOfAppointments; i++) {
            if (appointmentArray[i].getPatientID() == patientID && appointmentArray[i].getRecordID() == apptID) {
                appointmentArray[i].displayRecord();
                found = true;
            }
        }
    } else if (choice == 2) {
        char doctor[MAX_NAME_LEN];
        cin.ignore(1000, '\n');
        cout << "Enter Doctor Name (or part of it): ";
        cin.getline(doctor, MAX_NAME_LEN);
        for (int i = 0; i < numberOfAppointments; i++) {
            if (appointmentArray[i].getPatientID() == patientID && strstr(appointmentArray[i].getDoctorName(), doctor) != NULL) {
                appointmentArray[i].displayRecord();
                found = true;
            }
        }
    } else {
        throw InputException("Invalid search option.");
    }

    if (found == false) {
        throw InputException("No matching bookings found.");
    }
}

// ---- Sort Records (2 criteria: by Date, by Doctor Name) ----
// This is a SEPARATE Selection Sort from selectionSortPatients() in Stage 2,
// because it sorts an array of Appointment objects instead of Patient
// objects - same algorithm idea, different data type.
void selectionSortAppointments(Appointment appointmentArray[], int totalAppointments, int criteria) {
    for (int i = 0; i < totalAppointments - 1; i++) {
        int bestIndex = i;

        for (int j = i + 1; j < totalAppointments; j++) {
            bool jIsBetter = false;

            if (criteria == 1) {
                // sort by date: turn each date into one comparable number,
                // e.g. 2026-06-25 becomes 20260625, so smaller number = earlier date
                Date dj = appointmentArray[j].getApptDate();
                Date db = appointmentArray[bestIndex].getApptDate();
                int valJ = dj.year * 10000 + dj.month * 100 + dj.day;
                int valB = db.year * 10000 + db.month * 100 + db.day;
                if (valJ < valB) {
                    jIsBetter = true;
                }
            } else if (criteria == 2) {
                if (strcmp(appointmentArray[j].getDoctorName(), appointmentArray[bestIndex].getDoctorName()) < 0) {
                    jIsBetter = true;
                }
            }

            if (jIsBetter) {
                bestIndex = j;
            }
        }

        if (bestIndex != i) {
            Appointment temp = appointmentArray[i];
            appointmentArray[i] = appointmentArray[bestIndex];
            appointmentArray[bestIndex] = temp;
        }
    }
}

void sortOwnAppointmentsMenu(Appointment appointmentArray[], int numberOfAppointments, int patientID) {
    int choice;
    cout << "\n-- Sort My Bookings --" << endl;
    cout << "1. Sort by Date" << endl;
    cout << "2. Sort by Doctor Name" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    if (choice != 1 && choice != 2) {
        throw InputException("Invalid sort option.");
    }

    // pull out only THIS patient's own bookings into a separate temporary
    // array before sorting, so we never disturb other patients' bookings
    Appointment temp[MAX_APPOINTMENTS];
    int total = 0;
    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getPatientID() == patientID) {
            temp[total] = appointmentArray[i];
            total++;
        }
    }

    if (total == 0) {
        cout << "You have no bookings to sort." << endl;
        return;
    }

    selectionSortAppointments(temp, total, choice);

    cout << "\n-- Sorted Result --" << endl;
    for (int i = 0; i < total; i++) {
        temp[i].displayRecord();
    }
}

// ---- Summary Report (display + save to txt + read back from txt) ----
void generateAndSaveBookingSummary(const Patient& patient, Appointment appointmentArray[], int numberOfAppointments, const char* filename) {
    int total = 0, pending = 0, confirmed = 0, completed = 0, cancelled = 0;

    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getPatientID() == patient.getPatientID()) {
            total++;
            if (strcmp(appointmentArray[i].getStatus(), "Pending") == 0) pending++;
            else if (strcmp(appointmentArray[i].getStatus(), "Confirmed") == 0) confirmed++;
            else if (strcmp(appointmentArray[i].getStatus(), "Completed") == 0) completed++;
            else if (strcmp(appointmentArray[i].getStatus(), "Cancelled") == 0) cancelled++;
        }
    }

    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open summary report file for writing.");
    }

    outFile << "===== BOOKING SUMMARY FOR " << patient.getName() << " =====\n";
    outFile << "Total Bookings : " << total << "\n";
    outFile << "Pending        : " << pending << "\n";
    outFile << "Confirmed      : " << confirmed << "\n";
    outFile << "Completed      : " << completed << "\n";
    outFile << "Cancelled      : " << cancelled << "\n";
    outFile << "=====================================\n";
    outFile.close();

    cout << "===== BOOKING SUMMARY FOR " << patient.getName() << " =====" << endl;
    cout << "Total Bookings : " << total << endl;
    cout << "Pending        : " << pending << endl;
    cout << "Confirmed      : " << confirmed << endl;
    cout << "Completed      : " << completed << endl;
    cout << "Cancelled      : " << cancelled << endl;
    cout << "=====================================" << endl;
}

void customerSummaryReportMenu(const Patient& patient, Appointment appointmentArray[], int numberOfAppointments) {
    generateAndSaveBookingSummary(patient, appointmentArray, numberOfAppointments, FILE_SUMMARY);
    cout << "\nView the saved report from file as well? (Y/N): ";
    char ans;
    cin >> ans;
    if (ans == 'Y' || ans == 'y') {
        loadAndDisplaySummary(FILE_SUMMARY);
    }
}

// ---- Main Customer menu loop (shown after a successful login) ----
void customerMainMenu(Patient& currentPatient, Appointment appointmentArray[], int& numberOfAppointments) {
    int choice = -1;

    do {
        cout << "\n========== CUSTOMER MENU (" << currentPatient.getName() << ") ==========" << endl;
        cout << "1. View My Profile" << endl;
        cout << "2. Book New Appointment" << endl;
        cout << "3. Edit My Appointment" << endl;
        cout << "4. Cancel My Appointment" << endl;
        cout << "5. Display My Bookings" << endl;
        cout << "6. Search My Bookings" << endl;
        cout << "7. Sort My Bookings" << endl;
        cout << "8. Generate / View Booking Summary Report" << endl;
        cout << "0. Logout" << endl;
        cout << "Enter choice: ";

        // same try/catch pattern as the Staff module: catch errors here so
        // one bad input never crashes the whole program
        try {
            cin >> choice;

            switch (choice) {
                case 1: displayMyProfile(currentPatient); break;
                case 2: bookAppointment(appointmentArray, numberOfAppointments, MAX_APPOINTMENTS, currentPatient.getPatientID()); break;
                case 3: editOwnAppointment(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 4: cancelOwnAppointment(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 5: displayOwnAppointmentsMenu(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 6: searchOwnAppointmentsMenu(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 7: sortOwnAppointmentsMenu(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 8: customerSummaryReportMenu(currentPatient, appointmentArray, numberOfAppointments); break;
                case 0: cout << "Logging out..." << endl; break;
                default: cout << "Invalid choice, please try again." << endl;
            }
        } catch (InputException& e) {
            cout << "Input error: " << e.what() << endl;
        } catch (FileException& e) {
            cout << "File error: " << e.what() << endl;
        }

    } while (choice != 0);
}

// ---- Entry point for the whole Customer/Patient module (Register or Login) ----
void customerModuleEntry(PatientLinkedList& patientList, Appointment appointmentArray[], int& numberOfAppointments) {
    int choice;
    cout << "\n========== CUSTOMER / PATIENT MODULE ==========" << endl;
    cout << "1. Register new patient account" << endl;
    cout << "2. Login" << endl;
    cout << "3. Back to main menu" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        registerPatient(patientList);
    }

    if (choice == 1 || choice == 2) {
        Patient* loggedInPatient = patientLoginPrompt(patientList);
        if (loggedInPatient != NULL) {
            appendLoginLog(loggedInPatient->getUsername(), "Patient", "LOGIN");
            customerMainMenu(*loggedInPatient, appointmentArray, numberOfAppointments);
            appendLoginLog(loggedInPatient->getUsername(), "Patient", "LOGOUT");
        } else {
            cout << "Login failed. Returning to main menu." << endl;
        }
    } else {
        cout << "Returning to main menu." << endl;
    }
}


int main() {

    PatientLinkedList patientList;
    patientList.insertAtEnd(Patient(1001, "Ahmad Bin Ali", "990101-10-1234", 35, 'M',
                                      "ahmad99", "pass123", Date(15, 3, 2026), 5, "Fever", "Dr. Tan"));
    patientList.insertAtEnd(Patient(1002, "Siti Aminah", "950505-08-5678", 30, 'F',
                                      "siti95", "pass456", Date(16, 3, 2026), 3, "Flu", "Dr. Lee"));

    Appointment appointmentArray[MAX_APPOINTMENTS];
    int numberOfAppointments = 0;
    appointmentArray[numberOfAppointments] = Appointment(3001, 1001, "Dr. Tan", Date(20, 3, 2026), "09:00AM", "Follow-up checkup");
    numberOfAppointments++;

    // outer safety-net try/catch, same pattern as Stage 3's main()
    try {
        customerModuleEntry(patientList, appointmentArray, numberOfAppointments);
    } catch (FileException& e) {
        cout << "File error: " << e.what() << endl;
    } catch (InputException& e) {
        cout << "Input error: " << e.what() << endl;
    }

    return 0;
}
