#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

class PLAYER{
    private:
        std::string name;
        int bank;
    public:
        PLAYER(std::string, int);
        std::string get_name();
        int get_bank();
        void set_bank(int);
};