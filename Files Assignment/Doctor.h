#pragma once
#include<iostream>
using namespace std;
class Doctor {
public:
    string id;
    string name;
    string address;

    Doctor(string id, string name, string address)
        : id(id), name(name), address(address) {}

    string serialize() {
        return id + "|" + name + "|" + address + "\n";
    }

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
