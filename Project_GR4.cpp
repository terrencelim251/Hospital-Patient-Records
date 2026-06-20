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
const int MAX_PRESCRIPTIONS = 200;   // max number of prescriptions the system can hold at once

const int WARD_CAPACITY = 4;     // each ward can only fit 4 patients at the same time
const int TOTAL_WARDS = 10;      // ward numbers go from 1 to 10
const int TOTAL_DOCTORS = 6;     // how many doctors are in our schedule list
const int TOTAL_MEDICINE_TYPES = 8;  // how many different medicines the pharmacy keeps

// File names for the .txt files
const char FILE_PATIENTS[]     = "patients.txt";       // all patient records
const char FILE_STAFF[]        = "staff.txt";           // all staff/admin records
const char FILE_APPOINTMENTS[] = "appointments.txt";    // all appointment records
const char FILE_LOGIN_LOG[]    = "login_log.txt";       // login/logout activity log
const char FILE_SUMMARY[]      = "summary_report.txt";  // saved summary reports
const char FILE_PRESCRIPTIONS[]  = "prescriptions.txt";   // all prescription records
const char FILE_BILL[]           = "patient_bill.txt";     // the most recently generated bill
const char FILE_MEDICINE_STOCK[] = "medicine_stock.txt";  // pharmacy stock count


struct Date {
    int day;
    int month;
    int year;

    // Default constructor
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


  //For both Patient and StaffMember need a phone number and address, 

struct ContactInfo {
    char phone[20];
    char address[MAX_TEXT_LEN];

    ContactInfo() {
        strcpy(phone, "N/A");
        strcpy(address, "N/A");
    }
};

	// Use Person class as inheritance class for patient and staff as they use same parameter
class Person {
protected:
    // protected (not private) on purpose: child classes need direct access,
    // but outside code/classes still cannot touch these fields directly
    char   name[MAX_NAME_LEN];
    char   icNumber[14];     
    int    age;
    char   gender;
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
        cout << "Name: " << name << " | IC: " << icNumber<< " | Age: " << age << " | Gender: " << gender << endl;
    }

    // Simple getter functions used by other parts of the system like login check
    const char* getUsername() 
	const { return username; }
	
    const char* getPassword() 
	const { return password; }
	
    const char* getName()     
	const { return name; }

    // Added these getters below so the file-saving functions can
    // read every field of a Person (Patient/StaffMember) when writing it
    // out to a .txt file.
    const char* getIC()         
	const { return icNumber; }
	
    int getAge()       
	const { return age; }
	
    char getGenderChar() 
	const { return gender; }
	
    const char* getPhone()      
	const { return contact.phone; }
	
    const char* getAddress()    
	const { return contact.address; }

    // Added this setter because the constructors above do not take
    // phone/address as parameters. After loading a Patient/StaffMember back
    // from a text file, we use this to fill in the information back to the contact info.
    void setContact(const char* phone, const char* address) {
        strcpy(contact.phone, phone);
        strcpy(contact.address, address);
    }
};

class MedicalRecordBase {
protected:
    int  recordID;
    Date createdDate;
    char status[20];

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
    }

    virtual void displayRecord() const {
        char dateStr[15];
        createdDate.toString(dateStr);
        cout << "Record ID: " << recordID << " | Date: " << dateStr
             << " | Status: " << status << endl;
    }

    int  getRecordID() const { return recordID; }
    Date getCreatedDate() const { return createdDate; }
    const char* getStatus() const { return status; }
    void setStatus(const char* st) { strcpy(status, st); }
};


  //Derived class 1: Patient (inherits from Person)
  
class Patient : public Person {
private:
    int  patientID;
    Date admissionDate;
    int  wardNumber;
    char diagnosis[MAX_TEXT_LEN];
    char assignedDoctor[MAX_NAME_LEN];
    char patientStatus[20];

public:
    // Constructor 1 (default)
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

    // Destructor
    ~Patient() {
        // Patient does not hold any extra dynamic memory of its own right now,
        // but we still write the destructor to show it is called
        // correctly in the inheritance chain.
    }

    // Overriding the base class virtual function displayInfo()
    void displayInfo() const {
        char dateStr[15];
        admissionDate.toString(dateStr);
        cout << "[Patient] ID:" << patientID << " | Name:" << name
             << " | Age:" << age << " | Gender:" << gender
             << " | Ward:" << wardNumber << " | Doctor:" << assignedDoctor
             << " | Admitted:" << dateStr << " | Status:" << patientStatus
             << endl;
    }

    // Used later by LinkedList, sorting, searching, file I/O 
    int  getPatientID()      
	const { return patientID; }
	
    int  getWardNumber()         
	const { return wardNumber; }
	
    Date getAdmissionDate()  
	const { return admissionDate; }
	
    const char* getDiagnosis()      
	const { return diagnosis; }
	
    const char* getAssignedDoctor() 
	const { return assignedDoctor; }
	
    const char* getPatientStatus()  
	const { return patientStatus; }

    void setDiagnosis(const char* d)      
	{ strcpy(diagnosis, d); }
	
    void setAssignedDoctor(const char* d) 
	{ strcpy(assignedDoctor, d); }
	
    void setWardNumber(int w)                 
	{ wardNumber = w; }
	
    void setPatientStatus(const char* s)  
	{ strcpy(patientStatus, s); }

    // friend function declaration
    friend void printPatientBillingSlip(const Patient& p);

    // friend function declaration - needs access to
    // both Patient's private wardNumber AND StaffMember's private role,
    // so it must be declared friend in both classes)
    friend bool transferPatientWard(Patient& p, const class StaffMember& approver, int newWard);
};


class StaffMember : public Person {
private:
    int  staffID;
    char role[20];
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

    int  getStaffID() 
	const { return staffID; }
	
    const char* getRole() 
	const { return role; }

    // Added this getter so the file-saving function can read the
    // join date when writing a StaffMember out to staff.txt
    Date getDateJoined() const { return dateJoined; }

    // friend function declaration
    friend bool verifyStaffLogin(const StaffMember& s, const char* userInput, const char* passInput);

    // friend function declaration, declared in both
    // Patient and StaffMember because it touches private data
    friend bool transferPatientWard(class Patient& p, const StaffMember& approver, int newWard);
};

/*
  Derived class 3: Appointment (inherits from MedicalRecordBase
  Represents a patient's appointment booking. This is the link between the
  Customer module and the Staff module. When a patient (Customer) books an
  appointment, it is saved into appointments.txt, and Staff/Admin reads the
  same file to ensure data consistency
*/
class Appointment : public MedicalRecordBase {
private:
    int  patientID;
    char doctorName[MAX_NAME_LEN];
    Date appointmentDate;
    char timeSlot[10];  
    char reason[MAX_TEXT_LEN];
    char dayOfWeekText[15]; 

public:
    Appointment() : MedicalRecordBase() {
        patientID = 0;
        strcpy(doctorName, "N/A");
        strcpy(timeSlot, "N/A");
        strcpy(reason, "N/A");
        strcpy(dayOfWeekText, "Unknown");
    }

    Appointment(int recID, int patID, const char* doctor, Date apptDate,
                const char* time, const char* r)
        : MedicalRecordBase(recID, apptDate, "Pending") {
        patientID = patID;
        strcpy(doctorName, doctor);
        appointmentDate = apptDate;
        strcpy(timeSlot, time);
        strcpy(reason, r);
        strcpy(dayOfWeekText, "Unknown");
    }

    ~Appointment() {
    }

    // Override displayRecord()
    void displayRecord() const {
        char dateStr[15];
        appointmentDate.toString(dateStr);
        cout << "[Appointment] ID:" << recordID << " | PatientID:" << patientID
             << " | Doctor:" << doctorName << " | Date:" << dateStr
             << " (" << dayOfWeekText << ")"
             << " | Time:" << timeSlot << " | Reason:" << reason
             << " | Status:" << status << endl;
    }

    int  getPatientID()  const { return patientID; }
    Date getApptDate()   const { return appointmentDate; }
    const char* getDoctorName() const { return doctorName; }
    const char* getTimeSlot()   const { return timeSlot; }
    const char* getReason()     const { return reason; }
    const char* getDayOfWeekText() const { return dayOfWeekText; }

    // Added these two setters so a Customer can edit their own
    // appointment to change the date/time, or change the reason for visit
    void setApptDateTime(Date newDate, const char* newTime) {
        appointmentDate = newDate;
        strcpy(timeSlot, newTime);
    }
    void setReason(const char* newReason) {
        strcpy(reason, newReason);
    }

    // To lets us remember which day of the week this
    // appointment falls on, so we can check against the doctor's schedule
    void setDayOfWeekText(const char* newDay) {
        strcpy(dayOfWeekText, newDay);
    }

    // friend function declaration
    friend void printAppointmentSlip(const Appointment& a);
};

/*
  (struct DoctorSchedule)
  This just holds which days a doctor works, and how many patients they can
  see in one day. We use this to stop a Customer from booking a doctor on a
  day that doctor does not even work, and to stop too many people booking
  the same doctor on the same day.
*/
struct DoctorSchedule {
    char doctorName[MAX_NAME_LEN];
    char workingDays[60];
    int  maxPatientsPerDay;
    double consultationFee;
};

struct MedicineStock {
    char medicineName[MAX_NAME_LEN];
    int  stockQuantity;
    double pricePerUnit;
};

/*
  (class Prescription)
  This class inherits from MedicalRecordBase just like
  Appointment does, so it also gets a recordID, createdDate, and status. 
  It represents one medicine that a doctor gave to a patient.
*/
class Prescription : public MedicalRecordBase {
private:
    int  patientID;
    char medicineName[MAX_NAME_LEN];
    char dosageText[30];       
    char frequencyText[30]; 
    char prescribedByDoctor[MAX_NAME_LEN];
    int  quantityPrescribed;
    double pricePerUnitAtTime;    
    
public:
    Prescription() : MedicalRecordBase() {
        patientID = 0;
        strcpy(medicineName, "N/A");
        strcpy(dosageText, "N/A");
        strcpy(frequencyText, "N/A");
        strcpy(prescribedByDoctor, "N/A");
        quantityPrescribed = 0;
        pricePerUnitAtTime = 0.0;
    }

    Prescription(int recID, int patID, const char* medName, const char* doseText,
                  const char* freqText, const char* doctorName, int qty, Date prescribedDate,
                  double pricePerUnit)
        : MedicalRecordBase(recID, prescribedDate, "Active") {
        patientID = patID;
        strcpy(medicineName, medName);
        strcpy(dosageText, doseText);
        strcpy(frequencyText, freqText);
        strcpy(prescribedByDoctor, doctorName);
        quantityPrescribed = qty;
        pricePerUnitAtTime = pricePerUnit;
    }

