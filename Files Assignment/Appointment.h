#include<iostream>
using namespace std;
class Appointment {
public:
    string id;
    string date;
    string doctorID;

    Appointment(string id, string date, string doctorID)
        : id(id), date(date), doctorID(doctorID) {}
    // Serializes the Appointment data into a string for file storage
    string serialize() {
        return id + "|" + date + "|" + doctorID + "\n";
    }

    // Deserializes a string from file into an Appointment object
    static Appointment deserialize(const string& record) {
        stringstream ss(record);
        string id, date, doctorID;
        getline(ss, id, '|');       // Extract the Appointment ID
        getline(ss, date, '|');     // Extract the Appointment Date
        getline(ss, doctorID, '\n');// Extract the Doctor ID
        return Appointment(id, date, doctorID);
    }                    
};

