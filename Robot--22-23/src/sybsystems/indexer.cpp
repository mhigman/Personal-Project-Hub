#include "main.h"
bool indexing = false;
void updateIndexer(){
    if(master.get_digital(DIGITAL_B)){
        indexing = true;
    }
    if(indexing){
        if(indexer.get_position() < 360){
            indexer = 127;
        } else{
            indexer.tare_position();
            indexer = 0;
            indexing = false;
        }
    }
}