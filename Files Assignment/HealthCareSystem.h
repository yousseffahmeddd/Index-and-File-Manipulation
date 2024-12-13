#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Doctor.h"
#include "Appointment.h"
#include <algorithm>

struct LinkedListNode {
    std::string doctorId;
    int next; // Index of the next node; -1 if it's the last node
};

using namespace std;
class HealthcareSystem {
private:
    vector<int> doctorAvailList;
    vector<int> appointmentAvailList;

    //maps Doctor ID to index in `doctors`appointmentPrimaryIndex
    unordered_map<string, int> doctorPrimaryIndex;
    
    //maps Appointment ID to index in `appointments`
    unordered_map<string, int> appointmentPrimaryIndex;

    //maps Doctor Name to doctor indexes
    unordered_map<string, int> doctorSecondaryIndex;

    //maps Doctor ID to appointment indexes
    unordered_map<string, int> doctorAppointmentIndex;

    //LinkedLists for secondary indexes
    unordered_map<int, LinkedListNode> doctorNameLinkedList; // Maps index to LinkedListNode
	unordered_map<int, LinkedListNode> appointmentLinkedList; // Maps index to LinkedListNode

	// Indexes for the next available index in the linked lists
    int nextDoctorNameListIndex = 0;
	int nextAppointmentListIndex = 0;

    string doctorFile = "doctors.txt";        // Doctor data file
    string appointmentFile = "appointments.txt"; // Appointment data file
    string doctorPriIndex = "doctorIndexFile.txt";
    string appointment = "appointmentIndexFile.txt";

    void loadLinkedList(const string& fileName, unordered_map<int, LinkedListNode>& linkedList) {
        ifstream file(fileName);
        if (!file) {
            cerr << "Error opening linked list file: " << fileName << endl;
            return;
        }
        linkedList.clear();
        int index;
        string id; // Can be doctorId or appointmentId based on context
        int nextIndex;
        int maxIndex = -1;
        while (file >> index >> id >> nextIndex) {
            linkedList[index] = { id, nextIndex };
            if (index > maxIndex) {
                maxIndex = index;
            }
        }
        // Update appropriate next index based on which linked list we're loading
        if (fileName == "doctorLinkedList.txt") {
            nextDoctorNameListIndex = maxIndex + 1;
        }
        else if (fileName == "appointmentLinkedList.txt") {
            nextAppointmentListIndex = maxIndex + 1;
        }
        file.close();
    }

    void saveLinkedList(const string& fileName, const unordered_map<int, LinkedListNode>& linkedList) {
        ofstream file(fileName, ios::trunc);
        if (!file) {
            cerr << "Error opening linked list file: " << fileName << endl;
            return;
        }
        for (const auto& entry : linkedList) {
            file << entry.first << " " << entry.second.doctorId << " " << entry.second.next << endl;
        }
        file.close();
    }


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

        loadIndex(indexFile, indexMap);

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

    string searchSecondary(const string& value, const string& indexFile, unordered_map<string, list<string>>& indexMap) {
        // Load the secondary index
        loadIndex(indexFile, indexMap);

        // Iterate over the indexMap to find the key associated with the value
        for (const auto& entry : indexMap) {
            // entry.first is the key (string)
            // entry.second is the list<string> (values)

            // Check if the value exists in the list
            if (std::find(entry.second.begin(), entry.second.end(), value) != entry.second.end()) {
                // Value found, return the key
                return entry.first;
            }
        }

        // Value not found
        return "";
    }

    template <typename T>
    void loadIndex(const string& indexFile, unordered_map<string, T>& indexMap) {
        ifstream file(indexFile);
        if (!file) {
            cerr << "Error opening index file: " << indexFile << endl;
            return;
        }
        indexMap.clear();
        string key;
        T value;
        while (file >> key) {
            if constexpr (is_same<T, list<string>>::value) {
                string id;
                list<string> ids;
                while (file >> id) {
                    ids.push_back(id);
                    if (file.peek() == '\n' || file.peek() == EOF) break;
                }
                indexMap[key] = ids;
            }
            else {
                file >> value;
                indexMap[key] = value;
            }
        }
        file.close();
    }

