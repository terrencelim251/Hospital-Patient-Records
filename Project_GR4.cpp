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


bool verifyStaffLogin(const StaffMember& s, const char* userInput, const char* passInput) {
    return (strcmp(s.username, userInput) == 0 && strcmp(s.password, passInput) == 0);
}


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





int main() {

    PatientLinkedList patientList;

    Date d1(15, 3, 2026);
    Patient p1(1001, "Ahmad Bin Ali", "990101-10-1234", 35, 'M',
               "ahmad99", "pass123", d1, 5, "Fever", "Dr. Tan");

    Date d2(16, 3, 2026);
    Patient p2(1002, "Siti Aminah", "950505-08-5678", 30, 'F',
               "siti95", "pass456", d2, 3, "Flu", "Dr. Lee");

    patientList.insertAtEnd(p1);
    patientList.insertAtEnd(p2);

    cout << "\nAll patients in linked list:" << endl;
    patientList.displayAll();

    cout << "\nTotal patients: " << patientList.getCount() << endl;

    printPatientBillingSlip(p1);
    return 0;
}