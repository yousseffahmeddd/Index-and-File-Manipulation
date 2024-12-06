#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Doctor.h"
#include "Appointment.h"

using namespace std;
class HealthcareSystem {
private:
   vector<int> doctorAvailList;
    vector<int> appointmentAvailList;

    //maps Doctor ID to index in `doctors`
    unordered_map<string, int> doctorPrimaryIndex;

    //maps Appointment ID to index in `appointments`
    unordered_map<string, int> appointmentPrimaryIndex;

    //maps Doctor Name to doctor indexes
    unordered_map<string, vector<string>> doctorSecondaryIndex;

    //maps Doctor ID to appointment indexes
    unordered_map<string, vector<int>> doctorAppointmentIndex;

    string doctorFile = "doctors.txt";        // Doctor data file
    string appointmentFile = "appointments.tx t"; // Appointment data file

// Helper function to mark a record as deleted
    void markDeleted(const string& filePath, int position) {
        fstream file(filePath, ios::in | ios::out);
        if (!file) {
            cerr << "Error opening file for deletion." << endl;
            return;
        }
        file.seekp(position);
        file.put('*'); // Marks the record as deleted
        file.close();
    }

    int getAvailPosition(const string& filePath, vector<int>& availList) {
        if (!availList.empty()) {
            int position = availList.back();
            availList.pop_back(); // Remove from avail list
            return position;
        } else {
            fstream file(filePath, ios::app);
            return file.tellp(); // If no available spots, append to the end of file
        }
    }


public:
    void addDoctor() {
        string id, name, address;
        cout << "Enter Doctor ID: ";
        cin >> id;

        if (doctorPrimaryIndex.find(id) != doctorPrimaryIndex.end()) {
            cout << "Doctor ID already exists. Aborting." << endl;
            return;
        }

        cout << "Enter Doctor Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Doctor Address: ";
        getline(cin, address);

        Doctor doctor(id, name, address);

        // Get available position from the avail list or append if none
        int position = getAvailPosition(doctorFile, doctorAvailList);

        fstream file(doctorFile, ios::in | ios::out);
        if (!file) {
            cerr << "Error opening doctor file." << endl;
            return;
        }

        file.seekp(position, ios::beg); // Move to the available position
        file << doctor.serialize();
        file.close();

        doctorPrimaryIndex[id] = position;

        // Insert into secondary index (Doctor Name → Doctor ID)
        doctorSecondaryIndex[name].push_back(id); // Corrected to use `id` as a string

        cout << "Doctor added successfully." << endl;
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
