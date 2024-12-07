#pragma once
#include <iostream>
#include <string>
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
    unordered_map<string, string> doctorSecondaryIndex;

    //maps Doctor ID to appointment indexes
    unordered_map<string, string> doctorAppointmentIndex;

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
        }
        else {
            // Open file in binary mode at the end to get the size
            ifstream infile(filePath, ios::binary | ios::ate);
            int position = 0;
            if (infile) {
                position = infile.tellg();
            }
            infile.close();
            return position;
        }
    }

    int binarySearch(const string& id, const string& indexFile, unordered_map<string, int>& indexMap) {

        loadPrimaryIndex(indexFile, indexMap);

        // Convert the unordered_map to a vector of pairs
        vector<pair<string, int>> entries(indexMap.begin(), indexMap.end());

        // Perform binary search
        int low = 0;
        int high = entries.size() - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (entries[mid].first == id) {
                return entries[mid].second;
            }
            else if (entries[mid].first < id) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }
        return -1;
    }

    string binarySearch(const string& id, const string& indexFile, unordered_map<string, string>& indexMap) {
        // Load the primary index
        loadPrimaryIndex(indexFile, indexMap);

        // Convert the unordered_map to a vector of pairs
        vector<pair<string, string>> entries(indexMap.begin(), indexMap.end());

        sort(entries.begin(), entries.end(), [](const pair<string, string>& a, const pair<string, string>& b) {
            return a.second < b.second;
        });

        // Perform binary search
        int low = 0;
        int high = entries.size() - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (entries[mid].second == id) {
                return entries[mid].first;
            }
            else if (entries[mid].second < id) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }
        return "";
    }


    template <class T>
    void loadPrimaryIndex(const string& indexFile, unordered_map<string, T>& indexMap) {
        ifstream file(indexFile);
        if (!file) {
            cerr << "Error opening index file: " << indexFile << endl;
            return;
        }
        indexMap.clear();
        string key; // Referees to ID or DoctorName
        T value; // Referees to Position or Dr. ID
        while (file >> key >> value) {
            indexMap[key] = value;
        }
        file.close();
    }

    template <class T>
    void savePrimaryIndex(const string& indexFile, const unordered_map<string, T>& indexMap) {
        // Convert the unordered_map to a vector of pairs
        vector<pair<string, T>> entries(indexMap.begin(), indexMap.end());

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

        loadPrimaryIndex(doctorPriIndex, doctorPrimaryIndex);
        loadPrimaryIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);

        if (binarySearch(id, doctorPriIndex, doctorPrimaryIndex) != -1) {
            cout << "Doctor ID already exists. Aborting." << endl;
            return;
        }

        cout << "Enter Doctor Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Doctor Address: ";
        getline(cin, address);

        // Get available position or append
        int position = getAvailPosition(doctorFile, doctorAvailList);

        // Create Doctor object with offset
        Doctor doctor(id, name, address, position);  // Adjust the constructor to accept offset

        // Open the doctorFile for reading and writing in binary mode
        fstream file(doctorFile, ios::in | ios::out | ios::binary);
        if (!file) {
            // If the file doesn't exist, create it
            ofstream outfile(doctorFile, ios::binary);
            outfile.close();
            file.open(doctorFile, ios::in | ios::out | ios::binary);
            if (!file) {
                cerr << "Error creating doctor file." << endl;
                return;
            }
        }

        // Seek to the position
        file.seekp(position, ios::beg);

        // Write the serialized doctor data
        string serializedData = doctor.serialize();
        file.write(serializedData.c_str(), serializedData.length());
        file.close();

        // Update in-memory index and save to file
        doctorPrimaryIndex[id] = position;  // Use the actual position
        savePrimaryIndex("doctorIndexFile.txt", doctorPrimaryIndex);

        // Update secondary index in memory
        doctorSecondaryIndex[name] = id;
        savePrimaryIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);

        // Update the cumulative offset
        Doctor::cumulativeOffset = position + serializedData.length();

        cout << "Doctor added successfully." << endl;
    }


    void deleteDoctor(const string& doctorId) {
		int offset = binarySearch(doctorId, doctorPriIndex, doctorPrimaryIndex);
        // Open the doctor file for reading and writing in binary mode
        fstream file(doctorFile, ios::in | ios::out | ios::binary);

		if (offset == -1) {
			cout << "Doctor doesn't exist" << endl;
			return;
		}
        
        file.seekg(offset, ios::beg);
        char marker;
        file.get(marker);
        if (marker == '*') {
            cout << "Doctor already deleted" << endl;
            return;
        }
		// Mark the old record as deleted
		markDeleted(doctorFile, offset);
		// Get new position for the updated record
		int newPosition = getAvailPosition(doctorFile, doctorAvailList);
		
		if (!file) {
			cerr << "Error opening doctor file." << endl;
			return;
		}

		//remove from primary index
		doctorPrimaryIndex.erase(doctorId);

		savePrimaryIndex(doctorPriIndex, doctorPrimaryIndex);
		cout << "Doctor deleted successfully." << endl;
    }


    void updateDoctorName(const string& doctorId) {

        int offset = binarySearch(doctorId, doctorPriIndex, doctorPrimaryIndex);

        if (offset == -1) {
            cout << "Doctor doesn't exist" << endl;
            return;
        }

        // Open the doctor file to read the doctor data
        fstream file(doctorFile, ios::in | ios::binary | ios::out);
        if (!file) {
            cerr << "Error opening doctor file." << endl;
            return;
        }

        // Seek to the offset where the doctor data is located
        char ch;
        string id, name, address;

		file.seekg(offset + 2, ios::beg);
        while (file.get(ch) && ch != '|') {
            id += ch;
        }

        // Read Doctor Name (until we hit '|')
        while (file.get(ch) && ch != '|') {
            name += ch;
        }

        // Read Doctor Address (until the end of the line or EOF)
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            address += ch;
        }

		cout << "Enter New Name: ";
        string newName;
        cin.ignore();
		getline(cin, newName);

		if (newName.length() == name.length()) {
			file.seekp(offset + 2 + id.length() + 1, ios::beg);
			file.write(newName.c_str(), newName.length());
			cout << "Doctor Name Updated Successfully." << endl;
		}

        else {
            // Mark the old record as deleted
            markDeleted(doctorFile, offset);

            // Get new position for the updated record
            int newPosition = getAvailPosition(doctorFile, doctorAvailList);

            // Create a new Doctor object with the updated name
            Doctor updatedDoctor(id, newName, address, newPosition);

            // Serialize the updated doctor data
            string updatedSerializedData = updatedDoctor.serialize();

            // Open the doctor file for reading and writing in binary mode
            fstream file(doctorFile, ios::in | ios::out | ios::binary);
            if (!file) {
                cerr << "Error opening doctor file." << endl;
                return;
            }

            // Seek to the new position
            file.seekp(newPosition, ios::beg);

            // Write the updated serialized doctor data
            file.write(updatedSerializedData.c_str(), updatedSerializedData.length());
            file.close();

            // Update the primary index with the new position
            doctorPrimaryIndex[id] = newPosition;
            savePrimaryIndex("doctorIndexFile.txt", doctorPrimaryIndex);

            // Update the secondary index with the new name
            doctorSecondaryIndex.erase(name);
            doctorSecondaryIndex[newName] = id;
            savePrimaryIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);

            cout << "Doctor Name Updated Successfully." << endl;
        
        }
    }

    void addAppointment() {
        string id, date, doctorID;

        cout << "Enter Appointment ID: ";
        cin >> id;

        loadPrimaryIndex("appointmentIndexFile.txt", appointmentPrimaryIndex);
        loadPrimaryIndex("doctorIndexFile.txt", doctorPrimaryIndex);

        if (binarySearch(id, appointment, appointmentPrimaryIndex) != -1) {
            cout << "Appointment ID already exists. Aborting." << endl;
            return;
        }

        cout << "Enter Appointment Date (YYYY-MM-DD): ";
        cin >> date;

        cout << "Enter Doctor ID: ";
        cin >> doctorID;

        if (doctorPrimaryIndex.find(doctorID) == doctorPrimaryIndex.end()) {
            cout << "Doctor does not exist." << endl;
            return;
        }

        // Load secondary index
        loadPrimaryIndex("AppointmentSecondaryIndexFile.txt", doctorAppointmentIndex);

        // Get available position or append
        int position = getAvailPosition(appointmentFile, appointmentAvailList);

        // Create Appointment object with offset
        Appointment appointment(id, date, doctorID, position); // Adjust the constructor to accept offset

        // Open the appointmentFile for reading and writing in binary mode
        fstream file(appointmentFile, ios::in | ios::out | ios::binary);
        if (!file) {
            // If the file doesn't exist, create it
            ofstream outfile(appointmentFile, ios::binary);
            outfile.close();
            file.open(appointmentFile, ios::in | ios::out | ios::binary);
            if (!file) {
                cerr << "Error creating appointment file." << endl;
                return;
            }
        }

        // Seek to the position
        file.seekp(position, ios::beg);

        // Write the serialized appointment data
        string serializedData = appointment.serialize();
        file.write(serializedData.c_str(), serializedData.length());
        file.close();

        // Update in-memory index and save to file
        appointmentPrimaryIndex[id] = position; // Use the actual position
        savePrimaryIndex("appointmentIndexFile.txt", appointmentPrimaryIndex);

        // Update secondary index in memory
        doctorAppointmentIndex[doctorID] = id;
        savePrimaryIndex("AppointmentSecondaryIndexFile.txt", doctorAppointmentIndex);

        // Update the cumulative offset
        Appointment::cumulativeOffset = position + serializedData.length();

        cout << "Appointment added successfully." << endl;
    }


    void updateAppointmentDate(const string& appointmentId) {
        int offset = binarySearch(appointmentId, appointment, appointmentPrimaryIndex);

        if (offset == -1) {
            cout << "Appointment doesn't exist" << endl;
            return;
        }
        // Open the doctor file to read the doctor data
        fstream file(appointmentFile, ios::in | ios::binary | ios::out);
        if (!file) {
            cerr << "Error opening appointment file." << endl;
            return;
        }

        // Seek to the offset where the doctor data is located
        char ch;
        string id, date, doctorId;

        file.seekg(offset + 2, ios::beg);
        while (file.get(ch) && ch != '|') {
            id += ch;
        }

        // Read Doctor Name (until we hit '|')
        while (file.get(ch) && ch != '|') {
            date += ch;
        }

        // Read Doctor Address (until the end of the line or EOF)
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            doctorId += ch;
        }

        cout << "Enter New Appointment Date (YYYY-MM-DD): ";
        string newDate;
        cin.ignore();
        getline(cin, newDate);

        if (newDate.length() == date.length()) {
            file.seekp(offset + 2 + id.length() + 1, ios::beg);
            file.write(newDate.c_str(), newDate.length());
            cout << "Appointment Date Updated Successfully." << endl;
        }

        else {
            cout << "date needs to be in the specified format (YYYY-MM-DD)! "<<endl;
        }
    }



    void deleteAppointment() {


    }

    void printDoctorInfo(const string& doctorId) {

        int offset = binarySearch(doctorId, doctorPriIndex, doctorPrimaryIndex);

        if (offset == -1) {
            cout << "Doctor doesn't exist" << endl;
            return;
        }

        // Open the doctor file to read the doctor data
        fstream file(doctorFile, ios::in | ios::binary);
        if (!file) {
            cerr << "Error opening doctor file." << endl;
            return;
        }

        // Seek to the offset where the doctor data is located
        char ch;
        string id, name, address;

        file.seekg(offset + 2, ios::beg);
        while (file.get(ch) && ch != '|') {
            id += ch;
        }

        // Read Doctor Name (until we hit '|')
        while (file.get(ch) && ch != '|') {
            name += ch;
        }

        // Read Doctor Address (until the end of the line or EOF)
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            address += ch;
        }


        // Close the file after reading
          file.close();

        // Print the doctor's information
        cout << offset<<endl;
        cout << "Doctor ID: " << id << endl;
        cout << "Doctor Name: " << name << endl;
        cout << "Doctor Address: " << address << endl;
    }

    

    void printAppointmentInfo(const string& appointmentId) {
        int offset = binarySearch(appointmentId, appointment, appointmentPrimaryIndex);

        if (offset == -1) {
            cout << "Appointment doesn't exist" << endl;
            return;
        }
        // Open the doctor file to read the doctor data
        fstream file(appointmentFile, ios::in | ios::binary);
        if (!file) {
            cerr << "Error opening appointment file." << endl;
            return;
        }

        // Seek to the offset where the doctor data is located
        char ch;
        string id, date, doctorId;

        file.seekg(offset + 2, ios::beg);
        while (file.get(ch) && ch != '|') {
            id += ch;
        }

        // Read Doctor Name (until we hit '|')
        while (file.get(ch) && ch != '|') {
            date += ch;
        }

        // Read Doctor Address (until the end of the line or EOF)
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            doctorId += ch;
        }


        // Close the file after reading
        file.close();

        // Print the doctor's information
        cout << offset << endl;
        cout << "Appointment ID: " << id << endl;
        cout << "Appointment Date: " << date << endl;
        cout << "Doctor ID: " << doctorId << endl;
    }


    void handleQueries() {
        // Get Query From User
        string query;
        cout << "Enter Query: ";
        cin.ignore();
        getline(cin, query);
        for (int i = 0; i < query.length(); ++i) {
            query[i] = tolower(query[i]);
        }

        if (query.rfind("select", 0) == 0) { // Check The Query Validation
            // Check IF Primary OR Secondary Key
            string indexType = query.substr(query.find('t') + 2);

            if (indexType.rfind("doctor id", 0) == 0) {
                // DO Secondary Index ON Doctor ID 
            }
            else if (indexType.rfind("doctor name", 0) == 0) {
                // DO Secondary Index ON Doctor Name 
                string tName = query.substr(query.find("from") + 5);
                string condition = query.substr(query.find('=') + 2);
                

                if (binarySearch(condition, "doctorSecondaryIndexFile.txt", doctorSecondaryIndex) != "") {
                    cout << "Doctor Name: " << binarySearch(condition, "doctorSecondaryIndexFile.txt", doctorSecondaryIndex) << endl;
                }
                else {
                    cout << "Doctor Not Exist!" << endl;
                    return;
                }
            }
            else if ((indexType.rfind("all", 0) == 0) || (indexType.rfind("*", 0) == 0)) {
                // Get Table Name
                string tName = query.substr(query.find('m') + 2);
                string condition = query.substr(query.find('=') + 2);
                if (tName.rfind("doctor", 0) == 0) {
                    // DO Primary Index ON Doctor ID
                    printDoctorInfo(condition);
                }
                else if (tName.rfind("appointment", 0) == 0) {
                    // DO Primary Index ON Appointment ID
                    printAppointmentInfo(condition);
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
            system("cls");
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
            system("cls");
            switch (choice) {
            case 1: addDoctor(); system("pause"); break;
            case 2: addAppointment(); system("pause"); break;
            case 3: {
                string doctorId;
                cout << "Enter Doctor ID to update doctor name: ";
                cin >> doctorId;

				updateDoctorName(doctorId);
                system("pause");
				break;
            }
            case 4: {
                string appointmentId;
                cout << "Enter Appointment ID to view information: ";
                cin >> appointmentId;

                // Call the function to print doctor info
                updateAppointmentDate(appointmentId);
                system("pause");
                break;
            }
            case 5: {
                // Prompt for Doctor ID
                string doctorId;
                cout << "Enter Doctor ID to delete: ";
                cin >> doctorId;

                // Call the function to print doctor info
                deleteDoctor(doctorId);
                system("pause");
                break;
            }
            case 6: deleteAppointment(); break;
            case 7: {
                // Prompt for Doctor ID
                string doctorId;
                cout << "Enter Doctor ID to view information: ";
                cin >> doctorId;

                // Call the function to print doctor info
                printDoctorInfo(doctorId);
                system("pause");
                break;
            }
            case 8: {
                // Prompt for Doctor ID
                string appointmentId;
                cout << "Enter Appointment ID to view information: ";
                cin >> appointmentId;

                // Call the function to print doctor info
                printAppointmentInfo(appointmentId);
                system("pause");
                break;
            }
            case 9: handleQueries(); system("pause"); break;
            case 10: cout << "Exiting system.\n"; break;
            default: cout << "Invalid choice. Try again.\n"; system("pause"); break;
            }
        } while (choice != 10);
    }
};
