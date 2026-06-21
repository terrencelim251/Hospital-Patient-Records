# Hospital Patient Record Management System — README

**TDS4223 Data Structure and Algorithms — Group 4**

A console-based C++ hospital management system (no STL used), with two modules: Staff/Admin and Customer/Patient.

---

## How to Run

1. Open `Project_GR4.cpp` in Dev-C++ 5.11 (or any C++98-compatible compiler).
2. Click **Execute → Compile & Run** (or press **F9**).
3. The program will start automatically in a console window.

**First run:** the system automatically creates 50 sample patient records and one default admin account, saved into `.txt` files in the same folder as the program. These records will reload automatically every time you run the program again.

**If you want a completely fresh dataset:** close the program, delete all `.txt` files in the project folder, then run again.

---

## Login Instructions

### Staff / Admin Login
From the Main Menu: select **1 (Staff / Admin Module)** → **2 (Login)**

| Field | Value |
|---|---|
| Username | `admin` |
| Password | `admin123` |

### Customer / Patient Login
From the Main Menu: select **2 (Customer / Patient Module)** → **2 (Login)**

You can log in with any of the 50 auto-generated sample patients. The pattern is:
- Username: `patientXXXX`
- Password: `passXXXX`

where `XXXX` is the patient's ID, starting from 1001. For example:

| Field | Value |
|---|---|
| Username | `patient1001` |
| Password | `pass1001` |

You may also register a brand-new patient account from the Customer/Patient Module by choosing option 1 (Register).

---

## Full Menu Reference

### Staff / Admin Module (after login)

| # | Menu Option | What it does |
|---|---|---|
| 1 | Add New Patient Record | Register a new patient and admit them to a ward (checks ward capacity, max 4 per ward) |
| 2 | Edit Patient Record | Change a patient's diagnosis, assigned doctor, ward number, or status |
| 3 | Delete Patient Record | Permanently remove a patient record (asks for confirmation first) |
| 4 | Display All Patient Records | List every patient currently in the system |
| 5 | Search Patient Record | Find a patient by ID (Binary Search) or by name (linear search) |
| 6 | Sort Patient Records | Sort and display all patients by name or by ward number (Selection Sort) |
| 7 | Generate / View Summary Report | Show and save a count of Registered / Admitted / Discharged patients |
| 8 | View All Appointments | List every appointment booked by any patient |
| 9 | Update Appointment Status | Change an appointment's status (Pending / Confirmed / Completed / Cancelled) |
| 10 | Approve Ward Transfer | Move a patient to a different ward (checks the new ward has space) |
| 11 | Add Prescription For A Patient | Prescribe medicine to a patient; deducts the amount from pharmacy stock |
| 12 | View A Patient's Prescriptions | List all prescriptions for one specific patient (enter their Patient ID) |
| 13 | Generate Patient Bill | Calculate and save a bill (ward cost + medicine cost + consultation fees) |
| 14 | View Ward Occupancy Report | Show how many beds are filled/free in each of the 10 wards |
| 15 | Restock Medicine | Add more units to a medicine's pharmacy stock |
| 16 | Sort All Prescriptions | Sort and display all prescriptions by date or by Patient ID (Selection Sort) |
| 17 | Search Prescriptions By Medicine | Find prescriptions by medicine name (or part of the name) |
| 18 | Discharge Patient | Mark a patient as discharged, freeing their ward bed; can generate a final bill |
| 19 | View All Staff Accounts | List every registered staff/admin account |
| 20 | View All Prescriptions | List every prescription in the system, for every patient |
| 21 | View Most Prescribed Medicine Report | Show how many times each medicine has been prescribed |
| 22 | View System Statistics | Show overall totals: patients, staff, appointments, prescriptions, low stock, full wards |
| 0 | Logout | Return to the Main Menu |

### Customer / Patient Module (after login)

| # | Menu Option | What it does |
|---|---|---|
| 1 | View My Profile | Show your own patient details |
| 2 | Book New Appointment | Pick a doctor, day, date, and time for a new appointment |
| 3 | Edit My Appointment | Change the date/time or reason for one of your own appointments |
| 4 | Cancel My Appointment | Cancel one of your own appointments (asks for confirmation first) |
| 5 | Display My Bookings | Show all your bookings, or only the active ones (Pending/Confirmed) |
| 6 | Search My Bookings | Find one of your own bookings by Appointment ID or by doctor name |
| 7 | Sort My Bookings | Sort and display your own bookings by date or by doctor name |
| 8 | Generate / View Booking Summary Report | Show and save a count of your bookings by status |
| 9 | View My Prescriptions | List all medicine prescribed to you |
| 10 | View My Latest Bill | Show your own most recently generated bill |
| 11 | View Doctor Schedule | Show all 6 doctors, their working days, and consultation fees |
| 0 | Logout | Return to the Main Menu |

**Tip for testing:** Entering an invalid choice, a Patient ID that doesn't exist, or text into a numeric field will not crash the program — the system catches these and shows an error message, then returns to the menu.

---

## Notes & Troubleshooting

- All data is saved automatically to `.txt` files in the program's folder and reloaded the next time it runs.
- If you see a message like "skipped N corrupted line(s)", this usually means an old or incompatible `.txt` file is left over from a previous test run. Deleting all `.txt` files and restarting will generate a clean, valid dataset.
- Please close the program fully (choose Exit from the Main Menu) before running it again, rather than closing the console window directly, to make sure your data is saved properly.
- The system uses exception handling throughout, so invalid input (e.g. typing letters into a numeric field, or entering a Patient ID that doesn't exist) will show a clear error message and return to the menu instead of crashing.
