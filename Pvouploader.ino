#define VERSION "***** Your name ***** PVO S0 Uploader V3.2 5-2019 "   // Change to your name if you want

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include <TimeLib.h>
#include <MsTimer2.h>
#include <utility/w5100.h>
#include "FlashMini.h"
#include "S0Sensor.h"
#include "userdefs.h"

// global variables
byte   lastDayReset;
byte   lastHour;
byte   lastMinute;
byte   iDay;
byte   iHour;
byte   iMinute;
int    upTime;               // the amount of hours the Arduino is running
EthernetServer server(555);  // port changed from 80 to 555
EthernetUDP Udp;
char   webData[70];
#define EE_RESETDAY 4

void setup()
{
    // wait for the ethernet shield to wakeup
    delay(300);
    // initialize network
    Ethernet.begin(mac, ip, dnsserver, gateway, subnet);
    // set connect timeout parameters
    W5100.setRetransmissionTime(200); // 200ms per try
    W5100.setRetransmissionCount(8);

    // Try to set the time 10 times
    UpdateTime();

    // start listening
    server.begin();

    // initialize the sensors
    for(byte i = 0; i < NUMSENSORS; i++)
    {
        sensors[i]->Begin(i);
    }
    // set a random seed
    randomSeed(analogRead(0));

    // restore the last day on which the counters were reset
    lastDayReset = eeprom_read_byte((uint8_t*) EE_RESETDAY);
    // if the eeprom contains illegal data, set it to a useful value
    if(lastDayReset == 0 || lastDayReset > 31) lastDayReset = day();
    lastMinute = minute();
    lastHour = hour();
    upTime = 0;

    // start the timer interrupt
    MsTimer2::set(5, Every5ms); // 5ms period
    MsTimer2::start();
}

// check and update all counters every 5ms.
void Every5ms()
{
    for(byte i = 0; i < NUMSENSORS; i++)
    {
        sensors[i]->CheckSensor();
    }
    }

void loop()
{
    // get the actual time
    iDay = day();
    iHour = hour();
    iMinute = minute();
    // reset counters when todays day is different from the last day the counters were reset
    if(iDay != lastDayReset)
    {
    for(byte i = 0; i < NUMSENSORS; i++)
        {
            sensors[i]->Reset();
        }
     lastDayReset = iDay;
        // store today as the date of the last counter reset
        eeprom_write_byte((uint8_t*) EE_RESETDAY, lastDayReset);
    }

    // hour has changed
    // cannot simply check the change of an hour because 'updatetime' can also change the hour
    // therefore we also check that the minutes are 0
    if(iHour != lastHour && iMinute == 0)
    {
        lastHour = iHour;
        upTime++;
        // save the daily values every hour
        for(byte i = 0; i < NUMSENSORS; i++)
        {
            sensors[i]->Save();
        }
        // sync the time at fixed interval
     if(lastHour == 2 || lastHour == 6 || lastHour == 10 || lastHour == 14 || lastHour == 18 || lastHour == 22  )
        {
            UpdateTime();
        }
           } 

    // update every minute
    if(iMinute != lastMinute)
    {
        lastMinute = iMinute;
        for(byte i = 0; i < NUMSENSORS; i++)
        {
            sensors[i]->CalculateActuals();
        }

    // update every 5 minutes or whatever is set in userdefs
        if((lastMinute%UPDATEINTERVAL) == 0)
        {
            SendToPvOutput(sensors);
     // reset the maximum for pvoutput
            for(byte i = 0; i < NUMSENSORS; i++)
            {
                sensors[i]->ResetPeak();
            }
        }
        }
       // let all sensors do other stuff
    for(byte i = 0; i < NUMSENSORS; i++)
    {
      sensors[i]->Loop(lastMinute);
    }
    // see if there are clients to serve
    ServeWebClients();
      // give the ethernet shield some time to rest
    delay(50);
}