    template <typename T>
    void saveIndex(const string& indexFile, const unordered_map<string, T>& indexMap) {
        ofstream file(indexFile, ios::trunc);
        if (!file) {
            cerr << "Error opening index file: " << indexFile << endl;
            return;
        }

        // Extract entries from the unordered_map into a vector
        vector<pair<string, T>> entries(indexMap.begin(), indexMap.end());

        // Sort the entries based on the keys (doctor names)
        sort(entries.begin(), entries.end(), [](const pair<string, T>& a, const pair<string, T>& b) {
            return a.first < b.first;
            });

        for (const auto& entry : entries) {
            file << entry.first;
            if constexpr (is_same<T, list<string>>::value) {
                for (const auto& id : entry.second) {
                    file << " " << id;
                }
            }
            else {
                file << " " << entry.second;
            }
            file << endl;
        }
        file.close();
    }
public:
    void addDoctor() {
        string id, name, address;
        cout << "Enter Doctor ID: ";
        cin >> id;

        loadIndex(doctorPriIndex, doctorPrimaryIndex);
        loadIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);

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
        saveIndex("doctorIndexFile.txt", doctorPrimaryIndex);
        loadLinkedList("doctorLinkedList.txt", doctorNameLinkedList);

        // Update secondary index and linked list
        if (doctorSecondaryIndex.find(name) == doctorSecondaryIndex.end()) {
            // New name, create new linked list entry
            doctorSecondaryIndex[name] = nextDoctorNameListIndex;
            doctorNameLinkedList[nextDoctorNameListIndex] = { id, -1 };
            nextDoctorNameListIndex++;
        }
        else {
            // Existing name, append to linked list
            int currentIdx = doctorSecondaryIndex[name];
            while (doctorNameLinkedList[currentIdx].next != -1) {
                currentIdx = doctorNameLinkedList[currentIdx].next;
            }
            doctorNameLinkedList[currentIdx].next = nextDoctorNameListIndex;
            doctorNameLinkedList[nextDoctorNameListIndex] = { id, -1 };
            nextDoctorNameListIndex++;
        }

