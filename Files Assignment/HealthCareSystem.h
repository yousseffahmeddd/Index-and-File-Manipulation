#pragma once
#include<iostream>
#include <vector>
#include <unordered_map>
#include "Doctor.h"
#include "Appointment.h"

using namespace std;
class HealthcareSystem {
private:
    vector<Doctor> doctors;
    vector<Appointment> appointments;
    vector<int> doctorAvailList;
    vector<int> appointmentAvailList;

    //maps Doctor ID to index in `doctors`
    unordered_map<string, int> doctorPrimaryIndex;

    //maps Appointment ID to index in `appointments`
    unordered_map<string, int> appointmentPrimaryIndex;

    //maps Doctor Name to doctor indexes
    unordered_map<string, vector<int>> doctorSecondaryIndex;

    //maps Doctor ID to appointment indexes
    unordered_map<string, vector<int>> doctorAppointmentIndex;

public:
    void addDoctor() {
    }

    void deleteDoctor() {
    }

    void updateDoctorName() {
    }

    void addAppointment() {
    }

    void updateAppointmentDate() {
    }

    void deleteAppointment() {
    }

    void printDoctorInfo() {
    }

    void printAppointmentInfo() {
    }

    void handleQueries() {
    }

    void menu() {
        int choice;
        do {
            cout << endl << "--- Healthcare Management System ---" << endl;
            cout << "1. Add New Doctor" << endl;
            cout << "2. Add New Appointment" << endl;
            cout << "3. Update Doctor Name" << endl;
            cout << "4. Update Appointment Date" << endl;
            cout << "5. Delete Doctor" << endl;
            cout << "6. Delete Appointment" << endl;
            cout << "7. Print Doctor Info" << endl;
            cout << "8. Print Appointment Info" << endl;
            cout << "9. Write Query" << endl;
            cout << "10. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
            case 1: addDoctor(); break;
            case 2: addAppointment(); break;
            case 3: updateDoctorName(); break;
            case 4: updateAppointmentDate(); break;
            case 5: deleteDoctor(); break;
            case 6: deleteAppointment(); break;
            case 7: printDoctorInfo(); break;
            case 8: printAppointmentInfo(); break;
            case 9: handleQueries(); break;
            case 10: cout << "Exiting system.\n"; break;
            default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 10);
    }
};
