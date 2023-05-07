#include <iostream>
#include <map>

#ifndef INTERFACE_H
#define INTERFACE_H

using namespace std;

class Interface {
    public:

        //! Instance a new Interface Class
        Interface();

        //! Destructor
        ~Interface();

        static map<string, pair<int, int>> init_map();

};

#endif