        // Save secondary index and linked list
        saveIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);
        saveLinkedList("doctorLinkedList.txt", doctorNameLinkedList);


        // Update the cumulative offset
        Doctor::cumulativeOffset = position + serializedData.length();

        cout << "Doctor added successfully." << endl;
    }

    void deleteDoctor(const string& doctorId) {
        int offset = binarySearch(doctorId, doctorPriIndex, doctorPrimaryIndex);

        if (offset == -1) {
            cout << "Doctor doesn't exist" << endl;
            return;
        }

        // Open the doctor file
        fstream file(doctorFile, ios::in | ios::out | ios::binary);
        if (!file) {
            cerr << "Error opening doctor file." << endl;
            return;
        }

        // Read the doctor's name
        string id, name, address;
        char ch;
        file.seekg(offset + 2, ios::beg); // Skip the deletion marker
        while (file.get(ch) && ch != '|') {
            id += ch;
        }
        while (file.get(ch) && ch != '|') {
            name += ch;
        }
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            address += ch;
        }
        file.close();

        // Mark the doctor record as deleted
        markDeleted(doctorFile, offset);

        // Remove from primary index
        doctorPrimaryIndex.erase(doctorId);
        saveIndex(doctorPriIndex, doctorPrimaryIndex);

        // Load secondary index and linked list
        loadIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);
        loadLinkedList("doctorLinkedList.txt", doctorNameLinkedList);

        // Remove from linked list
        if (doctorSecondaryIndex.find(name) != doctorSecondaryIndex.end()) {
            int currentIdx = doctorSecondaryIndex[name];
            int prevIdx = -1;
            while (currentIdx != -1) {
                if (doctorNameLinkedList[currentIdx].doctorId == doctorId) {
                    // Found the node to delete
                    if (prevIdx == -1) {
                        // First node in the list
                        if (doctorNameLinkedList[currentIdx].next == -1) {
                            // Only node, remove name from secondary index
                            doctorSecondaryIndex.erase(name);
                        }
                        else {
                            // Move head to next node
                            doctorSecondaryIndex[name] = doctorNameLinkedList[currentIdx].next;
                        }
                    }
                    else {
                        // Middle or last node
                        doctorNameLinkedList[prevIdx].next = doctorNameLinkedList[currentIdx].next;
                    }
                    doctorNameLinkedList.erase(currentIdx);
                    break;
                }
                prevIdx = currentIdx;
                currentIdx = doctorNameLinkedList[currentIdx].next;
            }
        }

        // Save secondary index and linked list
        saveIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);
        saveLinkedList("doctorLinkedList.txt", doctorNameLinkedList);

        cout << "Doctor deleted successfully." << endl;
    }

    void updateDoctorName(const string& doctorId) {
        int offset = binarySearch(doctorId, doctorPriIndex, doctorPrimaryIndex);

        if (offset == -1) {
            cout << "Doctor doesn't exist" << endl;
            return;
        }

        // Open the doctor file
        fstream file(doctorFile, ios::in | ios::out | ios::binary);
        if (!file) {
            cerr << "Error opening doctor file." << endl;
            return;
        }

        // Read existing doctor data
        string id, name, address;
        char ch;
        file.seekg(offset + 2, ios::beg); // Skip the deletion marker
        while (file.get(ch) && ch != '|') {
            id += ch;
        }
        while (file.get(ch) && ch != '|') {
            name += ch;
        }
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            address += ch;
        }

        // Get new name
        cout << "Enter New Name: ";
        string newName;
        cin.ignore();
        getline(cin, newName);

        // Update the record
        if (newName.length() == name.length()) {
            // Names are of equal length; safe to overwrite in place
            file.seekp(offset + 2 + id.length() + 1, ios::beg);
            file.write(newName.c_str(), newName.length());
            cout << "Doctor Name Updated Successfully." << endl;
            file.close();

            // Update secondary index and linked list
            loadIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);
            loadLinkedList("doctorLinkedList.txt", doctorNameLinkedList);

            // Remove from old name list
            if (doctorSecondaryIndex.find(name) != doctorSecondaryIndex.end()) {
                int currentIdx = doctorSecondaryIndex[name];
                int prevIdx = -1;
                while (currentIdx != -1) {
                    if (doctorNameLinkedList[currentIdx].doctorId == doctorId) {
                        // Found the node to move
                        if (prevIdx == -1) {
                            // First node
                            if (doctorNameLinkedList[currentIdx].next == -1) {
                                doctorSecondaryIndex.erase(name);
                            }
                            else {
                                doctorSecondaryIndex[name] = doctorNameLinkedList[currentIdx].next;
                            }
                        }
                        else {
                            doctorNameLinkedList[prevIdx].next = doctorNameLinkedList[currentIdx].next;
                        }
                        break;
                    }
                    prevIdx = currentIdx;
                    currentIdx = doctorNameLinkedList[currentIdx].next;
                }
            }

            // Add to new name list
            if (doctorSecondaryIndex.find(newName) == doctorSecondaryIndex.end()) {
                // New name, create new linked list entry
                doctorSecondaryIndex[newName] = nextDoctorNameListIndex;
                doctorNameLinkedList[nextDoctorNameListIndex] = { doctorId, -1 };
                nextDoctorNameListIndex++;
            }
            else {
                // Existing name, append to linked list
                int currentIdx = doctorSecondaryIndex[newName];
                while (doctorNameLinkedList[currentIdx].next != -1) {
                    currentIdx = doctorNameLinkedList[currentIdx].next;
                }
                doctorNameLinkedList[currentIdx].next = nextDoctorNameListIndex;
                doctorNameLinkedList[nextDoctorNameListIndex] = { doctorId, -1 };
                nextDoctorNameListIndex++;
            }

            // Save secondary index and linked list
            saveIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);
            saveLinkedList("doctorLinkedList.txt", doctorNameLinkedList);

        }
        else {
            // Names have different lengths; need to create a new record
            file.close();
            // Mark the old record as deleted
            markDeleted(doctorFile, offset);

            // Get new position
            int newPosition = getAvailPosition(doctorFile, doctorAvailList);

            // Create a new Doctor object
            Doctor updatedDoctor(id, newName, address, newPosition);

            // Write the updated doctor data
            fstream outFile(doctorFile, ios::in | ios::out | ios::binary);
            if (!outFile) {
                cerr << "Error opening doctor file." << endl;
                return;
            }
            outFile.seekp(newPosition, ios::beg);
            string serializedData = updatedDoctor.serialize();
            outFile.write(serializedData.c_str(), serializedData.length());
            outFile.close();

            // Update primary index
            doctorPrimaryIndex[id] = newPosition;
            saveIndex("doctorIndexFile.txt", doctorPrimaryIndex);

            // Update secondary index and linked list as above
            // (Remove from old list and add to new list)

            // Load secondary index and linked list
            loadIndex("doctorSecondaryIndexFile.txt", doctorSecondaryIndex);
            loadLinkedList("doctorLinkedList.txt", doctorNameLinkedList);

            // Remove from old name list and add to new name list
            // (Same logic as before)

            cout << "Doctor Name Updated Successfully." << endl;
        }
    }

    void addAppointment() {
        string id, date, doctorId;
        cout << "Enter Appointment ID: ";
        cin >> id;

        loadIndex(appointment, appointmentPrimaryIndex);
        loadIndex("appointmentSecondaryIndexFile.txt", doctorAppointmentIndex);

        if (binarySearch(id, appointment, appointmentPrimaryIndex) != -1) {
            cout << "Doctor ID already exists. Aborting." << endl;
            return;
        }

        cout << "Enter Doctor date: ";
        cin.ignore();
        getline(cin, date);
        cout << "Enter Doctor ID: ";
        getline(cin, doctorId);

        // Get available position or append
        int position = getAvailPosition(appointmentFile, appointmentAvailList);

        // Create Appointment object with offset
        Appointment appointment(id, date, doctorId, position);  // Adjust the constructor to accept offset

        // Open the doctorFile for reading and writing in binary mode
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

        // Write the serialized doctor data
        string serializedData = appointment.serialize();
        file.write(serializedData.c_str(), serializedData.length());
        file.close();

        // Update in-memory index and save to file
        appointmentPrimaryIndex[id] = position;  // Use the actual position
        saveIndex("appointmentIndexFile.txt", appointmentPrimaryIndex);
        loadLinkedList("appointmentLinkedList.txt", appointmentLinkedList);

        // Update secondary index and linked list
        if (doctorAppointmentIndex.find(doctorId) == doctorAppointmentIndex.end()) {
            // New name, create new linked list entry
            doctorAppointmentIndex[doctorId] = nextAppointmentListIndex;
            appointmentLinkedList[nextAppointmentListIndex] = { id, -1 };
            nextAppointmentListIndex++;
        }
        else {
            // Existing name, append to linked list
            int currentIdx = doctorAppointmentIndex[doctorId];
            while (appointmentLinkedList[currentIdx].next != -1) {
                currentIdx = appointmentLinkedList[currentIdx].next;
            }
            appointmentLinkedList[currentIdx].next = nextAppointmentListIndex;
            appointmentLinkedList[nextAppointmentListIndex] = { id, -1 };
            nextAppointmentListIndex++;
        }

        // Save secondary index and linked list
        saveIndex("appointmentSecondaryIndexFile.txt", doctorAppointmentIndex);
        saveLinkedList("appointmentLinkedList.txt", appointmentLinkedList);

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

    void deleteAppointment(const string& appointmentId) {
        // Step 1: Locate the Appointment Record
        int offset = binarySearch(appointmentId, "appointmentIndexFile.txt", appointmentPrimaryIndex);

        if (offset == -1) {
            cout << "Appointment doesn't exist" << endl;
            return;
        }

        // Step 2: Read Appointment Data
        string id, date, doctorId;
        fstream file(appointmentFile, ios::in | ios::binary);
        if (!file) {
            cerr << "Error opening appointment file." << endl;
            return;
        }

        char ch;
        file.seekg(offset + 2, ios::beg); // Skip the deletion marker
        while (file.get(ch) && ch != '|') {
            id += ch;
        }
        while (file.get(ch) && ch != '|') {
            date += ch;
        }
        while (file.get(ch) && ch != '\n' && ch != EOF) {
            doctorId += ch;
        }
        file.close();

        // Step 3: Mark the Appointment Record as Deleted
        markDeleted(appointmentFile, offset);

        // Step 4: Update the Primary Index
        appointmentPrimaryIndex.erase(appointmentId);
        saveIndex("appointmentIndexFile.txt", appointmentPrimaryIndex);

        // Step 5: Update the Secondary Index and Linked List
        loadIndex("AppointmentSecondaryIndexFile.txt", doctorAppointmentIndex);
        loadLinkedList("appointmentLinkedList.txt", appointmentLinkedList);

        // Remove from linked list
        if (doctorAppointmentIndex.find(doctorId) != doctorAppointmentIndex.end()) {
            int currentIdx = doctorAppointmentIndex[doctorId];
            int prevIdx = -1;
            while (currentIdx != -1) {
                if (appointmentLinkedList[currentIdx].doctorId == appointmentId) {
                    // Found the node to delete
                    if (prevIdx == -1) {
                        // First node in the list
                        if (appointmentLinkedList[currentIdx].next == -1) {
                            // Only node, remove doctorId from secondary index
                            doctorAppointmentIndex.erase(doctorId);
                        }
                        else {
                            // Move head to next node
                            doctorAppointmentIndex[doctorId] = appointmentLinkedList[currentIdx].next;
                        }
                    }
                    else {
                        // Middle or last node
                        appointmentLinkedList[prevIdx].next = appointmentLinkedList[currentIdx].next;
                    }
                    appointmentLinkedList.erase(currentIdx);
                    break;
                }
                prevIdx = currentIdx;
                currentIdx = appointmentLinkedList[currentIdx].next;
            }
        }

        // Step 6: Save the Updated Indexes
        saveIndex("AppointmentSecondaryIndexFile.txt", doctorAppointmentIndex);
        saveLinkedList("appointmentLinkedList.txt", appointmentLinkedList);

        cout << "Appointment deleted successfully." << endl;
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

    /*void printAppointmentsByDoctor(const string& doctorId) {
        loadIndex("AppointmentSecondaryIndexFile.txt", doctorAppointmentIndex);

        if (doctorAppointmentIndex.find(doctorId) == doctorAppointmentIndex.end()) {
            cout << "No appointments found for Doctor ID: " << doctorId << endl;
            return;
        }

        cout << "Appointments for Doctor ID " << doctorId << ":" << endl;
        for (const auto& appointmentId : doctorAppointmentIndex[doctorId]) {
            printAppointmentInfo(appointmentId);
        }
    }*/

    vector<string> getRecordsForName(const string& file1Name, const string& file2Name, const string& targetName) {
        // Open File 1 to get the name and pointer (line number)
        ifstream file1(file1Name);
        if (!file1) {
            cerr << "Error opening " << file1Name << endl;
            return {};
        }

        string name;
        int pointer;

        // Process File 1 to find the pointer for the target name
        int targetPointer = -1;
        while (file1 >> name >> pointer) {
            if (name == targetName) {
                targetPointer = pointer;  // Get the pointer for the target name
                break;
            }
        }

        // If target name was not found in File 1, return an empty vector
        if (targetPointer == -1) {
            cerr << "Name not found in File 1." << endl;
            return {};
        }

        // Close File 1 after processing
        file1.close();

        // Open File 2 to process the records
        ifstream file2(file2Name);
        if (!file2) {
            cerr << "Error opening " << file2Name << endl;
            return {};
        }

        // Vector to store the result
        vector<string> result;
        int currentPointer = targetPointer;

        // Follow the pointer chain in File 2
        string line;
        while (currentPointer != -1) {
            file2.clear();              // Clear the EOF flag
            file2.seekg(0, ios::beg);   // Go back to the beginning of the file

            bool found = false;
            while (getline(file2, line)) {
                stringstream ss(line);
                int field1, field3;
                string field2;

                ss >> field1 >> field2 >> field3;

                if (field1 == currentPointer) {
                    result.push_back(field2);  // Add the second field to the result
                    currentPointer = field3;  // Update the pointer to the next record's ID (third field)
                    found = true;
                    break;
                }
            }

            if (!found) {
                cerr << "Error: Pointer " << currentPointer << " not found in File 2." << endl;
                break;
            }
        }

        // Close File 2 after processing
        file2.close();

        return result;  // Return the vector of collected strings
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

            if (indexType.rfind("doctor name", 0) == 0) {
                // DO Secondary Index ON Doctor Name 
                string tName = query.substr(query.find("from") + 5);
                string condition = query.substr(query.find('=') + 2);
                vector<string> doctorIDs = getRecordsForName("doctorSecondaryIndexFile.txt", "doctorLinkedList.txt", condition);  // ===> param
                ifstream file("doctors.txt");
                vector<string> result;

                if (!file) {
                    cerr << "Error opening file: doctors.txt" << endl;
                    return;
                }
                string line;
                while (getline(file, line)) {
                    // Skip lines that start with '*'
                    if (line.empty() || line[0] == '*') {
                        continue;
                    }

                    // Extract the substring from the third character to the first '|'
                    size_t firstPipe = line.find('|');
                    if (firstPipe != string::npos && line.size() > 2) {
                        string checkSubstring = line.substr(2, firstPipe - 2);

                        // Compare with the first value in the checkVector
                        if (!doctorIDs.empty() && doctorIDs[0] == checkSubstring) {
                            // Find the second '|'
                            size_t secondPipe = line.find('|', firstPipe + 1);

                            if (secondPipe != string::npos) {
                                // Extract the substring between the first and second '|'
                                string value = line.substr(firstPipe + 1, secondPipe - firstPipe - 1);
                                result.push_back(value);
                            }
                        }
                    }
                }
                file.close();

                for (const string& names : result) {
                    cout << "Doctor Name: " << names << endl;
                }
            }
            else if ((indexType.rfind("all", 0) == 0) || (indexType.rfind("*", 0) == 0)) {
                // Get Table Name
                string tName = query.substr(query.find('m') + 2);
                string condition = query.substr(query.find('=') + 2);
                if (tName.rfind("appointments", 0) == 0) {
                    // Secondary index on Appointment
                    vector<string> doctorIDs = getRecordsForName("AppointmentSecondaryIndexFile.txt", "appointmentLinkedList.txt", condition); // ===> Param
                    for (const string& doctorID : doctorIDs) {
                        cout << "Appointment ID: " << doctorID << endl;
                    }
                }
                else if (tName.rfind("doctor", 0) == 0) {
                    // DO Primary Index ON Doctor ID
                    printDoctorInfo(condition);
                }
                else {
                    cout << "Table Not Exist!" << endl;
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
            case 6: {
                // Prompt for Doctor ID
                string appointmentId;
				cout << "Enter Appointment ID to delete: ";
                cin >> appointmentId;

                // Call the function to print doctor info
                deleteAppointment(appointmentId);
                system("pause");
                break;
            }
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



