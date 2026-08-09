#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/utils.h"
#include "headers/validation.h"
#include "headers/user.h"
#include "headers/auth.h"
#include "headers/patient.h"
#include "headers/doctor.h"
#include "headers/appointment.h"
#include "headers/billing.h"

/* ---- Forward declarations of menu functions, grouped by role ---- */
static void adminMenu(User **userHead, Patient **patientHead, Doctor **doctorHead, Appointment **apptHead, Bill **billHead);
static void employeeMenu(Patient **patientHead, Doctor **doctorHead, Appointment **apptHead, Bill **billHead);
static void doctorMenu(Doctor *doctorHead, Appointment *apptHead, Patient *patientHead, int linkedDoctorId);

/* ---- Admin sub-menus ---- */
static void adminUserMenu(User **userHead);
static void adminPatientMenu(Patient **patientHead);
static void adminDoctorMenu(Doctor **doctorHead);
static void adminAppointmentMenu(Appointment **apptHead, Patient *patientHead, Doctor *doctorHead);
static void adminBillingMenu(Bill **billHead, Patient *patientHead);
static void adminReportsMenu(Patient *patientHead, Doctor *doctorHead, Appointment *apptHead, Bill *billHead);

/* ---- Shared input helpers used by multiple menus ---- */
static int promptValidatedDate(char *buffer, int size);
static int promptValidatedTime(char *buffer, int size);

/* one-time splash screen  */
static void showSplashScreen(void)
{
    clearScreen();
    printf("=================================================\n");
    printf("       HOSPITAL MANAGEMENT SYSTEM\n");
    printf("       Software Development-I (CSE 1290)\n");
    printf("       Northern University Bangladesh\n");
    printf("=================================================\n");
    printf("  Developed By: Mohammad Mahfuz Rahman (41250102605)\n");
    printf("  Section: 5B\n");
    printf("=================================================\n");
    pauseScreen();
}

int main(void)
{
    showSplashScreen();

    User *userHead = loadUsers();
    Patient *patientHead = loadPatients();
    Doctor *doctorHead = loadDoctors();
    Appointment *apptHead = loadAppointments();
    Bill *billHead = loadBills();

    userHead = ensureDefaultAdmin(userHead);

    int running = 1;
    while (running)
    {
        User *loggedInUser = login(userHead);

        if (loggedInUser == NULL)
        {
            /* Either the user gave up, or exhausted login
             * attempts. Either way, exit the program. */
            running = 0;
            break;
        }

        switch (loggedInUser->role)
        {
        case ROLE_ADMIN:
            adminMenu(&userHead, &patientHead, &doctorHead, &apptHead, &billHead);
            break;

        case ROLE_EMPLOYEE:
            employeeMenu(&patientHead, &doctorHead, &apptHead, &billHead);
            break;

        case ROLE_DOCTOR:
            doctorMenu(doctorHead, apptHead, patientHead, loggedInUser->linkedDoctorId);
            break;
        }

        /* After any menu returns (user chose "logout"), loop
         * back to the login screen rather than exiting, so
         * multiple staff can use the same running program. */
        clearScreen();
        printf("Logged out.\n");
        pauseScreen();
    }

    /* Final save, in case anything changed during the session
     * that wasn't already saved by an individual CRUD action.
     * (Each module also saves immediately after every change —
     * this is a safety net, not the primary save mechanism.) */
    saveUsers(userHead);
    savePatients(patientHead);
    saveDoctors(doctorHead);
    saveAppointments(apptHead);
    saveBills(billHead);

    freeUsers(userHead);
    freePatients(patientHead);
    freeDoctors(doctorHead);
    freeAppointments(apptHead);
    freeBills(billHead);

    printf("\nThank you for using the Hospital Management System. Goodbye.\n");
    printf("Developed by Mohammad Mahfuz Rahman (41250102605)\n");
    return 0;
}

/* ============================================================
 * Shared input helpers
 * ============================================================ */