    ~Prescription() {
    }

    void displayRecord() const {
        char dateStr[15];
        Date d = getCreatedDate();
        d.toString(dateStr);
        cout << "[Prescription] ID:" << recordID << " | PatientID:" << patientID
             << " | Medicine:" << medicineName << " | Dosage:" << dosageText
             << " | Frequency:" << frequencyText << " | Doctor:" << prescribedByDoctor
             << " | Quantity:" << quantityPrescribed << " | Date:" << dateStr
             << " | Status:" << status << endl;
    }

    int  getPatientID() const { return patientID; }
    const char* getMedicineName() const { return medicineName; }
    const char* getDosageText() const { return dosageText; }
    const char* getFrequencyText() const { return frequencyText; }
    const char* getPrescribedByDoctor() const { return prescribedByDoctor; }
    double getPricePerUnit() const { return pricePerUnitAtTime; }
    int  getQuantityPrescribed() const { return quantityPrescribed; }
};

/*
  (PatientNode)
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
  (PatientLinkedList)
  Manages the whole linked list: adding nodes, removing nodes, displaying all
  patients, counting patients, etc. This uses new / delete to manage memory.
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
            delete temp;   //dynamic memory release (delete)
        }
        head = NULL;
    }

    // Add a new patient to the end of the list
    void insertAtEnd(const Patient& p) {
        PatientNode* newNode = new PatientNode(p);   //dynamic memory allocation (new)
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
                delete current;   //dynamic memory release (delete)
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

    // Added this: search by username instead of ID, used for
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

    // Selection Sort and Binary Search both work on arrays using index numbers
    // (arr[0], arr[1], ...), which is much simpler and faster than walking a
    // linked list step by step. So our strategy is to keep the data in a linked
    // list for everyday add/remove operations, but copy it into an array
    // whenever we need to sort or search.
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
    cout << "Patient ID  : " << p.patientID << endl; 
    cout << "Name        : " << p.name << endl;        
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
    p.wardNumber = newWard;   // direct write to Patient's private member, it was allowed because friend
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
  Binary Search only works correctly on an array that is already sorted by
  the same field we are searching on. We keep cutting the search range in
  half: check the middle element, if it's too small, move the lower bound
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
            return mid;         
        } else if (midID < targetID) {
            low = mid + 1;          // target must be in the right half
        } else {
            high = mid - 1;         // target must be in the left half
        }
    }
    return -1;   // not found
}


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

// PATIENTS file (patients.txt)
// One line per patient, fields separated by '|':
// id|name|ic|age|gender|username|password|phone|address|admDay|admMonth|admYear|ward|diagnosis|doctor|status
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
        // To avoid crashing even if the file does not exist any data yet
        return;
    }

    int corruptedLineCount = 0;
    char line[600];
    while (inFile.getline(line, 600)) {
        if (strlen(line) == 0) continue;   // skip blank lines

        // we read every field one at a time, and check right away if the
        // line actually had that many fields. if a field is missing
        // (result comes back NULL) we skip this whole line instead of
        // crashing - this protects us from old or corrupted data files
        char* token;
        bool lineIsGood = true;

        token = strtok(line, "|");
        if (token == NULL) { lineIsGood = false; }
        int id = 0;
        if (lineIsGood == true) { id = atoi(token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char name[MAX_NAME_LEN];
        if (lineIsGood == true) { strcpy(name, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char ic[20];
        if (lineIsGood == true) { strcpy(ic, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        int age = 0;
        if (lineIsGood == true) { age = atoi(token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char gender = 'N';
        if (lineIsGood == true) { gender = token[0]; }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char username[MAX_NAME_LEN];
        if (lineIsGood == true) { strcpy(username, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char password[MAX_PASS_LEN];
        if (lineIsGood == true) { strcpy(password, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char phone[20];
        if (lineIsGood == true) { strcpy(phone, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char address[MAX_TEXT_LEN];
        if (lineIsGood == true) { strcpy(address, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        int admDay = 1;
        if (lineIsGood == true) { admDay = atoi(token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        int admMonth = 1;
        if (lineIsGood == true) { admMonth = atoi(token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        int admYear = 2026;
        if (lineIsGood == true) { admYear = atoi(token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        int ward = 0;
        if (lineIsGood == true) { ward = atoi(token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char diagnosis[MAX_TEXT_LEN];
        if (lineIsGood == true) { strcpy(diagnosis, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char doctor[MAX_NAME_LEN];
        if (lineIsGood == true) { strcpy(doctor, token); }

        token = strtok(NULL, "|");
        if (token == NULL) { lineIsGood = false; }
        char pStatus[20];
        if (lineIsGood == true) { strcpy(pStatus, token); }

        if (lineIsGood == false) {
            corruptedLineCount = corruptedLineCount + 1;
            continue;
        }

        Date admDate(admDay, admMonth, admYear);
        Patient p(id, name, ic, age, gender, username, password,
                   admDate, ward, diagnosis, doctor);
        p.setContact(phone, address);
        p.setPatientStatus(pStatus);

        list.insertAtEnd(p);
    }
    inFile.close();

    if (corruptedLineCount > 0) {
        cout << "Warning: skipped " << corruptedLineCount
             << " corrupted line(s) in " << filename << endl;
    }
}

// STAFF file (staff.txt)
// Format: id|name|ic|age|gender|username|password|phone|address|role|joinDay|joinMonth|joinYear
// Staff records are kept in a plain array (not a linked list) 
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
                << d.day << "|" << d.month << "|" << d.year << "\n";
    }
    outFile.close();
}

int loadStaffFromFile(StaffMember staffArray[], int maxSize, const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return 0;   // no staff file yet
    }

    char line[600];
    int count = 0;
    int corruptedLineCount = 0;
    while (inFile.getline(line, 600) && count < maxSize) {
        if (strlen(line) == 0) continue;

        // check every field for NULL right after reading it, and skip the
        // whole line if any field is missing - this stops a damaged or
        // old-format line from crashing the program
        char* token;
        token = strtok(line, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int id = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char name[MAX_NAME_LEN]; strcpy(name, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char ic[20]; strcpy(ic, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int age = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char gender = token[0];

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char username[MAX_NAME_LEN]; strcpy(username, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char password[MAX_PASS_LEN]; strcpy(password, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char phone[20]; strcpy(phone, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char address[MAX_TEXT_LEN]; strcpy(address, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char role[20]; strcpy(role, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int jDay = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int jMonth = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int jYear = atoi(token);

        Date joined(jDay, jMonth, jYear);
        StaffMember s(id, name, ic, age, gender, username, password, role, joined);
        s.setContact(phone, address);

        staffArray[count] = s;
        count++;
    }
    inFile.close();

    if (corruptedLineCount > 0) {
        cout << "Warning: skipped " << corruptedLineCount
             << " corrupted line(s) in " << filename << endl;
    }

    return count;
}

// APPOINTMENTS file (appointments.txt)
// Format: recordID|patientID|doctorName|day|month|year|timeSlot|reason|status
// This is the file BOTH modules read/write - when a Customer books an
// appointment it goes in here, and Staff reads the same file
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
                << appointmentArray[i].getStatus() << "|"
                << appointmentArray[i].getDayOfWeekText() << "\n";
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
    int corruptedLineCount = 0;
    while (inFile.getline(line, 600) && count < maxSize) {
        if (strlen(line) == 0) continue;

        char* token;
        token = strtok(line, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int recID = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int patID = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char doctor[MAX_NAME_LEN]; strcpy(doctor, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int day = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int month = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int year = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char timeSlot[10]; strcpy(timeSlot, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char reason[MAX_TEXT_LEN]; strcpy(reason, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char status[20]; strcpy(status, token);

        // older saved files might not have a day-of-week field, so we check
        // before reading it, to be safe
        char dayOfWeek[15];
        token = strtok(NULL, "|");
        if (token != NULL) {
            strcpy(dayOfWeek, token);
        } else {
            strcpy(dayOfWeek, "Unknown");
        }

        Date apptDate(day, month, year);
        Appointment a(recID, patID, doctor, apptDate, timeSlot, reason);
        a.setStatus(status);
        a.setDayOfWeekText(dayOfWeek);

        appointmentArray[count] = a;
        count++;
    }
    inFile.close();

    if (corruptedLineCount > 0) {
        cout << "Warning: skipped " << corruptedLineCount
             << " corrupted line(s) in " << filename << endl;
    }

    return count;
}

// LOGIN LOG file (login_log.txt)
// Every login/logout appends ONE new line - we never overwrite this file,
// so it keeps growing into a full history this is why we open it in
// ios::app "append" mode instead of the normal write mode.
void appendLoginLog(const char* username, const char* role, const char* action) {
    ofstream logFile(FILE_LOGIN_LOG, ios::app);
    if (!logFile.is_open()) {
        throw FileException("Could not open login log file.");
    }

    time_t now = time(0);          // get the current system time
    char* timeText = ctime(&now);  // turn it into readable text, like "Thu Jun 18 ..."

    // ctime() puts a '\n' at the end of the string by default - remove it so our log line stays on a single line
    int len = strlen(timeText);
    if (len > 0 && timeText[len - 1] == '\n') {
        timeText[len - 1] = '\0';
    }

    logFile << "[" << timeText << "] " << username << " (" << role << ") - " << action << "\n";
    logFile.close();
}


void generateAndSavePatientSummary(const PatientLinkedList& list, const char* filename) {
    Patient temp[MAX_PATIENTS];
    int total = list.copyToArray(temp, MAX_PATIENTS);

    // There are now 3 possible statuses, not just two -
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

// ---------------------------------------------------------------------------
// Small helper used everywhere we read a text field with cin.getline().
// If the user just presses Enter and leaves a field blank, strtok() later
// treats "||" (two pipes with nothing between them) as ONE delimiter, not
// an empty field - this silently shifts every field after it and breaks
// the save/load format. To stop this from ever happening, we replace any
// truly empty field with "N/A" right after reading it.
// ---------------------------------------------------------------------------
void fillInIfEmpty(char* field, const char* defaultValue) {
    if (strlen(field) == 0) {
        strcpy(field, defaultValue);
    }
}


// BONUS FEATURE 1: Ward Capacity Management
// Each ward can only hold WARD_CAPACITY patients at the same time. This
// function just counts how many "Admitted" patients are currently in one
// specific ward, so we can check it before letting a new patient in.
int countPatientsInWard(PatientLinkedList& patientList, int wardNumber) {
    Patient patientArray[MAX_PATIENTS];
    int totalPatients = patientList.copyToArray(patientArray, MAX_PATIENTS);

    int patientsInThisWard = 0;

    for (int i = 0; i < totalPatients; i++) {
        if (patientArray[i].getWardNumber() == wardNumber) {
            // only count patients who are actually staying in the ward right now, not ones who already registered but are not admitted yet
            if (strcmp(patientArray[i].getPatientStatus(), "Admitted") == 0) {
                patientsInThisWard = patientsInThisWard + 1;
            }
        }
    }

    return patientsInThisWard;
}

// BONUS FEATURE 2: Doctor Scheduling and Availability
// We made up a fixed schedule for our 6 doctors: which days they work, and
// how many patients they can see per day. A Customer can only book a doctor
// on a day that doctor actually works, and only if there is still space.


// this function fills in the doctor schedule list with our made-up data.
// it does not load from a file because the schedule does not really change
void setupDoctorScheduleList(DoctorSchedule doctorScheduleList[]) {
    strcpy(doctorScheduleList[0].doctorName, "Dr. Tan");
    strcpy(doctorScheduleList[0].workingDays, "Monday,Wednesday,Friday");
    doctorScheduleList[0].maxPatientsPerDay = 10;
    doctorScheduleList[0].consultationFee = 80.00;

    strcpy(doctorScheduleList[1].doctorName, "Dr. Lee");
    strcpy(doctorScheduleList[1].workingDays, "Tuesday,Thursday");
    doctorScheduleList[1].maxPatientsPerDay = 8;
    doctorScheduleList[1].consultationFee = 70.00;

    strcpy(doctorScheduleList[2].doctorName, "Dr. Lim");
    strcpy(doctorScheduleList[2].workingDays, "Monday,Tuesday,Wednesday,Thursday,Friday");
    doctorScheduleList[2].maxPatientsPerDay = 15;
    doctorScheduleList[2].consultationFee = 60.00;

    strcpy(doctorScheduleList[3].doctorName, "Dr. Kumar");
    strcpy(doctorScheduleList[3].workingDays, "Wednesday,Friday");
    doctorScheduleList[3].maxPatientsPerDay = 6;
    doctorScheduleList[3].consultationFee = 100.00;

    strcpy(doctorScheduleList[4].doctorName, "Dr. Wong");
    strcpy(doctorScheduleList[4].workingDays, "Monday,Tuesday");
    doctorScheduleList[4].maxPatientsPerDay = 8;
    doctorScheduleList[4].consultationFee = 90.00;

    strcpy(doctorScheduleList[5].doctorName, "Dr. Farah");
    strcpy(doctorScheduleList[5].workingDays, "Thursday,Friday,Saturday");
    doctorScheduleList[5].maxPatientsPerDay = 10;
    doctorScheduleList[5].consultationFee = 75.00;
}

void displayDoctorScheduleList(DoctorSchedule doctorScheduleList[]) {
    cout << "\n-- Doctors Available --" << endl;
    for (int i = 0; i < TOTAL_DOCTORS; i++) {
        cout << (i + 1) << ". " << doctorScheduleList[i].doctorName
             << " (works on: " << doctorScheduleList[i].workingDays
             << " | max " << doctorScheduleList[i].maxPatientsPerDay
             << " patients per day | consultation fee: RM" << doctorScheduleList[i].consultationFee
             << ")" << endl;
    }
}

// checks if a day name like "Monday" appears inside the doctor's working
// days text. since none of our day names are part of another day name, a
// simple strstr check is safe to use here
bool isDoctorWorkingOnThisDay(DoctorSchedule oneDoctorSchedule, const char* dayOfWeekText) {
    if (strstr(oneDoctorSchedule.workingDays, dayOfWeekText) != NULL) {
        return true;
    }
    return false;
}

// counts how many appointments are already booked for one doctor on one
// exact date, so we know if that doctor is fully booked already or noy
int countAppointmentsForDoctorOnDate(Appointment appointmentArray[], int numberOfAppointments,
                                       const char* doctorName, Date theDate) {
    int bookedCount = 0;

    for (int i = 0; i < numberOfAppointments; i++) {
        bool sameDoctor = false;
        if (strcmp(appointmentArray[i].getDoctorName(), doctorName) == 0) {
            sameDoctor = true;
        }

        bool sameDate = false;
        Date apptDate = appointmentArray[i].getApptDate();
        if (apptDate.day == theDate.day && apptDate.month == theDate.month && apptDate.year == theDate.year) {
            sameDate = true;
        }

        // only count appointments that are not already cancelled
        bool notCancelled = true;
        if (strcmp(appointmentArray[i].getStatus(), "Cancelled") == 0) {
            notCancelled = false;
        }

        if (sameDoctor == true && sameDate == true && notCancelled == true) {
            bookedCount = bookedCount + 1;
        }
    }

    return bookedCount;
}

// BONUS FEATURE 3: Prescription and Medicine Stock Tracking
// This part lets a doctor give a patient a prescription, and it takes the
// medicine away from the pharmacy stock count, just like a real pharmacy.

void saveMedicineStockToFile(MedicineStock medicineStockArray[], int totalMedicineTypes, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open medicine stock file for writing.");
    }
    for (int i = 0; i < totalMedicineTypes; i++) {
        outFile << medicineStockArray[i].medicineName << "|"
                << medicineStockArray[i].stockQuantity << "|"
                << medicineStockArray[i].pricePerUnit << "\n";
    }
    outFile.close();
}

int loadMedicineStockFromFile(MedicineStock medicineStockArray[], int maxArraySize, const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return 0;
    }

    char line[200];
    int count = 0;
    int corruptedLineCount = 0;
    while (inFile.getline(line, 200) && count < maxArraySize) {
        if (strlen(line) == 0) continue;

        char* token;
        token = strtok(line, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        strcpy(medicineStockArray[count].medicineName, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        medicineStockArray[count].stockQuantity = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        medicineStockArray[count].pricePerUnit = atof(token);

        count++;
    }
    inFile.close();

    if (corruptedLineCount > 0) {
        cout << "Warning: skipped " << corruptedLineCount
             << " corrupted line(s) in " << filename << endl;
    }

    return count;
}

// this function tries to load the medicine stock from file first. if there
// is no file or first time running the program, it makes a starting
// stock list by hand and saves it
void setupMedicineStock(MedicineStock medicineStockArray[]) {
    int loadedCount = loadMedicineStockFromFile(medicineStockArray, TOTAL_MEDICINE_TYPES, FILE_MEDICINE_STOCK);

    if (loadedCount > 0) {
        return;
    }

    strcpy(medicineStockArray[0].medicineName, "Panadol");
    medicineStockArray[0].stockQuantity = 100;
    medicineStockArray[0].pricePerUnit = 0.50;

    strcpy(medicineStockArray[1].medicineName, "Amoxicillin");
    medicineStockArray[1].stockQuantity = 80;
    medicineStockArray[1].pricePerUnit = 1.20;

    strcpy(medicineStockArray[2].medicineName, "Antihistamine");
    medicineStockArray[2].stockQuantity = 60;
    medicineStockArray[2].pricePerUnit = 0.80;

    strcpy(medicineStockArray[3].medicineName, "Insulin");
    medicineStockArray[3].stockQuantity = 40;
    medicineStockArray[3].pricePerUnit = 15.00;

    strcpy(medicineStockArray[4].medicineName, "Aspirin");
    medicineStockArray[4].stockQuantity = 90;
    medicineStockArray[4].pricePerUnit = 0.30;

    strcpy(medicineStockArray[5].medicineName, "Cough Syrup");
    medicineStockArray[5].stockQuantity = 50;
    medicineStockArray[5].pricePerUnit = 5.50;

    strcpy(medicineStockArray[6].medicineName, "Vitamin C");
    medicineStockArray[6].stockQuantity = 120;
    medicineStockArray[6].pricePerUnit = 0.40;

    strcpy(medicineStockArray[7].medicineName, "Ibuprofen");
    medicineStockArray[7].stockQuantity = 70;
    medicineStockArray[7].pricePerUnit = 0.60;

    saveMedicineStockToFile(medicineStockArray, TOTAL_MEDICINE_TYPES, FILE_MEDICINE_STOCK);
}

const int LOW_STOCK_THRESHOLD = 20;   // if stock drops below this, we show a warning

void displayMedicineStockList(MedicineStock medicineStockArray[]) {
    cout << "\n-- Pharmacy Stock --" << endl;
    for (int i = 0; i < TOTAL_MEDICINE_TYPES; i++) {
        cout << (i + 1) << ". " << medicineStockArray[i].medicineName
             << " (Stock left: " << medicineStockArray[i].stockQuantity
             << ", Price: RM" << medicineStockArray[i].pricePerUnit << " each)";

        // check stock level one step at a time instead of one big condition
        bool isLowStock = false;
        if (medicineStockArray[i].stockQuantity < LOW_STOCK_THRESHOLD) {
            isLowStock = true;
        }
        if (isLowStock == true) {
            cout << " [LOW STOCK]";
        }

        cout << endl;
    }
}

void savePrescriptionsToFile(Prescription prescriptionArray[], int numberOfPrescriptions, const char* filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileException("Could not open prescriptions file for writing.");
    }
    for (int i = 0; i < numberOfPrescriptions; i++) {
        Date d = prescriptionArray[i].getCreatedDate();
        outFile << prescriptionArray[i].getRecordID() << "|"
                << prescriptionArray[i].getPatientID() << "|"
                << prescriptionArray[i].getMedicineName() << "|"
                << prescriptionArray[i].getDosageText() << "|"
                << prescriptionArray[i].getFrequencyText() << "|"
                << prescriptionArray[i].getPrescribedByDoctor() << "|"
                << prescriptionArray[i].getQuantityPrescribed() << "|"
                << d.day << "|" << d.month << "|" << d.year << "|"
                << prescriptionArray[i].getStatus() << "|"
                << prescriptionArray[i].getPricePerUnit() << "\n";
    }
    outFile.close();
}

int loadPrescriptionsFromFile(Prescription prescriptionArray[], int maxArraySize, const char* filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return 0;
    }

    char line[400];
    int count = 0;
    int corruptedLineCount = 0;
    while (inFile.getline(line, 400) && count < maxArraySize) {
        if (strlen(line) == 0) continue;

        char* token;
        token = strtok(line, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int recID = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int patID = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char medName[MAX_NAME_LEN]; strcpy(medName, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char doseText[30]; strcpy(doseText, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char freqText[30]; strcpy(freqText, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char doctorText[MAX_NAME_LEN]; strcpy(doctorText, token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int qty = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int day = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int month = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        int year = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { corruptedLineCount++; continue; }
        char statusText[20]; strcpy(statusText, token);

        // older saved files might not have a price field yet, so we check before reading it to be safe
        double priceValue;
        token = strtok(NULL, "|");
        if (token != NULL) {
            priceValue = atof(token);
        } else {
            priceValue = 0.0;
        }

        Date prescribedDate(day, month, year);
        Prescription p(recID, patID, medName, doseText, freqText, doctorText, qty, prescribedDate, priceValue);
        p.setStatus(statusText);

        prescriptionArray[count] = p;
        count++;
    }
    inFile.close();

    if (corruptedLineCount > 0) {
        cout << "Warning: skipped " << corruptedLineCount
             << " corrupted line(s) in " << filename << endl;
    }

    return count;
}

int generateNextPrescriptionID(Prescription prescriptionArray[], int numberOfPrescriptions) {
    int maxID = 4000;
    for (int i = 0; i < numberOfPrescriptions; i++) {
        if (prescriptionArray[i].getRecordID() > maxID) {
            maxID = prescriptionArray[i].getRecordID();
        }
    }
    return maxID + 1;
}

// this is the function a doctor (Staff) uses to give a patient a new
// prescription. it shows the stock list, asks which medicine and how much,
// checks there is enough stock, then takes the medicine away from stock
void addPrescriptionForPatient(Prescription prescriptionArray[], int& numberOfPrescriptions,
                                 MedicineStock medicineStockArray[], int patientID, const char* doctorName,
                                 PatientLinkedList& patientList) {
    if (numberOfPrescriptions >= MAX_PRESCRIPTIONS) {
        cout << "Prescription list is full, cannot add more right now." << endl;
        return;
    }

    // check the patient ID is real before we let a doctor prescribe anything for it
    Patient* checkPatient = patientList.findByID(patientID);
    if (checkPatient == NULL) {
        throw InputException("Patient ID not found.");
    }

    displayMedicineStockList(medicineStockArray);

    int medicineChoice;
    cout << "Pick a medicine (enter the number): ";
    cin >> medicineChoice;

    if (medicineChoice < 1 || medicineChoice > TOTAL_MEDICINE_TYPES) {
        throw InputException("That is not a valid medicine choice.");
    }

    int medicineIndex = medicineChoice - 1;

    int quantityWanted;
    cout << "How many units to prescribe: ";
    cin >> quantityWanted;

    if (quantityWanted < 1) {
        throw InputException("Quantity must be at least 1.");
    }

    // a sanity check on the upper end too: one prescription for over 100 units at once is unusual
    if (quantityWanted > 100) {
        throw InputException("Quantity per prescription cannot be more than 100 units.");
    }

    if (quantityWanted > medicineStockArray[medicineIndex].stockQuantity) {
        throw InputException("Not enough stock for this medicine.");
    }

    char dosageText[30];
    char frequencyText[30];
    cin.ignore(1000, '\n');
    cout << "Dosage (example: 500mg): ";
    cin.getline(dosageText, 30);
    fillInIfEmpty(dosageText, "N/A");
    cout << "Frequency (example: 3 times a day): ";
    cin.getline(frequencyText, 30);
    fillInIfEmpty(frequencyText, "N/A");

    // take the medicine away from the pharmacy stock
    medicineStockArray[medicineIndex].stockQuantity = medicineStockArray[medicineIndex].stockQuantity - quantityWanted;

    int newPrescriptionID = generateNextPrescriptionID(prescriptionArray, numberOfPrescriptions);
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date todayDate(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);

    Prescription newPrescription(newPrescriptionID, patientID,
                                   medicineStockArray[medicineIndex].medicineName,
                                   dosageText, frequencyText, doctorName,
                                   quantityWanted, todayDate,
                                   medicineStockArray[medicineIndex].pricePerUnit);

    prescriptionArray[numberOfPrescriptions] = newPrescription;
    numberOfPrescriptions = numberOfPrescriptions + 1;

    savePrescriptionsToFile(prescriptionArray, numberOfPrescriptions, FILE_PRESCRIPTIONS);
    saveMedicineStockToFile(medicineStockArray, TOTAL_MEDICINE_TYPES, FILE_MEDICINE_STOCK);

    cout << "Prescription added. Remaining stock for "
         << medicineStockArray[medicineIndex].medicineName << ": "
         << medicineStockArray[medicineIndex].stockQuantity << endl;
}

void viewPrescriptionsForPatient(Prescription prescriptionArray[], int numberOfPrescriptions, int patientID) {
    cout << "\n-- Prescriptions --" << endl;

    bool foundAny = false;
    for (int i = 0; i < numberOfPrescriptions; i++) {
        if (prescriptionArray[i].getPatientID() == patientID) {
            prescriptionArray[i].displayRecord();
            foundAny = true;
        }
    }

    if (foundAny == false) {
        cout << "No prescriptions found for this patient." << endl;
    }
}

// BONUS FEATURE 4: Patient Billing (Ward Cost + Medicine Cost)
// This ties together the ward number and the prescriptions to work out how
// much one patient owes the hospital, and saves it to a txt file.

// different ward groups cost different amounts per day. 
double getWardDailyRate(int wardNumber) {
    if (wardNumber >= 1 && wardNumber <= 3) {
        return 50.00;    // General Ward
    } else if (wardNumber >= 4 && wardNumber <= 6) {
        return 100.00;   // Semi-Private Ward
    } else {
        return 200.00;   // Private Ward (ward 7 to 10)
    }
}

const char* getWardTypeName(int wardNumber) {
    if (wardNumber >= 1 && wardNumber <= 3) {
        return "General Ward";
    } else if (wardNumber >= 4 && wardNumber <= 6) {
        return "Semi-Private Ward";
    } else {
        return "Private Ward";
    }
}

// adds up the cost of every prescription that belongs to one patient
double calculateMedicineCostForPatient(Prescription prescriptionArray[], int numberOfPrescriptions, int patientID) {
    double totalMedicineCost = 0.0;

    for (int i = 0; i < numberOfPrescriptions; i++) {
        if (prescriptionArray[i].getPatientID() == patientID) {
            double thisLineCost = prescriptionArray[i].getQuantityPrescribed() * prescriptionArray[i].getPricePerUnit();
            totalMedicineCost = totalMedicineCost + thisLineCost;
        }
    }

    return totalMedicineCost;
}

double calculateConsultationFeesForPatient(Appointment appointmentArray[], int numberOfAppointments,
                                             DoctorSchedule doctorScheduleList[], int patientID) {
    double totalFees = 0.0;

    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getPatientID() != patientID) {
            continue;
        }
        if (strcmp(appointmentArray[i].getStatus(), "Cancelled") == 0) {
            continue;
        }

        // find which doctor this appointment to get the fee
        for (int j = 0; j < TOTAL_DOCTORS; j++) {
            if (strcmp(appointmentArray[i].getDoctorName(), doctorScheduleList[j].doctorName) == 0) {
                totalFees = totalFees + doctorScheduleList[j].consultationFee;
            }
        }
    }

    return totalFees;
}

void generateAndSaveBillForPatient(Patient& targetPatient, Prescription prescriptionArray[], int numberOfPrescriptions,
                                     Appointment appointmentArray[], int numberOfAppointments,
                                     DoctorSchedule doctorScheduleList[]) {
    int numberOfDaysStayed;
    cout << "How many days has this patient stayed (enter 0 if not admitted yet): ";
    cin >> numberOfDaysStayed;

    if (numberOfDaysStayed < 0) {
        throw InputException("Number of days cannot be negative.");
    }

    // a sanity check on the upper end: 365 days or more in one single stay
    // is extremely unusual for this kind of system, and is maybe a
    // typo, so we stop it here just like we did for prescription quantity
    if (numberOfDaysStayed >= 365) {
        throw InputException("Number of days stayed looks too large, please check and try again.");
    }

    int wardNumber = targetPatient.getWardNumber();
    double dailyRate = getWardDailyRate(wardNumber);
    double wardCost = dailyRate * numberOfDaysStayed;
    double medicineCost = calculateMedicineCostForPatient(prescriptionArray, numberOfPrescriptions, targetPatient.getPatientID());
    double consultationFees = calculateConsultationFeesForPatient(appointmentArray, numberOfAppointments,
                                                                     doctorScheduleList, targetPatient.getPatientID());
    double totalCost = wardCost + medicineCost + consultationFees;

    ofstream outFile(FILE_BILL);
    if (!outFile.is_open()) {
        throw FileException("Could not open bill file for writing.");
    }

    outFile << "===== HOSPITAL BILL =====\n";
    outFile << "Patient Name      : " << targetPatient.getName() << "\n";
    outFile << "Patient ID        : " << targetPatient.getPatientID() << "\n";
    outFile << "Ward              : " << wardNumber << " (" << getWardTypeName(wardNumber) << ")\n";
    outFile << "Daily Rate        : RM" << dailyRate << "\n";
    outFile << "Days Stayed       : " << numberOfDaysStayed << "\n";
    outFile << "Ward Cost         : RM" << wardCost << "\n";
    outFile << "Medicine Items    :\n";

    for (int i = 0; i < numberOfPrescriptions; i++) {
        if (prescriptionArray[i].getPatientID() == targetPatient.getPatientID()) {
            double itemCost = prescriptionArray[i].getQuantityPrescribed() * prescriptionArray[i].getPricePerUnit();
            outFile << "   - " << prescriptionArray[i].getMedicineName()
                    << " x" << prescriptionArray[i].getQuantityPrescribed()
                    << " = RM" << itemCost << "\n";
        }
    }

    outFile << "Medicine Cost     : RM" << medicineCost << "\n";
    outFile << "Consultation Fees : RM" << consultationFees << "\n";
    outFile << "TOTAL COST        : RM" << totalCost << "\n";
    outFile << "==========================\n";
    outFile.close();

    // show the same bill on screen right away too
    cout << "\n===== HOSPITAL BILL =====" << endl;
    cout << "Patient Name      : " << targetPatient.getName() << endl;
    cout << "Patient ID        : " << targetPatient.getPatientID() << endl;
    cout << "Ward              : " << wardNumber << " (" << getWardTypeName(wardNumber) << ")" << endl;
    cout << "Daily Rate        : RM" << dailyRate << endl;
    cout << "Days Stayed       : " << numberOfDaysStayed << endl;
    cout << "Ward Cost         : RM" << wardCost << endl;
    cout << "Medicine Items    :" << endl;

    for (int i = 0; i < numberOfPrescriptions; i++) {
        if (prescriptionArray[i].getPatientID() == targetPatient.getPatientID()) {
            double itemCost = prescriptionArray[i].getQuantityPrescribed() * prescriptionArray[i].getPricePerUnit();
            cout << "   - " << prescriptionArray[i].getMedicineName()
                 << " x" << prescriptionArray[i].getQuantityPrescribed()
                 << " = RM" << itemCost << endl;
        }
    }

    cout << "Medicine Cost     : RM" << medicineCost << endl;
    cout << "Consultation Fees : RM" << consultationFees << endl;
    cout << "TOTAL COST        : RM" << totalCost << endl;
    cout << "==========================" << endl;
}

// lets a Customer (or Staff) read back the most recently saved bill from
// the txt file, the same idea as loadAndDisplaySummary()
void loadAndDisplayBill() {
    ifstream inFile(FILE_BILL);
    if (!inFile.is_open()) {
        cout << "No bill has been generated yet." << endl;
        return;
    }

    cout << "\n----- Reading saved bill from file -----" << endl;
    char line[200];
    while (inFile.getline(line, 200)) {
        cout << line << endl;
    }
    inFile.close();
}

// BONUS FEATURE: View All Staff Accounts
// A simple admin report that just lists every staff member currently registered in the system, one per line.
void displayAllStaffAccounts(StaffMember staffArray[], int numberOfStaff) {
    cout << "\n===== ALL STAFF ACCOUNTS =====" << endl;

    if (numberOfStaff == 0) {
        cout << "No staff accounts found." << endl;
        return;
    }

    for (int i = 0; i < numberOfStaff; i++) {
        staffArray[i].displayInfo();
    }

    cout << "Total staff accounts: " << numberOfStaff << endl;
    cout << "===============================" << endl;
}

// BONUS FEATURE: View All Prescriptions (admin overview, not filtered by
// one patient). Useful for Staff to see everything that has been given out
// across the whole hospital, not just for one person at a time.
void displayAllPrescriptions(Prescription prescriptionArray[], int numberOfPrescriptions) {
    cout << "\n===== ALL PRESCRIPTIONS =====" << endl;

    if (numberOfPrescriptions == 0) {
        cout << "No prescriptions found." << endl;
        return;
    }

    for (int i = 0; i < numberOfPrescriptions; i++) {
        prescriptionArray[i].displayRecord();
    }

    cout << "Total prescriptions: " << numberOfPrescriptions << endl;
    cout << "===============================" << endl;
}

// BONUS FEATURE: Stop A Patient Double-Booking The same doctor at the same Eexact date and time. We check this before saving a new appointment.
bool hasSameTimeSlotConflict(Appointment appointmentArray[], int numberOfAppointments,
                               int patientID, Date theDate, const char* theTimeSlot) {
    for (int i = 0; i < numberOfAppointments; i++) {
        bool samePatient = false;
        if (appointmentArray[i].getPatientID() == patientID) {
            samePatient = true;
        }

        bool sameDate = false;
        Date apptDate = appointmentArray[i].getApptDate();
        if (apptDate.day == theDate.day && apptDate.month == theDate.month && apptDate.year == theDate.year) {
            sameDate = true;
        }

        bool sameTime = false;
        if (strcmp(appointmentArray[i].getTimeSlot(), theTimeSlot) == 0) {
            sameTime = true;
        }

        bool notCancelled = true;
        if (strcmp(appointmentArray[i].getStatus(), "Cancelled") == 0) {
            notCancelled = false;
        }

        if (samePatient == true && sameDate == true && sameTime == true && notCancelled == true) {
            return true;
        }
    }
    return false;
}

// BONUS FEATURE 5: Ward Occupancy Report
// Loops through all 10 wards and shows how many beds are taken and how
// many are free in each one. This is just a way to look at the
// numbers that countPatientsInWard() already calculates for us.
void displayWardOccupancyReport(PatientLinkedList& patientList) {
    cout << "\n===== WARD OCCUPANCY REPORT =====" << endl;

    for (int wardNumber = 1; wardNumber <= TOTAL_WARDS; wardNumber++) {
        int patientsInThisWard = countPatientsInWard(patientList, wardNumber);
        int freeBeds = WARD_CAPACITY - patientsInThisWard;

        cout << "Ward " << wardNumber << " (" << getWardTypeName(wardNumber) << ")"
             << " : " << patientsInThisWard << " / " << WARD_CAPACITY << " beds taken";

        if (freeBeds <= 0) {
            cout << " [FULL]" << endl;
        } else {
            cout << " [" << freeBeds << " free]" << endl;
        }
    }

    cout << "===================================" << endl;

    // also add up the totals for each ward TYPE, not just each individual
    // ward number, so Staff can see the bigger picture easier
    int generalWardTotal = 0;
    int semiPrivateWardTotal = 0;
    int privateWardTotal = 0;

    for (int wardNumber = 1; wardNumber <= TOTAL_WARDS; wardNumber++) {
        int patientsInThisWard = countPatientsInWard(patientList, wardNumber);

        if (wardNumber >= 1 && wardNumber <= 3) {
            generalWardTotal = generalWardTotal + patientsInThisWard;
        } else if (wardNumber >= 4 && wardNumber <= 6) {
            semiPrivateWardTotal = semiPrivateWardTotal + patientsInThisWard;
        } else {
            privateWardTotal = privateWardTotal + patientsInThisWard;
        }
    }

    cout << "\n-- Totals By Ward Type --" << endl;
    cout << "General Ward (Ward 1-3)      : " << generalWardTotal << " / " << (3 * WARD_CAPACITY) << " beds taken" << endl;
    cout << "Semi-Private Ward (Ward 4-6) : " << semiPrivateWardTotal << " / " << (3 * WARD_CAPACITY) << " beds taken" << endl;
    cout << "Private Ward (Ward 7-10)     : " << privateWardTotal << " / " << (4 * WARD_CAPACITY) << " beds taken" << endl;
}

// BONUS FEATURE: Most Prescribed Medicine Report
// Goes through every prescription and works out which medicine has been
// given out the most number of times. Just a simple "linear scan and keep
// the best one so far" idea, similar to how Selection Sort picks the best element
void displayMostPrescribedMedicineReport(Prescription prescriptionArray[], int numberOfPrescriptions,
                                           MedicineStock medicineStockArray[]) {
    cout << "\n===== MOST PRESCRIBED MEDICINE REPORT =====" << endl;

    if (numberOfPrescriptions == 0) {
        cout << "No prescriptions have been made yet." << endl;
        return;
    }

    for (int medicineIndex = 0; medicineIndex < TOTAL_MEDICINE_TYPES; medicineIndex++) {
        int timesPrescribed = 0;

        for (int i = 0; i < numberOfPrescriptions; i++) {
            if (strcmp(prescriptionArray[i].getMedicineName(), medicineStockArray[medicineIndex].medicineName) == 0) {
                timesPrescribed = timesPrescribed + 1;
            }
        }

        cout << medicineStockArray[medicineIndex].medicineName << " : prescribed " << timesPrescribed << " time(s)" << endl;
    }

    cout << "=============================================" << endl;
}

// BONUS FEATURE: System Statistics Dashboard
// Pulls together a quick count from every part of the system into one
// screen, so Staff can see the overall picture without going into each menu one by one.
void displaySystemStatistics(PatientLinkedList& patientList, int numberOfStaff, int numberOfAppointments,
                               int numberOfPrescriptions, MedicineStock medicineStockArray[]) {
    cout << "\n===== SYSTEM STATISTICS =====" << endl;
    cout << "Total Patients      : " << patientList.getCount() << endl;
    cout << "Total Staff Accounts : " << numberOfStaff << endl;
    cout << "Total Appointments  : " << numberOfAppointments << endl;
    cout << "Total Prescriptions : " << numberOfPrescriptions << endl;

    // count how many medicines are running low on stock
    int lowStockCount = 0;
    for (int i = 0; i < TOTAL_MEDICINE_TYPES; i++) {
        if (medicineStockArray[i].stockQuantity < LOW_STOCK_THRESHOLD) {
            lowStockCount = lowStockCount + 1;
        }
    }
    cout << "Medicines Low On Stock : " << lowStockCount << " out of " << TOTAL_MEDICINE_TYPES << endl;

    // count how many of the 10 wards are completely full right now
    int fullWardCount = 0;
    for (int wardNumber = 1; wardNumber <= TOTAL_WARDS; wardNumber++) {
        int patientsInThisWard = countPatientsInWard(patientList, wardNumber);
        if (patientsInThisWard >= WARD_CAPACITY) {
            fullWardCount = fullWardCount + 1;
        }
    }
    cout << "Wards Currently Full   : " << fullWardCount << " out of " << TOTAL_WARDS << endl;

    cout << "===============================" << endl;
}

// BONUS FEATURE 6: Restock Medicine
// The opposite of giving out medicine - this lets Staff add more units
// back into the pharmacy stock when new supply arrives.
void restockMedicineMenu(MedicineStock medicineStockArray[]) {
    displayMedicineStockList(medicineStockArray);

    int medicineChoice;
    cout << "Pick a medicine to restock (enter the number): ";
    cin >> medicineChoice;

    if (medicineChoice < 1 || medicineChoice > TOTAL_MEDICINE_TYPES) {
        throw InputException("That is not a valid medicine choice.");
    }

    int medicineIndex = medicineChoice - 1;

    int amountToAdd;
    cout << "How many units to add to stock: ";
    cin >> amountToAdd;

    if (amountToAdd < 1) {
        throw InputException("Amount to add must be at least 1.");
    }

    // a sanity check: restocking an unusually large amount at once is
    // probably a typo, so we ask the staff member to confirm first
    if (amountToAdd > 1000) {
        cout << "That is a very large amount (" << amountToAdd << " units). Are you sure? (Y/N): ";
        char confirmAnswer;
        cin >> confirmAnswer;

        bool userConfirmed = false;
        if (confirmAnswer == 'Y' || confirmAnswer == 'y') {
            userConfirmed = true;
        }

        if (userConfirmed == false) {
            cout << "Restock cancelled." << endl;
            return;
        }
    }

    medicineStockArray[medicineIndex].stockQuantity = medicineStockArray[medicineIndex].stockQuantity + amountToAdd;
    saveMedicineStockToFile(medicineStockArray, TOTAL_MEDICINE_TYPES, FILE_MEDICINE_STOCK);

    cout << "Stock updated. New stock for " << medicineStockArray[medicineIndex].medicineName
         << ": " << medicineStockArray[medicineIndex].stockQuantity << endl;
}

// BONUS FEATURE 7: Sorting and Searching Prescriptions
// Same Selection Sort idea as before, just applied to a 3rd data type now.
// criteria 1 = sort by prescribed date, criteria 2 = sort by patient ID
void selectionSortPrescriptions(Prescription prescriptionArray[], int totalPrescriptions, int criteria) {
    for (int i = 0; i < totalPrescriptions - 1; i++) {
        int bestIndex = i;

        for (int j = i + 1; j < totalPrescriptions; j++) {
            bool jIsBetter = false;

            if (criteria == 1) {
                Date dateJ = prescriptionArray[j].getCreatedDate();
                Date dateBest = prescriptionArray[bestIndex].getCreatedDate();
                int valueJ = dateJ.year * 10000 + dateJ.month * 100 + dateJ.day;
                int valueBest = dateBest.year * 10000 + dateBest.month * 100 + dateBest.day;
                if (valueJ < valueBest) {
                    jIsBetter = true;
                }
            } else if (criteria == 2) {
                if (prescriptionArray[j].getPatientID() < prescriptionArray[bestIndex].getPatientID()) {
                    jIsBetter = true;
                }
            }

            if (jIsBetter == true) {
                bestIndex = j;
            }
        }

        if (bestIndex != i) {
            Prescription temporaryPrescription = prescriptionArray[i];
            prescriptionArray[i] = prescriptionArray[bestIndex];
            prescriptionArray[bestIndex] = temporaryPrescription;
        }
    }
}

void sortAllPrescriptionsMenu(Prescription prescriptionArray[], int numberOfPrescriptions) {
    cout << "\n-- Sort All Prescriptions --" << endl;
    cout << "1. Sort by Date Prescribed" << endl;
    cout << "2. Sort by Patient ID" << endl;
    cout << "Enter choice: ";

    int sortChoice;
    cin >> sortChoice;

    if (sortChoice != 1 && sortChoice != 2) {
        throw InputException("Invalid sort option.");
    }

    selectionSortPrescriptions(prescriptionArray, numberOfPrescriptions, sortChoice);

    cout << "\n-- Sorted Result --" << endl;
    for (int i = 0; i < numberOfPrescriptions; i++) {
        prescriptionArray[i].displayRecord();
    }
}

void searchPrescriptionsByMedicineMenu(Prescription prescriptionArray[], int numberOfPrescriptions) {
    char medicineNameToFind[MAX_NAME_LEN];
    cout << "\n-- Search Prescriptions By Medicine Name --" << endl;
    cout << "Enter medicine name (or part of it): ";
    cin.ignore(1000, '\n');
    cin.getline(medicineNameToFind, MAX_NAME_LEN);

    bool foundAny = false;
    for (int i = 0; i < numberOfPrescriptions; i++) {
        if (strstr(prescriptionArray[i].getMedicineName(), medicineNameToFind) != NULL) {
            prescriptionArray[i].displayRecord();
            foundAny = true;
        }
    }

    if (foundAny == false) {
        throw InputException("No prescriptions found with that medicine name.");
    }
}

// BONUS FEATURE 8: Discharge Patient
// A dedicated function (instead of using the generic Edit Patient option)
// just for discharging someone. Once the status changes to "Discharged",
// countPatientsInWard() automatically stops counting this patient, which
// frees up the bed for the next admission.
void dischargePatientMenu(PatientLinkedList& patientList, Prescription prescriptionArray[], int numberOfPrescriptions,
                            Appointment appointmentArray[], int numberOfAppointments,
                            DoctorSchedule doctorScheduleList[]) {
    int patientIDToDischarge;
    cout << "\n-- Discharge Patient --" << endl;
    cout << "Enter Patient ID to discharge: ";
    cin >> patientIDToDischarge;

    Patient* targetPatient = patientList.findByID(patientIDToDischarge);
    if (targetPatient == NULL) {
        throw InputException("Patient ID not found.");
    }

    if (strcmp(targetPatient->getPatientStatus(), "Admitted") != 0) {
        throw InputException("This patient is not currently admitted, so they cannot be discharged.");
    }

    targetPatient->setPatientStatus("Discharged");
    savePatientsToFile(patientList, FILE_PATIENTS);

    cout << "Patient " << targetPatient->getName() << " has been discharged." << endl;
    cout << "Ward " << targetPatient->getWardNumber() << " now has a free bed." << endl;

    cout << "\nWould you like to generate the final bill now? (Y/N): ";
    char answer;
    cin >> answer;

    if (answer == 'Y' || answer == 'y') {
        generateAndSaveBillForPatient(*targetPatient, prescriptionArray, numberOfPrescriptions,
                                        appointmentArray, numberOfAppointments, doctorScheduleList);
    }
}

// Works out the next free Patient ID by looking at the highest ID currently
// in the list and adding 1 with auto increment done by hand)
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
    cin.getline(name, MAX_NAME_LEN);
    fillInIfEmpty(name, "N/A");
    cout << "IC Number: "; cin.getline(ic, 20);
    fillInIfEmpty(ic, "N/A");
    cout << "Age: "; cin >> age;
    cout << "Gender (M/F): "; cin >> gender;
    cout << "Role (Doctor/Nurse/Admin): "; cin >> role;
    cin.ignore(1000, '\n');
	cout << "Username: "; cin.getline(username, MAX_NAME_LEN);
	if (strlen(username) == 0) {
	    throw InputException("Username cannot be empty.");
	}
	cout << "Password: "; cin.getline(password, MAX_PASS_LEN);
	if (strlen(password) == 0) {
	    throw InputException("Password cannot be empty.");
	}
    cout << "Phone: "; cin.getline(phone, 20);
    fillInIfEmpty(phone, "N/A");
    cout << "Address: "; cin.getline(address, MAX_TEXT_LEN);
    fillInIfEmpty(address, "N/A");

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
    cout << "Username: "; cin.getline(username, MAX_NAME_LEN);
	cout << "Password: "; cin.getline(password, MAX_PASS_LEN);

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
    fillInIfEmpty(name, "N/A");
    cout << "IC Number: "; cin.getline(ic, 20);
    fillInIfEmpty(ic, "N/A");
    cout << "Age: "; cin >> age;
    cout << "Gender (M/F): "; cin >> gender;
    cout << "Ward Number (1-10): "; cin >> ward;
    
    // basic input validation - this is where the try/catch error handling
    if (ward < 1 || ward > 10) {
        throw InputException("Ward number must be between 1 and 10.");
    }
    
    // bonus feature: check the ward is not already full before letting a
    // new patient be admitted into it
    int patientsAlreadyInWard = countPatientsInWard(list, ward);
    if (patientsAlreadyInWard >= WARD_CAPACITY) {
        throw InputException("This ward is already full (max 4 patients). Please choose a different ward.");
    }
    cin.ignore(1000, '\n');
    cout << "Diagnosis: "; cin.getline(diagnosis, MAX_TEXT_LEN);
    fillInIfEmpty(diagnosis, "N/A");
    cout << "Assigned Doctor: "; cin.getline(doctor, MAX_NAME_LEN);
    fillInIfEmpty(doctor, "N/A");
    cout << "Username (for patient login): "; cin.getline(username, MAX_NAME_LEN);
    if (strlen(username) == 0) {
	    throw InputException("Username cannot be empty.");
	}
    cout << "Password: "; cin.getline(password, MAX_PASS_LEN);
    if (strlen(password) == 0) {
	    throw InputException("Password cannot be empty.");
	}
    cout << "Phone: "; cin.getline(phone, 20);
    fillInIfEmpty(phone, "N/A");
    cout << "Address: "; cin.getline(address, MAX_TEXT_LEN);
    fillInIfEmpty(address, "N/A");
    
    int newID = generateNextPatientID(list);
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date admDate(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
    Patient newPatient(newID, name, ic, age, gender, username, password,
                        admDate, ward, diagnosis, doctor);
    newPatient.setContact(phone, address);
    list.insertAtEnd(newPatient);
    
    // save immediately so the Customer module 
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
        fillInIfEmpty(newDiag, "N/A");
        targetPatient->setDiagnosis(newDiag);
    } else if (editChoice == 2) {
        char newDoc[MAX_NAME_LEN];
        cout << "New Assigned Doctor: "; cin.getline(newDoc, MAX_NAME_LEN);
        fillInIfEmpty(newDoc, "N/A");
        targetPatient->setAssignedDoctor(newDoc);
    } else if (editChoice == 3) {
        int newWard;
        cout << "New Ward Number: "; cin >> newWard;

        if (newWard < 1 || newWard > TOTAL_WARDS) {
            throw InputException("Ward number must be between 1 and 10.");
        }

        // only check capacity if this patient is currently occupying a bed -
        // if they are not "Admitted" yet, changing the ward number does not
        // take up a bed by itself
        if (strcmp(targetPatient->getPatientStatus(), "Admitted") == 0) {
            int patientsAlreadyInNewWard = countPatientsInWard(list, newWard);
            if (patientsAlreadyInNewWard >= WARD_CAPACITY) {
                throw InputException("That ward is already full (max 4 patients). Please choose a different ward.");
            }
        }

        targetPatient->setWardNumber(newWard);
    } else if (editChoice == 4) {
        char newStatus[20];
        cout << "New Status (Admitted/Discharged): "; cin >> newStatus;

        // only check capacity when this patient is actually MOVING INTO
        // "Admitted" status. Changing to "Discharged" always frees a bed
        // instead of taking one, so it never needs a capacity check
        bool wasAlreadyAdmitted = (strcmp(targetPatient->getPatientStatus(), "Admitted") == 0);
        bool isBecomingAdmitted = (strcmp(newStatus, "Admitted") == 0);

        if (isBecomingAdmitted == true && wasAlreadyAdmitted == false) {
            int patientsAlreadyInWard = countPatientsInWard(list, targetPatient->getWardNumber());
            if (patientsAlreadyInWard >= WARD_CAPACITY) {
                throw InputException("This patient's ward is already full (max 4 patients). Change the ward number first, or pick a different status.");
            }
        }

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

    Patient* patientToDelete = list.findByID(id);
    if (patientToDelete == NULL) {
        throw InputException("Patient ID not found, nothing deleted.");
    }

    cout << "You are about to delete this record:" << endl;
    patientToDelete->displayInfo();
    cout << "Are you sure? (Y/N): ";
    char confirmAnswer;
    cin >> confirmAnswer;

    bool userConfirmed = false;
    if (confirmAnswer == 'Y' || confirmAnswer == 'y') {
        userConfirmed = true;
    }

    if (userConfirmed == false) {
        cout << "Delete cancelled, nothing was removed." << endl;
        return;
    }

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

// ---- Appointment management (extra feature: keeps Staff aware of bookings made by Customers ----
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

    // bonus feature: check the new ward still has space before transferring
    int patientsAlreadyInNewWard = countPatientsInWard(list, newWard);
    if (patientsAlreadyInNewWard >= WARD_CAPACITY) {
        cout << "Ward " << newWard << " is already full (max 4 patients). Cannot transfer there." << endl;
        return;
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
                    Appointment appointmentArray[], int numberOfAppointments,
                    Prescription prescriptionArray[], int& numberOfPrescriptions,
                    MedicineStock medicineStockArray[], DoctorSchedule doctorScheduleList[],
                    StaffMember staffArray[], int numberOfStaff) {
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
        cout << "11. Add Prescription For A Patient" << endl;
        cout << "12. View A Patient's Prescriptions" << endl;
        cout << "13. Generate Patient Bill" << endl;
        cout << "14. View Ward Occupancy Report" << endl;
        cout << "15. Restock Medicine" << endl;
        cout << "16. Sort All Prescriptions" << endl;
        cout << "17. Search Prescriptions By Medicine" << endl;
        cout << "18. Discharge Patient" << endl;
        cout << "19. View All Staff Accounts" << endl;
        cout << "20. View All Prescriptions" << endl;
        cout << "21. View Most Prescribed Medicine Report" << endl;
        cout << "22. View System Statistics" << endl;
        cout << "0. Logout" << endl;
        cout << "Enter choice: ";

        // try/catch around each menu action: if anything goes wrong (bad
        // input, record not found, file error) we print a friendly message
        // and go straight back to the menu instead of crashing or ending the program
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
                case 11: {
                    int patientIDForPrescription;
                    cout << "Enter Patient ID to prescribe medicine for: ";
                    cin >> patientIDForPrescription;
                    addPrescriptionForPatient(prescriptionArray, numberOfPrescriptions,
                                                medicineStockArray, patientIDForPrescription,
                                                currentStaff.getName(), patientList);
                    break;
                }
                case 12: {
                    int patientIDToView;
                    cout << "Enter Patient ID to view prescriptions for: ";
                    cin >> patientIDToView;
                    viewPrescriptionsForPatient(prescriptionArray, numberOfPrescriptions, patientIDToView);
                    break;
                }
                case 13: {
                    int patientIDForBill;
                    cout << "Enter Patient ID to generate a bill for: ";
                    cin >> patientIDForBill;
                    Patient* patientForBill = patientList.findByID(patientIDForBill);
                    if (patientForBill == NULL) {
                        throw InputException("Patient ID not found.");
                    }
                    generateAndSaveBillForPatient(*patientForBill, prescriptionArray, numberOfPrescriptions,
                                                    appointmentArray, numberOfAppointments, doctorScheduleList);
                    break;
                }
                case 14: displayWardOccupancyReport(patientList); break;
                case 15: restockMedicineMenu(medicineStockArray); break;
                case 16: sortAllPrescriptionsMenu(prescriptionArray, numberOfPrescriptions); break;
                case 17: searchPrescriptionsByMedicineMenu(prescriptionArray, numberOfPrescriptions); break;
                case 18: dischargePatientMenu(patientList, prescriptionArray, numberOfPrescriptions,
                                                appointmentArray, numberOfAppointments, doctorScheduleList); break;
                case 19: displayAllStaffAccounts(staffArray, numberOfStaff); break;
                case 20: displayAllPrescriptions(prescriptionArray, numberOfPrescriptions); break;
                case 21: displayMostPrescribedMedicineReport(prescriptionArray, numberOfPrescriptions, medicineStockArray); break;
                case 22: displaySystemStatistics(patientList, numberOfStaff, numberOfAppointments, numberOfPrescriptions, medicineStockArray); break;
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
                       Appointment appointmentArray[], int numberOfAppointments,
                       Prescription prescriptionArray[], int& numberOfPrescriptions,
                       MedicineStock medicineStockArray[], DoctorSchedule doctorScheduleList[]) {
    int choice;
    cout << "\n========== STAFF / ADMIN MODULE ==========" << endl;
    cout << "1. Register new staff account" << endl;
    cout << "2. Login" << endl;
    cout << "3. Back to main menu" << endl;
    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore(1000, '\n');

    if (choice == 1) {
        registerStaff(staffArray, numberOfStaff, MAX_STAFF);
    }

    if (choice == 1 || choice == 2) {
        int loggedInIndex = staffLoginPrompt(staffArray, numberOfStaff);
        if (loggedInIndex != -1) {
            appendLoginLog(staffArray[loggedInIndex].getUsername(), staffArray[loggedInIndex].getRole(), "LOGIN");
            staffMainMenu(patientList, staffArray[loggedInIndex], appointmentArray, numberOfAppointments,
                          prescriptionArray, numberOfPrescriptions, medicineStockArray, doctorScheduleList,
                          staffArray, numberOfStaff);
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
// actually admitted, see editPatientRecord().
void registerPatient(PatientLinkedList& list) {
    char name[MAX_NAME_LEN], ic[20], username[MAX_NAME_LEN], password[MAX_PASS_LEN];
    char phone[20], address[MAX_TEXT_LEN];
    int age;
    char gender;

    cout << "\n-- Patient Registration --" << endl;
    cout << "Full Name: "; cin.getline(name, MAX_NAME_LEN);
    fillInIfEmpty(name, "N/A");
    cout << "IC Number: "; cin.getline(ic, 20);
    fillInIfEmpty(ic, "N/A");
    cout << "Age: "; cin >> age;
    cout << "Gender (M/F): "; cin >> gender;
    cin.ignore(1000, '\n');
    cout << "Username: "; cin.getline(username, MAX_NAME_LEN);
    if (strlen(username) == 0) {
	    throw InputException("Username cannot be empty.");
	}
	cout << "Password: "; cin.getline(password, MAX_PASS_LEN);
	if (strlen(password) == 0) {
	    throw InputException("Password cannot be empty.");
	}
    cout << "Phone: "; cin.getline(phone, 20);
    fillInIfEmpty(phone, "N/A");
    cout << "Address: "; cin.getline(address, MAX_TEXT_LEN);
    fillInIfEmpty(address, "N/A");

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
    cout << "Username: "; cin.getline(username, MAX_NAME_LEN);
	cout << "Password: "; cin.getline(password, MAX_PASS_LEN);

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
void bookAppointment(Appointment appointmentArray[], int& numberOfAppointments, int maxArraySize,
                      int patientID, DoctorSchedule doctorScheduleList[]) {
    if (numberOfAppointments >= maxArraySize) {
        cout << "Appointment list is full, cannot book more right now." << endl;
        return;
    }

    cout << "\n-- Book New Appointment --" << endl;

    // Pick a doctor from the schedule list instead of just typing any name
    displayDoctorScheduleList(doctorScheduleList);

    int doctorChoice;
    cout << "Pick a doctor (enter the number): ";
    cin >> doctorChoice;

    if (doctorChoice < 1 || doctorChoice > TOTAL_DOCTORS) {
        throw InputException("That is not a valid doctor choice.");
    }

    int doctorIndex = doctorChoice - 1;

    char dayOfWeekText[15];
    cout << "Which day of the week (example: Monday): ";
    cin >> dayOfWeekText;

    bool doctorWorksThisDay = isDoctorWorkingOnThisDay(doctorScheduleList[doctorIndex], dayOfWeekText);
    if (doctorWorksThisDay == false) {
        throw InputException("This doctor does not work on that day. Please pick another day or doctor.");
    }

    int day, month, year;
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

    Date apptDate(day, month, year);

    // Check the doctor still has space left on that exact date
    int alreadyBookedCount = countAppointmentsForDoctorOnDate(appointmentArray, numberOfAppointments,
                                                                 doctorScheduleList[doctorIndex].doctorName, apptDate);
    if (alreadyBookedCount >= doctorScheduleList[doctorIndex].maxPatientsPerDay) {
        throw InputException("Sorry, this doctor is fully booked on that day. Please pick another day.");
    }

    char timeSlot[10], reason[MAX_TEXT_LEN];
    cin.ignore(1000, '\n');
    cout << "Preferred Time (e.g. 10:30AM): "; cin.getline(timeSlot, 10);
    fillInIfEmpty(timeSlot, "N/A");

    // Stop the same patient booking two appointments at the exact same date and time
    bool conflictFound = hasSameTimeSlotConflict(appointmentArray, numberOfAppointments, patientID, apptDate, timeSlot);
    if (conflictFound == true) {
        throw InputException("You already have a booking at this exact date and time.");
    }

    cout << "Reason for visit: "; cin.getline(reason, MAX_TEXT_LEN);

    int newID = generateNextAppointmentID(appointmentArray, numberOfAppointments);
    Appointment newAppt(newID, patientID, doctorScheduleList[doctorIndex].doctorName, apptDate, timeSlot, reason);
    newAppt.setDayOfWeekText(dayOfWeekText);

    appointmentArray[numberOfAppointments] = newAppt;
    numberOfAppointments++;

    // save immediately, so Staff sees this new booking right away the next time they open the Staff module
    saveAppointmentsToFile(appointmentArray, numberOfAppointments, FILE_APPOINTMENTS);
    cout << "Appointment booked successfully! Appointment ID: " << newID << endl;
}

// ---- Edit/Update Record: change date/time or reason on patient own booking ----
void editOwnAppointment(Appointment appointmentArray[], int numberOfAppointments, int patientID) {
    int apptID;
    cout << "\n-- Edit My Appointment --" << endl;
    cout << "Enter your Appointment ID: ";
    cin >> apptID;

    for (int i = 0; i < numberOfAppointments; i++) {
        if (appointmentArray[i].getRecordID() == apptID) {
            // security check: a patient may only edit their OWN bookings, never someone else ID
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
                fillInIfEmpty(reason, "N/A");
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

// ---- Delete Record: cancel OWN booking ----
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

            cout << "You are about to cancel this booking:" << endl;
            appointmentArray[i].displayRecord();
            cout << "Are you sure? (Y/N): ";
            char confirmAnswer;
            cin >> confirmAnswer;

            bool userConfirmed = false;
            if (confirmAnswer == 'Y' || confirmAnswer == 'y') {
                userConfirmed = true;
            }

            if (userConfirmed == false) {
                cout << "Cancel request stopped, your booking is still active." << endl;
                return;
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
            // check if this booking is "active" by comparing the status text one by one, instead of doing it in one line
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
// This is a SEPARATE Selection Sort from selectionSortPatients(),
// because it sorts an array of Appointment objects instead of Patient
// objects - same algorithm idea, different data type.
void selectionSortAppointments(Appointment appointmentArray[], int totalAppointments, int criteria) {
    for (int i = 0; i < totalAppointments - 1; i++) {
        int bestIndex = i;

        for (int j = i + 1; j < totalAppointments; j++) {
            bool jIsBetter = false;

            if (criteria == 1) {
                // sort by date: turn each date into one comparable number, like 2026-06-25 becomes 20260625, so smaller number = earlier date
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

    // pull out only this patient's own bookings into a separate temporary array before sorting, so we not disturb other patients' bookings
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
void customerMainMenu(Patient& currentPatient, Appointment appointmentArray[], int& numberOfAppointments,
                       DoctorSchedule doctorScheduleList[],
                       Prescription prescriptionArray[], int numberOfPrescriptions) {
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
        cout << "9. View My Prescriptions" << endl;
        cout << "10. View My Latest Bill" << endl;
        cout << "11. View Doctor Schedule" << endl;
        cout << "0. Logout" << endl;
        cout << "Enter choice: ";

        // same try/catch pattern as the Staff module: catch errors here so one bad input never crashes the whole program
        try {
            cin >> choice;

            switch (choice) {
                case 1: displayMyProfile(currentPatient); break;
                case 2: bookAppointment(appointmentArray, numberOfAppointments, MAX_APPOINTMENTS,
                                          currentPatient.getPatientID(), doctorScheduleList); break;
                case 3: editOwnAppointment(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 4: cancelOwnAppointment(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 5: displayOwnAppointmentsMenu(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 6: searchOwnAppointmentsMenu(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 7: sortOwnAppointmentsMenu(appointmentArray, numberOfAppointments, currentPatient.getPatientID()); break;
                case 8: customerSummaryReportMenu(currentPatient, appointmentArray, numberOfAppointments); break;
                case 9: viewPrescriptionsForPatient(prescriptionArray, numberOfPrescriptions, currentPatient.getPatientID()); break;
                case 10: loadAndDisplayBill(); break;
                case 11: displayDoctorScheduleList(doctorScheduleList); break;
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
void customerModuleEntry(PatientLinkedList& patientList, Appointment appointmentArray[], int& numberOfAppointments,
                          DoctorSchedule doctorScheduleList[],
                          Prescription prescriptionArray[], int numberOfPrescriptions) {
    int choice;
    cout << "\n========== CUSTOMER / PATIENT MODULE ==========" << endl;
    cout << "1. Register new patient account" << endl;
    cout << "2. Login" << endl;
    cout << "3. Back to main menu" << endl;
    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore(1000, '\n');

    if (choice == 1) {
        registerPatient(patientList);
    }

    if (choice == 1 || choice == 2) {
        Patient* loggedInPatient = patientLoginPrompt(patientList);
        if (loggedInPatient != NULL) {
            appendLoginLog(loggedInPatient->getUsername(), "Patient", "LOGIN");
            customerMainMenu(*loggedInPatient, appointmentArray, numberOfAppointments,
                             doctorScheduleList, prescriptionArray, numberOfPrescriptions);
            appendLoginLog(loggedInPatient->getUsername(), "Patient", "LOGOUT");
        } else {
            cout << "Login failed. Returning to main menu." << endl;
        }
    } else {
        cout << "Returning to main menu." << endl;
    }
}

// this function looks at how many patients we already have. if it is less
// than 50, it makes more fake patients one by one until we have 50 total
void generateSampleData(PatientLinkedList& patientList) {
    int currentTotal = patientList.getCount();

    // if we already have enough patients, just stop here, no need to do more
    if (currentTotal >= 50) {
        return;
    }

    char firstNameList[20][20] = {
        "Ahmad", "Siti", "Wong", "Kumar", "Lee", "Tan", "Nurul", "Raj",
        "Mei", "Hassan", "Farah", "Chong", "Aisyah", "Wei", "Devi", "Ali",
        "Lim", "Zainab", "Kavi", "Hui"
    };

    char lastNameList[10][20] = {
        "Bin Ismail", "Binti Yusof", "Mei Ling", "Raj Kumar", "Chong Wei",
        "Wei Jian", "Binti Hassan", "Kumar Singh", "Hui Ling", "Bin Abu"
    };

    char diagnosisList[12][20] = {
        "Fever", "Flu", "Asthma", "Diabetes", "Hypertension", "Migraine",
        "Sprain", "Fracture", "Allergy", "Food Poisoning", "Back Pain", "Anemia"
    };

    char doctorList[6][20] = {
        "Dr. Tan", "Dr. Lee", "Dr. Lim", "Dr. Kumar", "Dr. Wong", "Dr. Farah"
    };

    int howManyWeNeed = 50 - currentTotal;
    int startingID = generateNextPatientID(patientList);

    for (int i = 0; i < howManyWeNeed; i++) {
        int newPatientID = startingID + i;

        // just keep cycling through the name lists using the loop number
        int firstNameIndex = i % 20;
        int lastNameIndex = i % 10;

        char fullName[MAX_NAME_LEN];
        strcpy(fullName, firstNameList[firstNameIndex]);
        strcat(fullName, " ");
        strcat(fullName, lastNameList[lastNameIndex]);

        int icDay = (i % 28) + 1;
        int icMonth = (i % 12) + 1;
        int icYear = 70 + (i % 30);
        char icNumber[25];
        sprintf(icNumber, "%02d%02d%02d-%02d-%04d", icYear, icMonth, icDay, 10, 1000 + i);

        char usernameText[MAX_NAME_LEN];
        sprintf(usernameText, "patient%d", newPatientID);

        char passwordText[MAX_PASS_LEN];
        sprintf(passwordText, "pass%d", newPatientID);

        int age = 18 + (i % 60);

        // pick gender step by step instead of using a shortcut
        char gender;
        if (i % 2 == 0) {
            gender = 'M';
        } else {
            gender = 'F';
        }

        int wardNumber = (i % 10) + 1;
        int diagnosisIndex = i % 12;
        int doctorIndex = i % 6;

        int admissionDay = (i % 28) + 1;
        int admissionMonth = (i % 12) + 1;
        Date admissionDate(admissionDay, admissionMonth, 2026);

        Patient newSamplePatient(newPatientID, fullName, icNumber, age, gender,
                                   usernameText, passwordText, admissionDate,
                                   wardNumber, diagnosisList[diagnosisIndex],
                                   doctorList[doctorIndex]);

        // each ward gets 5 sample patients (since ward number cycles every
        // 10 patients and we make 50 total). if all 5 stayed "Admitted" the
        // ward would already be full before the demo even starts, since our
        // ward capacity is only 4. so we mark the last 2 of every 5 as
        // "Discharged" instead, which leaves 1 free bed in every ward
        int cycleNumber = i / 10;
        if (cycleNumber >= 3) {
            newSamplePatient.setPatientStatus("Discharged");
        }

        patientList.insertAtEnd(newSamplePatient);
    }

    savePatientsToFile(patientList, FILE_PATIENTS);
    cout << "Sample data added. Total patients now: " << patientList.getCount() << endl;
}

// this function checks if there is already at least one staff account. if
// there is none yet, it makes one default admin account so we are able to
// log in and test the Staff side of the system right away
void generateDefaultAdmin(StaffMember staffArray[], int& numberOfStaff) {
    // if there is already a staff account, we do not need to make another one
    if (numberOfStaff > 0) {
        return;
    }

    Date joinedDate(1, 1, 2026);
    StaffMember defaultAdmin(2000, "System Administrator", "000000-00-0000", 40, 'M',
                               "admin", "admin123", "Admin", joinedDate);

    staffArray[numberOfStaff] = defaultAdmin;
    numberOfStaff = numberOfStaff + 1;

    saveStaffToFile(staffArray, numberOfStaff, FILE_STAFF);
    cout << "Default admin account made for you. Username: admin   Password: admin123" << endl;
}

// a simple help screen so the person using the program can quickly understand what the system can do
void displayHelpScreen() {
    cout << "\n===== ABOUT THIS SYSTEM =====" << endl;
    cout << "This is a Hospital Patient Record Management System made for" << endl;
    cout << "TDS4223 Data Structure and Algorithms, Group 4." << endl;
    cout << endl;
    cout << "Staff / Admin can: add, edit, delete, search, sort, and display" << endl;
    cout << "patient records, manage appointments, prescribe medicine, generate" << endl;
    cout << "bills, and discharge patients." << endl;
    cout << endl;
    cout << "Customer / Patient can: register, book appointments, view their" << endl;
    cout << "own bookings, prescriptions, and bills." << endl;
    cout << endl;
    cout << "Default admin login -> Username: admin   Password: admin123" << endl;
    cout << "===============================" << endl;
}


int main() {
    cout << "=================================================" << endl;
    cout << "   HOSPITAL PATIENT RECORD MANAGEMENT SYSTEM" << endl;
    cout << "   TDS4223 Group 4" << endl;
    cout << "=================================================" << endl;

    PatientLinkedList patientList;
    StaffMember staffArray[MAX_STAFF];
    int numberOfStaff = 0;
    Appointment appointmentArray[MAX_APPOINTMENTS];
    int numberOfAppointments = 0;
    Prescription prescriptionArray[MAX_PRESCRIPTIONS];
    int numberOfPrescriptions = 0;
    MedicineStock medicineStockArray[TOTAL_MEDICINE_TYPES];
    DoctorSchedule doctorScheduleList[TOTAL_DOCTORS];

    // the doctor schedule is fixed, made up by us, so we just set it up directly instead of loading it from a file
    setupDoctorScheduleList(doctorScheduleList);

    // try to load whatever data was saved from last time. if a file is
    // missing or something goes wrong, we still let the program keep
    // running instead of just crashing before the menu even shows up
    try {
        loadPatientsFromFile(patientList, FILE_PATIENTS);
        numberOfStaff = loadStaffFromFile(staffArray, MAX_STAFF, FILE_STAFF);
        numberOfAppointments = loadAppointmentsFromFile(appointmentArray, MAX_APPOINTMENTS, FILE_APPOINTMENTS);
        numberOfPrescriptions = loadPrescriptionsFromFile(prescriptionArray, MAX_PRESCRIPTIONS, FILE_PRESCRIPTIONS);
        setupMedicineStock(medicineStockArray);

        generateSampleData(patientList);
        generateDefaultAdmin(staffArray, numberOfStaff);

    } catch (FileException& fileProblem) {
        cout << "Something went wrong while loading the files: " << fileProblem.what() << endl;
    }

    int mainChoice = -1;

    do {
        cout << "\n========== MAIN MENU ==========" << endl;
        cout << "1. Staff / Admin Module" << endl;
        cout << "2. Customer / Patient Module" << endl;
        cout << "3. Help / About This System" << endl;
        cout << "0. Exit System" << endl;
        cout << "Enter choice: ";

        // same try/catch idea as the other modules - if something goes
        // wrong we just print the message and show the menu again
        try {
            cin >> mainChoice;

            if (mainChoice == 1) {
                staffModuleEntry(patientList, staffArray, numberOfStaff, appointmentArray, numberOfAppointments,
                                 prescriptionArray, numberOfPrescriptions, medicineStockArray, doctorScheduleList);
            } else if (mainChoice == 2) {
                customerModuleEntry(patientList, appointmentArray, numberOfAppointments,
                                    doctorScheduleList, prescriptionArray, numberOfPrescriptions);
            } else if (mainChoice == 3) {
                displayHelpScreen();
            } else if (mainChoice == 0) {
                cout << "Thank you for using the system. Goodbye!" << endl;
            } else {
                cout << "That is not a valid choice, please try again." << endl;
            }

        } catch (InputException& inputProblem) {
            cout << "Input error: " << inputProblem.what() << endl;
        } catch (FileException& fileProblem) {
            cout << "File error: " << fileProblem.what() << endl;
        }

    } while (mainChoice != 0);

    return 0;
}
