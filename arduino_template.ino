#include "Korzhenevskiy_tick_tack_2.h"

#define serialdebug

#ifdef serialdebug
#define Snl Serial.println
#define S Serial.print
#endif

#ifndef serialdebug
#define Snl 
#define S 
#endif
#define Sline Serial.println("-------------------------------------------------------------------")



//____________________________________________________________________
/////////////////////////// SETUP ////////////////////////////////////
void setup() {
	

#ifdef serialdebug
	Serial.begin(115200);
	delay(100);
	Snl(" START. Serial debug ON");
#endif




Snl("Setup end");
Sline;
}
/////////////////////////// SETUP END /////////////////////////////////
//____________________________________________________________________










//____________________________________________________________________
/////////////////////////// LOOP ////////////////////////////////////
void loop() {


}
/////////////////////////// LOOP END /////////////////////////////////
//____________________________________________________________________

