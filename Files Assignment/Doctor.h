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
};
