#pragma once
#include<iostream>
using namespace std;
class Appointment {
public:
    string id;
    string date;
    string doctorID;

    Appointment(string id, string date, string doctorID)
        : id(id), date(date), doctorID(doctorID) {}
};

