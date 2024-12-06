#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Doctor.h"
#include "Appointment.h"
#include <algorithm>

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
    string appointmentFile = "appointments.txt"; // Appointment data file
    string doctorPriIndex = "doctorIndexFile.txt";
    string appointment = "appointmentIndexFile.txt";
    

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

    void loadPrimaryIndex(const string& indexFile, unordered_map<string, int>& indexMap) {
        ifstream file(indexFile);
        if (!file) {
            cerr << "Error opening index file: " << indexFile << endl;
            return;
        }
        indexMap.clear();
        string id;
        int position;
        while (file >> id >> position) {
            indexMap[id] = position;
        }
        file.close();
    }

    void savePrimaryIndex(const string& indexFile, const unordered_map<string, int>& indexMap) {
        // Convert the unordered_map to a vector of pairs
        vector<pair<string, int>> entries(indexMap.begin(), indexMap.end());

        // Sort the vector by the key (ID) in lexicographical order
        sort(entries.begin(), entries.end());

        // Open the file for writing, truncating any existing content
        ofstream file(indexFile, ios::trunc);
        if (!file) {
            cerr << "Error opening index file: " << indexFile << endl;
            return;
        }

        // Write sorted entries to the file
        for (const auto& entry : entries) {
            file << entry.first << " " << entry.second << endl;
        }
        file.close();
    }




public:
    void addDoctor() {
        string id, name, address;
        cout << "Enter Doctor ID: ";
        cin >> id;

        loadPrimaryIndex("doctorIndexFile.txt", doctorPrimaryIndex);


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

        // Get available position or append
        int position = getAvailPosition(doctorFile, doctorAvailList);

        ofstream file(doctorFile, ios::app);
        if (!file) {
            cerr << "Error opening doctor file." << endl;
            return;
        }
        file.seekp(position, ios::beg);
        file << doctor.serialize();
        file.close();

        // Update in-memory index and save to file
        doctorPrimaryIndex[id] = position;
        savePrimaryIndex("doctorIndexFile.txt", doctorPrimaryIndex);

        // Update secondary index in memory
        doctorSecondaryIndex[name].push_back(id);

        cout << "Doctor added successfully." << endl;
    }

    void deleteDoctor() {
    
    }

    void updateDoctorName() {

    }

    void addAppointment() {
        string id, date, doctorID;

        cout << "Enter Appointment ID: ";
        cin >> id;

        loadPrimaryIndex("appointmentIndexFile.txt", appointmentPrimaryIndex);
        loadPrimaryIndex("doctorIndexFile.txt", doctorPrimaryIndex);


        if (appointmentPrimaryIndex.find(id) != appointmentPrimaryIndex.end()) {
            cout << "Appointment ID already exists." << endl;
            return;
        }

        cout << "Enter Appointment Date (YYYY-MM-DD): ";
        cin >> date;

        cout << "Enter Doctor ID: ";
        cin >> doctorID;

        if (doctorPrimaryIndex.find(doctorID) == doctorPrimaryIndex.end()) {
            cout << "Doctor is not exists." << endl;
            return;
        }

        Appointment appointment(id, date, doctorID);

        // Get available position from the avail list or append if none
        int position = getAvailPosition(appointmentFile, appointmentAvailList);

        ofstream file(appointmentFile, ios::app);
        if (!file) {
            cerr << "Error opening appointment file." << endl;
            return;
        }

        file.seekp(position, ios::beg);
        file << appointment.serialize();
        file.close();

        // Update in-memory index and save to file
        appointmentPrimaryIndex[id] = position;
        savePrimaryIndex("appointmentIndexFile.txt", appointmentPrimaryIndex);

        cout << "Appointment added successfully." << endl;
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
        // Get Query From User
        string query;
        cout << "Enter Query: ";
        cin >> query;
        for (int i = 0; i < query.length(); ++i) {
            query[i] = tolower(query[i]);
        }

        if (query.rfind("select", 0) == 0) { // Check The Query Validation
            // Check IF Primary OR Secondary Key
            string indexType = query.substr(query.find('t') + 2);
            cout << indexType << endl;

            if (indexType.rfind("doctor id", 0) == 0) {
                // DO Secondary Index ON Doctor ID 
            }
            else if (indexType.rfind("doctor name", 0) == 0) {
                // DO Secondary Index ON Doctor Name 
            }
            else if ((indexType.rfind("all", 0) == 0) || (indexType.rfind("*", 0) == 0)) {
                // Get Table Name
                string tName = query.substr(query.find('m') + 2);
                string condition = query.substr(query.find('=') + 1);
                if (tName.rfind("doctors", 0) == 0) {
                    // DO Primary Index ON Doctor ID
                    // USE stoi TO CHANGE FROM STRING TO INT EX: stoi(condition)
                }
                else if (tName.rfind("appointments", 0) == 0) {
                    // DO Primary Index ON Appointment ID
                }
                else {
                    cout << "TABLE NOT EXIST!" << endl;
                }
            }
            else {
                cout << "INVALID INDEXING FORM!" << endl;
            }
        }
        else {
            cout << "INVALID QUERY FORMAT!" << endl;
        }
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
            default: cout << "Invalid choice. Try again.\n"; break;
            }
        } while (choice != 10);
    }
};
