#include "argument.hpp"
#include "boot_control.hpp"
#include <iostream>

int main(int argc, char** argv)
{
   auto options = open_power::boot::util::ArgumentParser(argc, argv);
   auto Major = std::stoi((options)["major"]);
   auto Minor = std::stoi((options)["minor"]);
   std::cout << "Executing "<<Major<<"."<<Minor<<std::endl;
   open_power::boot::Control::executeStep(Major, Minor); 
}}