/* Repeats the prompt until isValidDate() accepts the input.
 * Returns 1 once a valid date is stored in buffer (always
 * succeeds eventually, since it loops on invalid input). */
static int promptValidatedDate(char *buffer, int size)
{
    while (1)
    {
        printf("Enter date (DD-MM-YYYY): ");
        readLine(buffer, size);
        if (isValidDate(buffer))
        {
            return 1;
        }
        printf("Invalid date format. Example: 22-06-2026\n");
    }
}

static int promptValidatedTime(char *buffer, int size)
{
    while (1)
    {
        printf("Enter time (HH:MM, 24-hour): ");
        readLine(buffer, size);
        if (isValidTime(buffer))
        {
            return 1;
        }
        printf("Invalid time format. Example: 14:30\n");
    }
}

/* ============================================================
 * ADMIN MENU — full access to every module
 * ============================================================
 */

static void adminMenu(User **userHead, Patient **patientHead, Doctor **doctorHead,
                      Appointment **apptHead, Bill **billHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("            ADMINISTRATOR MENU\n");
        printf("==========================================\n");
        printf("1. Manage Users (Employees / Doctors / Admins)\n");
        printf("2. Manage Patients\n");
        printf("3. Manage Doctors\n");
        printf("4. Manage Appointments\n");
        printf("5. Manage Billing\n");
        printf("6. Reports\n");
        printf("0. Logout\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
            adminUserMenu(userHead);
            break;
        case 2:
            adminPatientMenu(patientHead);
            break;
        case 3:
            adminDoctorMenu(doctorHead);
            break;
        case 4:
            adminAppointmentMenu(apptHead, *patientHead, *doctorHead);
            break;
        case 5:
            adminBillingMenu(billHead, *patientHead);
            break;
        case 6:
            adminReportsMenu(*patientHead, *doctorHead, *apptHead, *billHead);
            break;
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

static void adminUserMenu(User **userHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("              MANAGE USERS\n");
        printf("==========================================\n");
        printf("1. Add User\n");
        printf("2. View All Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("0. Back\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            char username[USERNAME_LEN], password[PASSWORD_LEN];
            int roleChoice, linkedDoctorId = -1;

            printf("Username: ");
            readLine(username, USERNAME_LEN);
            printf("Password: ");
            readLine(password, PASSWORD_LEN);

            printf("Role (0=Admin, 1=Employee, 2=Doctor): ");
            roleChoice = readInt("");
            if (roleChoice < 0 || roleChoice > 2)
            {
                printf("Invalid role. Defaulting to Employee.\n");
                roleChoice = ROLE_EMPLOYEE;
            }

            if (roleChoice == ROLE_DOCTOR)
            {
                linkedDoctorId = readInt("Enter the linked Doctor ID (from Doctor records): ");
            }

            *userHead = addUser(*userHead, username, password,
                                (Role)roleChoice, linkedDoctorId);
            saveUsers(*userHead);
            printf("User created successfully.\n");
            pauseScreen();
            break;
        }
        case 2:
        {
            printf("%-5s %-20s %-15s %-12s\n", "ID", "Username", "Role", "LinkedDocID");
            printf("--------------------------------------------------------\n");
            for (User *curr = *userHead; curr != NULL; curr = curr->next)
            {
                printf("%-5d %-20s %-15s %-12d\n",
                       curr->id, curr->username, roleToString(curr->role),
                       curr->linkedDoctorId);
            }
            pauseScreen();
            break;
        }
        case 3:
        {
            int id = readInt("Enter User ID to update: ");
            User *existing = findUserById(*userHead, id);
            if (existing == NULL)
            {
                printf("No user found with that ID.\n");
                pauseScreen();
                break;
            }

            printf("Current username: %s\n", existing->username);
            printf("Current role: %s\n", roleToString(existing->role));
            if (existing->role == ROLE_DOCTOR)
            {
                printf("Current linked Doctor ID: %d\n", existing->linkedDoctorId);
            }

            char username[USERNAME_LEN];
            int roleChoice, linkedDoctorId = -1;

            printf("New Username: ");
            readLine(username, USERNAME_LEN);

            printf("New Role (0=Admin, 1=Employee, 2=Doctor): ");
            roleChoice = readInt("");
            if (roleChoice < 0 || roleChoice > 2)
            {
                printf("Invalid role. Defaulting to Employee.\n");
                roleChoice = ROLE_EMPLOYEE;
            }

            if (roleChoice == ROLE_DOCTOR)
            {
                linkedDoctorId = readInt("Enter the linked Doctor ID (from Doctor records): ");
            }

            updateUser(*userHead, id, username, (Role)roleChoice, linkedDoctorId);

            /* Password is changed only on explicit confirmation,
             * so an admin who just wants to fix a typo'd username
             * or role doesn't accidentally reset a working
             * password to a blank/forgotten value. */
            int changePassword = readInt("Change password? (1=Yes, 0=No): ");
            if (changePassword == 1)
            {
                char newPassword[PASSWORD_LEN];
                printf("New Password: ");
                readLine(newPassword, PASSWORD_LEN);
                setUserPassword(*userHead, id, newPassword);
            }

            saveUsers(*userHead);
            printf("User updated successfully.\n");
            pauseScreen();
            break;
        }
        case 4:
        {
            int id = readInt("Enter User ID to delete: ");
            *userHead = deleteUser(*userHead, id);
            saveUsers(*userHead);
            printf("User deleted (if it existed).\n");
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

static void adminPatientMenu(Patient **patientHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("            MANAGE PATIENTS\n");
        printf("==========================================\n");
        printf("1. Add Patient\n");
        printf("2. View All Patients\n");
        printf("3. Update Patient\n");
        printf("4. Delete Patient\n");
        printf("5. Search by Name (linear search)\n");
        printf("6. Search by ID (binary search)\n");
        printf("0. Back\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            char name[PATIENT_NAME_LEN], gender[PATIENT_GENDER_LEN];
            char phone[PATIENT_PHONE_LEN], address[PATIENT_ADDRESS_LEN];
            char diagnosis[PATIENT_DIAGNOSIS_LEN];
            int age;

            printf("Name: ");
            readLine(name, PATIENT_NAME_LEN);
            age = readInt("Age: ");
            printf("Gender: ");
            readLine(gender, PATIENT_GENDER_LEN);

            do
            {
                printf("Phone (digits only, 7-15 chars): ");
                readLine(phone, PATIENT_PHONE_LEN);
            } while (!isValidPhone(phone));

            printf("Address: ");
            readLine(address, PATIENT_ADDRESS_LEN);
            printf("Diagnosis: ");
            readLine(diagnosis, PATIENT_DIAGNOSIS_LEN);

            *patientHead = addPatient(*patientHead, name, age, gender,
                                      phone, address, diagnosis);
            savePatients(*patientHead);
            printf("Patient added successfully.\n");
            pauseScreen();
            break;
        }
        case 2:
            displayAllPatients(*patientHead);
            pauseScreen();
            break;
        case 3:
        {
            int id = readInt("Enter Patient ID to update: ");
            Patient *existing = findPatientById(*patientHead, id);
            if (existing == NULL)
            {
                printf("No patient found with that ID.\n");
                pauseScreen();
                break;
            }

            char name[PATIENT_NAME_LEN], gender[PATIENT_GENDER_LEN];
            char phone[PATIENT_PHONE_LEN], address[PATIENT_ADDRESS_LEN];
            char diagnosis[PATIENT_DIAGNOSIS_LEN];
            int age;

            printf("New Name: ");
            readLine(name, PATIENT_NAME_LEN);
            age = readInt("New Age: ");
            printf("New Gender: ");
            readLine(gender, PATIENT_GENDER_LEN);

            do
            {
                printf("New Phone (digits only, 7-15 chars): ");
                readLine(phone, PATIENT_PHONE_LEN);
            } while (!isValidPhone(phone));

            printf("New Address: ");
            readLine(address, PATIENT_ADDRESS_LEN);
            printf("New Diagnosis: ");
            readLine(diagnosis, PATIENT_DIAGNOSIS_LEN);

            updatePatient(*patientHead, id, name, age, gender, phone, address, diagnosis);
            savePatients(*patientHead);
            printf("Patient updated successfully.\n");
            pauseScreen();
            break;
        }
        case 4:
        {
            int id = readInt("Enter Patient ID to delete: ");
            *patientHead = deletePatient(*patientHead, id);
            savePatients(*patientHead);
            printf("Patient deleted (if it existed).\n");
            pauseScreen();
            break;
        }
        case 5:
        {
            char namePart[PATIENT_NAME_LEN];
            printf("Enter name (or part of it) to search: ");
            readLine(namePart, PATIENT_NAME_LEN);
            Patient *result = searchPatientByName(*patientHead, namePart);
            displayPatient(result);
            pauseScreen();
            break;
        }
        case 6:
        {
            int id = readInt("Enter Patient ID to search: ");
            int comparisons;
            Patient *result = searchPatientByIdBinary(*patientHead, id, &comparisons);
            displayPatient(result);
            printf("(Binary search completed in %d comparison(s).)\n", comparisons);
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

static void adminDoctorMenu(Doctor **doctorHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("             MANAGE DOCTORS\n");
        printf("==========================================\n");
        printf("1. Add Doctor\n");
        printf("2. View All Doctors\n");
        printf("3. Update Doctor\n");
        printf("4. Delete Doctor\n");
        printf("5. Search by Name (linear search)\n");
        printf("6. Toggle Availability\n");
        printf("0. Back\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            char name[DOCTOR_NAME_LEN], specialization[DOCTOR_SPECIALIZATION_LEN];
            char phone[DOCTOR_PHONE_LEN];
            int availChoice;

            printf("Name: ");
            readLine(name, DOCTOR_NAME_LEN);
            printf("Specialization: ");
            readLine(specialization, DOCTOR_SPECIALIZATION_LEN);

            do
            {
                printf("Phone (digits only, 7-15 chars): ");
                readLine(phone, DOCTOR_PHONE_LEN);
            } while (!isValidPhone(phone));

            availChoice = readInt("Availability (1=Available, 0=Busy): ");

            *doctorHead = addDoctor(*doctorHead, name, specialization, phone,
                                    availChoice ? DOCTOR_AVAILABLE : DOCTOR_BUSY);
            saveDoctors(*doctorHead);
            printf("Doctor added successfully.\n");
            pauseScreen();
            break;
        }
        case 2:
            displayAllDoctors(*doctorHead);
            pauseScreen();
            break;
        case 3:
        {
            int id = readInt("Enter Doctor ID to update: ");
            Doctor *existing = findDoctorById(*doctorHead, id);
            if (existing == NULL)
            {
                printf("No doctor found with that ID.\n");
                pauseScreen();
                break;
            }

            char name[DOCTOR_NAME_LEN], specialization[DOCTOR_SPECIALIZATION_LEN];
            char phone[DOCTOR_PHONE_LEN];
            int availChoice;

            printf("New Name: ");
            readLine(name, DOCTOR_NAME_LEN);
            printf("New Specialization: ");
            readLine(specialization, DOCTOR_SPECIALIZATION_LEN);

            do
            {
                printf("New Phone (digits only, 7-15 chars): ");
                readLine(phone, DOCTOR_PHONE_LEN);
            } while (!isValidPhone(phone));

            availChoice = readInt("Availability (1=Available, 0=Busy): ");

            updateDoctor(*doctorHead, id, name, specialization, phone,
                         availChoice ? DOCTOR_AVAILABLE : DOCTOR_BUSY);
            saveDoctors(*doctorHead);
            printf("Doctor updated successfully.\n");
            pauseScreen();
            break;
        }
        case 4:
        {
            int id = readInt("Enter Doctor ID to delete: ");
            *doctorHead = deleteDoctor(*doctorHead, id);
            saveDoctors(*doctorHead);
            printf("Doctor deleted (if it existed).\n");
            pauseScreen();
            break;
        }
        case 5:
        {
            char namePart[DOCTOR_NAME_LEN];
            printf("Enter name (or part of it) to search: ");
            readLine(namePart, DOCTOR_NAME_LEN);
            Doctor *result = searchDoctorByName(*doctorHead, namePart);
            displayDoctor(result);
            pauseScreen();
            break;
        }
        case 6:
        {
            int id = readInt("Enter Doctor ID to toggle: ");
            Doctor *d = findDoctorById(*doctorHead, id);
            if (d == NULL)
            {
                printf("No doctor found with that ID.\n");
            }
            else
            {
                Availability newAvail = (d->availability == DOCTOR_AVAILABLE)
                                            ? DOCTOR_BUSY
                                            : DOCTOR_AVAILABLE;
                setDoctorAvailability(*doctorHead, id, newAvail);
                saveDoctors(*doctorHead);
                printf("Doctor %s is now %s.\n", d->name, availabilityToString(newAvail));
            }
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

static void adminAppointmentMenu(Appointment **apptHead, Patient *patientHead, Doctor *doctorHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("           MANAGE APPOINTMENTS\n");
        printf("==========================================\n");
        printf("1. Book Appointment (by specialization)\n");
        printf("2. View All Appointments\n");
        printf("3. Update Appointment Status\n");
        printf("4. Cancel/Delete Appointment\n");
        printf("5. View Appointments by Patient ID\n");
        printf("6. Search by Date (binary search)\n");
        printf("0. Back\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            int patientId = readInt("Enter Patient ID: ");
            Patient *p = findPatientById(patientHead, patientId);
            if (p == NULL)
            {
                printf("No patient found with that ID. Add the patient first.\n");
                pauseScreen();
                break;
            }

            char specialization[DOCTOR_SPECIALIZATION_LEN];
            printf("Required specialization (e.g. Cardiology): ");
            readLine(specialization, DOCTOR_SPECIALIZATION_LEN);

            /* This is the core business-rule lookup: only an
             * AVAILABLE doctor matching the specialization is
             * offered — see doctor.c for the implementation. */
            Doctor *d = findAvailableDoctorBySpecialization(doctorHead, specialization);
            if (d == NULL)
            {
                printf("No available doctor found for specialization '%s'.\n", specialization);
                pauseScreen();
                break;
            }

            printf("Matched doctor: Dr. %s (ID %d, %s)\n", d->name, d->id, d->specialization);

            char date[APPOINTMENT_DATE_LEN], time[APPOINTMENT_TIME_LEN];
            promptValidatedDate(date, APPOINTMENT_DATE_LEN);
            promptValidatedTime(time, APPOINTMENT_TIME_LEN);

            /* Double-booking guard: refuse to create the
             * appointment if this doctor already has a
             * non-cancelled appointment at the exact same
             * date+time. */
            if (isDoctorAvailableAt(*apptHead, d->id, date, time))
            {
                printf("Dr. %s already has an appointment at %s %s. Please choose a different date/time.\n",
                       d->name, date, time);
                pauseScreen();
                break;
            }

            *apptHead = addAppointment(*apptHead, patientId, d->id, date, time);
            saveAppointments(*apptHead);
            printf("Appointment booked successfully with Dr. %s.\n", d->name);
            pauseScreen();
            break;
        }
        case 2:
            displayAllAppointments(*apptHead);
            pauseScreen();
            break;
        case 3:
        {
            int id = readInt("Enter Appointment ID: ");
            int statusChoice = readInt("New status (0=Scheduled, 1=Completed, 2=Cancelled): ");
            if (statusChoice < 0 || statusChoice > 2)
            {
                printf("Invalid status.\n");
            }
            else if (updateAppointmentStatus(*apptHead, id, (AppointmentStatus)statusChoice))
            {
                saveAppointments(*apptHead);
                printf("Status updated.\n");
            }
            else
            {
                printf("No appointment found with that ID.\n");
            }
            pauseScreen();
            break;
        }
        case 4:
        {
            int id = readInt("Enter Appointment ID to delete: ");
            *apptHead = deleteAppointment(*apptHead, id);
            saveAppointments(*apptHead);
            printf("Appointment deleted (if it existed).\n");
            pauseScreen();
            break;
        }
        case 5:
        {
            int patientId = readInt("Enter Patient ID: ");
            displayAppointmentsByPatientId(*apptHead, patientId);
            pauseScreen();
            break;
        }
        case 6:
        {
            char date[APPOINTMENT_DATE_LEN];
            promptValidatedDate(date, APPOINTMENT_DATE_LEN);
            int comparisons;
            Appointment *result = searchAppointmentByDateBinary(*apptHead, date, &comparisons);
            displayAppointment(result);
            printf("(Binary search completed in %d comparison(s).)\n", comparisons);
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

static void adminBillingMenu(Bill **billHead, Patient *patientHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("             MANAGE BILLING\n");
        printf("==========================================\n");
        printf("1. Create Bill\n");
        printf("2. View All Bills\n");
        printf("3. View Receipt by Bill ID\n");
        printf("4. View Bills by Patient ID\n");
        printf("5. Delete Bill\n");
        printf("0. Back\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            int patientId = readInt("Enter Patient ID: ");
            if (findPatientById(patientHead, patientId) == NULL)
            {
                printf("No patient found with that ID.\n");
                pauseScreen();
                break;
            }

            float consultationFee = readFloat("Consultation Fee: ");
            float medicineCost = readFloat("Medicine Cost: ");
            float otherCharges = readFloat("Other Charges: ");

            char date[BILL_DATE_LEN];
            promptValidatedDate(date, BILL_DATE_LEN);

            *billHead = addBill(*billHead, patientId, consultationFee,
                                medicineCost, otherCharges, date);
            saveBills(*billHead);
            printf("Bill created successfully.\n");
            pauseScreen();
            break;
        }
        case 2:
            displayAllBills(*billHead);
            pauseScreen();
            break;
        case 3:
        {
            int id = readInt("Enter Bill ID: ");
            displayBillReceipt(findBillById(*billHead, id));
            pauseScreen();
            break;
        }
        case 4:
        {
            int patientId = readInt("Enter Patient ID: ");
            displayBillsByPatientId(*billHead, patientId);
            pauseScreen();
            break;
        }
        case 5:
        {
            int id = readInt("Enter Bill ID to delete: ");
            *billHead = deleteBill(*billHead, id);
            saveBills(*billHead);
            printf("Bill deleted (if it existed).\n");
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

static void adminReportsMenu(Patient *patientHead, Doctor *doctorHead,
                             Appointment *apptHead, Bill *billHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("                 REPORTS\n");
        printf("==========================================\n");
        printf("1. Patient Summary\n");
        printf("2. Doctor List\n");
        printf("3. Appointment Report\n");
        printf("4. Billing Summary\n");
        printf("0. Back\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            int count = 0;
            for (Patient *p = patientHead; p != NULL; p = p->next)
                count++;
            printf("Total registered patients: %d\n\n", count);
            displayAllPatients(patientHead);
            pauseScreen();
            break;
        }
        case 2:
        {
            int total = 0, available = 0;
            for (Doctor *d = doctorHead; d != NULL; d = d->next)
            {
                total++;
                if (d->availability == DOCTOR_AVAILABLE)
                    available++;
            }
            printf("Total doctors: %d | Available: %d | Busy: %d\n\n",
                   total, available, total - available);
            displayAllDoctors(doctorHead);
            pauseScreen();
            break;
        }
        case 3:
        {
            int scheduled = 0, completed = 0, cancelled = 0;
            for (Appointment *a = apptHead; a != NULL; a = a->next)
            {
                if (a->status == APPT_SCHEDULED)
                    scheduled++;
                else if (a->status == APPT_COMPLETED)
                    completed++;
                else
                    cancelled++;
            }
            printf("Scheduled: %d | Completed: %d | Cancelled: %d\n\n",
                   scheduled, completed, cancelled);
            displayAllAppointments(apptHead);
            pauseScreen();
            break;
        }
        case 4:
        {
            float grandTotal = 0.0f;
            int count = 0;
            for (Bill *b = billHead; b != NULL; b = b->next)
            {
                grandTotal += b->totalAmount;
                count++;
            }
            printf("Total bills: %d | Combined revenue: %.2f\n\n", count, grandTotal);
            displayAllBills(billHead);
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

/* ============================================================
 * EMPLOYEE MENU — patients, appointment booking, billing.
 * No access to doctor records or user account management.
 * ============================================================ */

static void employeeMenu(Patient **patientHead, Doctor **doctorHead,
                         Appointment **apptHead, Bill **billHead)
{
    int choice;

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("              EMPLOYEE MENU\n");
        printf("==========================================\n");
        printf("1. Add Patient\n");
        printf("2. View All Patients\n");
        printf("3. Search Patient by Name\n");
        printf("4. Book Appointment (by specialization)\n");
        printf("5. View All Appointments\n");
        printf("6. Create Bill\n");
        printf("7. View Receipt by Bill ID\n");
        printf("0. Logout\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
        {
            char name[PATIENT_NAME_LEN], gender[PATIENT_GENDER_LEN];
            char phone[PATIENT_PHONE_LEN], address[PATIENT_ADDRESS_LEN];
            char diagnosis[PATIENT_DIAGNOSIS_LEN];
            int age;

            printf("Name: ");
            readLine(name, PATIENT_NAME_LEN);
            age = readInt("Age: ");
            printf("Gender: ");
            readLine(gender, PATIENT_GENDER_LEN);

            do
            {
                printf("Phone (digits only, 7-15 chars): ");
                readLine(phone, PATIENT_PHONE_LEN);
            } while (!isValidPhone(phone));

            printf("Address: ");
            readLine(address, PATIENT_ADDRESS_LEN);
            printf("Diagnosis: ");
            readLine(diagnosis, PATIENT_DIAGNOSIS_LEN);

            *patientHead = addPatient(*patientHead, name, age, gender,
                                      phone, address, diagnosis);
            savePatients(*patientHead);
            printf("Patient added successfully.\n");
            pauseScreen();
            break;
        }
        case 2:
            displayAllPatients(*patientHead);
            pauseScreen();
            break;
        case 3:
        {
            char namePart[PATIENT_NAME_LEN];
            printf("Enter name (or part of it) to search: ");
            readLine(namePart, PATIENT_NAME_LEN);
            displayPatient(searchPatientByName(*patientHead, namePart));
            pauseScreen();
            break;
        }
        case 4:
        {
            int patientId = readInt("Enter Patient ID: ");
            Patient *p = findPatientById(*patientHead, patientId);
            if (p == NULL)
            {
                printf("No patient found with that ID. Add the patient first.\n");
                pauseScreen();
                break;
            }

            char specialization[DOCTOR_SPECIALIZATION_LEN];
            printf("Required specialization (e.g. Cardiology): ");
            readLine(specialization, DOCTOR_SPECIALIZATION_LEN);

            Doctor *d = findAvailableDoctorBySpecialization(*doctorHead, specialization);
            if (d == NULL)
            {
                printf("No available doctor found for specialization '%s'.\n", specialization);
                pauseScreen();
                break;
            }

            printf("Matched doctor: Dr. %s (ID %d, %s)\n", d->name, d->id, d->specialization);

            char date[APPOINTMENT_DATE_LEN], time[APPOINTMENT_TIME_LEN];
            promptValidatedDate(date, APPOINTMENT_DATE_LEN);
            promptValidatedTime(time, APPOINTMENT_TIME_LEN);

            /* Double-booking guard: same check used in the
             * admin's booking flow, so the rule applies
             * consistently no matter who books the appointment. */
            if (isDoctorAvailableAt(*apptHead, d->id, date, time))
            {
                printf("Dr. %s already has an appointment at %s %s. Please choose a different date/time.\n",
                       d->name, date, time);
                pauseScreen();
                break;
            }

            *apptHead = addAppointment(*apptHead, patientId, d->id, date, time);
            saveAppointments(*apptHead);
            printf("Appointment booked successfully with Dr. %s.\n", d->name);
            pauseScreen();
            break;
        }
        case 5:
            displayAllAppointments(*apptHead);
            pauseScreen();
            break;
        case 6:
        {
            int patientId = readInt("Enter Patient ID: ");
            if (findPatientById(*patientHead, patientId) == NULL)
            {
                printf("No patient found with that ID.\n");
                pauseScreen();
                break;
            }

            float consultationFee = readFloat("Consultation Fee: ");
            float medicineCost = readFloat("Medicine Cost: ");
            float otherCharges = readFloat("Other Charges: ");

            char date[BILL_DATE_LEN];
            promptValidatedDate(date, BILL_DATE_LEN);

            *billHead = addBill(*billHead, patientId, consultationFee,
                                medicineCost, otherCharges, date);
            saveBills(*billHead);
            printf("Bill created successfully.\n");
            pauseScreen();
            break;
        }
        case 7:
        {
            int id = readInt("Enter Bill ID: ");
            displayBillReceipt(findBillById(*billHead, id));
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}

/* ============================================================
 * DOCTOR MENU — read-only, restricted to the logged-in doctor's
 * OWN data. linkedDoctorId comes from the User record and is
 * used to filter every query, so a doctor can never see another
 * doctor's patients or appointments.
 * ============================================================ */

static void doctorMenu(Doctor *doctorHead, Appointment *apptHead, Patient *patientHead,
                       int linkedDoctorId)
{
    int choice;

    Doctor *self = findDoctorById(doctorHead, linkedDoctorId);
    if (self == NULL)
    {
        printf("Your account is not linked to a valid Doctor record.\n");
        printf("Please contact an Administrator.\n");
        pauseScreen();
        return;
    }

    do
    {
        clearScreen();
        printf("==========================================\n");
        printf("   DOCTOR MENU - Dr. %s\n", self->name);
        printf("==========================================\n");
        printf("1. View My Appointments\n");
        printf("2. View a Specific Patient's Details\n");
        printf("0. Logout\n");
        choice = readInt("Select an option: ");

        switch (choice)
        {
        case 1:
            /* Filtered strictly by this doctor's own id. */
            displayAppointmentsByDoctorId(apptHead, linkedDoctorId);
            pauseScreen();
            break;
        case 2:
        {
            int patientId = readInt("Enter Patient ID: ");

            /* Authorization check: a doctor may only view a
             * patient's details if that patient has at least
             * one appointment WITH this doctor. This prevents
             * a doctor from looking up any patient in the
             * hospital, not just their own. */
            int isMyPatient = 0;
            for (Appointment *a = apptHead; a != NULL; a = a->next)
            {
                if (a->patientId == patientId && a->doctorId == linkedDoctorId)
                {
                    isMyPatient = 1;
                    break;
                }
            }

            if (!isMyPatient)
            {
                printf("This patient is not assigned to you.\n");
            }
            else
            {
                displayPatient(findPatientById(patientHead, patientId));
            }
            pauseScreen();
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid option.\n");
            pauseScreen();
        }
    } while (choice != 0);
}