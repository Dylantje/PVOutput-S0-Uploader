#ifndef userdefs
#define userdefs
//*****************************************************************
// Network variables
static byte mac[]       = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   // MAC address can be any number, as long as it is unique in your local network
static byte ip[]        = { 192,168,5,99 };                         // ***** IP Change to your ip range *****
static byte dnsserver[] = { 1,1,1,1};                               // ***** 1,1,1,1 or 8.8.8.8 *****                                                     
static byte gateway[]   = { 192,168,5,1 };                          // ***** Change to your ip range *****
static byte subnet[]    = { 255,255,255,0 };                        // ***** Change to your ip range *****
//*****************************************************************
#define NTP_SERVER "nl.pool.ntp.org"                                // If you are having problems with the time synchonisation, try a different NTP server
#define NTP_SERVER "de.pool.ntp.org" 
//*****************************************************************
#define PVOUTPUT_API_KEY " ***** YOUR Pvoutput Api *****"           // ***** pvouploader-key in the PvOutput *****
//***************************************************************** // ***** Do not use the read only key *****
#define UPDATEINTERVAL 5
//*****************************************************************
#define TIME_OFFSET 0                                               // Change to 1 or 2 when the uploader hase some missing data in Pvoutput
//*****************************************************************
#define NUMSENSORS 1                                                // Total use of sensors 
//***************************************************************** // ***** Arduino Don't use pin 4 and 7 *****
// Where the: 2 = pin, 1000 = pulse S0 counter, xxxxx = SID         // When you have a 800 pulse S0 counter, this this value to 800                                                   
S0Sensor  S1(2,1000,xxxxx,2,1);                                     // Pin 2 = First SID  Solar
//S0Sensor  S2(3,1000,xxxxx,2,1);                                   // Pin 3 = Second SID Solar
//S0Sensor  S3(5,1000,xxxxx,4,1);                                   // Pin 5 = Third SID  Usage [ Change to 4 for upload to usage  in Pvoutput ] 
//*****************************************************************
BaseSensor* sensors[] = {&S1,};                                     // {&S1,&S2,&S3,};
#endif
