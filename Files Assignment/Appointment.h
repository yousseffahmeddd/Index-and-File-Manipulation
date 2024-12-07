#include <iostream>
#include <sstream>  // For stringstream operations

using namespace std;

class Appointment {
public:
    string id;
    string date;
    string doctorID;
    int offset;  // The offset for this specific appointment

    // Static cumulative offset for all appointments
    static int cumulativeOffset;

    // Constructor to initialize appointment details and offset
    Appointment(string id, string date, string doctorID)
        : id(id), date(date), doctorID(doctorID), offset(cumulativeOffset) {}

    // Serializes the Appointment to a string
    string serialize() {
        int recordLength = id.length() + date.length() + doctorID.length() + 3; // +3 for separators and newline
        cumulativeOffset += recordLength + 2;  // Update the cumulative offset after this appointment
        return to_string(recordLength) + id + "|" + date + "|" + doctorID + "\n";
    }

    // Static method to deserialize a string back into an Appointment object
    static Appointment deserialize(const string& record) {
        stringstream ss(record);
        string id, date, doctorID;
        getline(ss, id, '|');
        getline(ss, date, '|');
        getline(ss, doctorID, '\n');
        return Appointment(id, date, doctorID);
    }
};


