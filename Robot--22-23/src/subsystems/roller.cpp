#include "main.h"
#include "pros/misc.h"
bool rolling = false;
void updateRoller(){
    if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)){
        rolling = true;
        roller = 127;
    }
    if(rolling){
        rollerDetector.set_led_pwm(100);
        if(red){
            if(rollerDetector.get_rgb().red > 200){
                rolling = false;
            }
        } else {
            if(rollerDetector.get_rgb().blue > 200){
                rolling = false;
            }
        }
    } else {
        roller = 0;
    }
}