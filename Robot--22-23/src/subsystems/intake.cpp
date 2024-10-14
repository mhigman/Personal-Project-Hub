#include "pros/misc.h"
#include "main.h"
bool lockTakes = false;
bool takes = false;
void updateIntake(){
    if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && !(lockTakes)){
        takes = !takes;
        lockTakes = true;
    } else if(!(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1))){
        lockTakes = false;
    }
    if(takes){intake = 127;} else {intake = 0;}
}