#include <iostream>
#include <sstream> // Include the sstream header

using namespace std;

class Doctor {
public:
    string id;
    string name;
    string address;
    int offset;
    static int cumulativeOffset;

    Doctor(string id, string name, string address, int offset=0)
        : id(id), name(name), address(address), offset(offset) {
    }



    void setDoctorName(string name) {
		this->name = name;
    }


    string serialize() {
        int recordLength = id.length() + name.length() + address.length() + 2;
        cumulativeOffset += recordLength + 2;
        return to_string(recordLength) + id + "|" + name + "|" + address + "\n";
    }
    string getId() const { return id; }
    string getName() const { return name; }
    string getAddress() const { return address; }

    // Deserializes a string from file into a Doctor object
    static Doctor deserialize(const string& record) {
        stringstream ss(record);
        string id, name, address;
        getline(ss, id, '|');       // Extract the Doctor ID
        getline(ss, name, '|');     // Extract the Doctor Name
        getline(ss, address, '\n'); // Extract the Doctor Address
        return Doctor(id, name, address);
    }
};