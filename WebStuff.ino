String inString = String((char*)"");

void ServeWebClients()
{
    EthernetClient client = server.available();
    if (client) 
    {
        inString = client.readStringUntil('\n');
        client << F("HTTP/1.1 200 OK") << endl;
        client << F("Content-Type: text/html") << endl;
        client << F("Connection: close") << endl << endl;
        if (Command((char*)"save")) SaveValues();
        if (Command((char*)"reset")) ResetValues();
        if (Command((char*)"restart")) while(1); // stay here until the watchdog barks
        if (Command((char*)"ntp")) UpdateTime(); // reload the ntp time
        int i=inString.indexOf("?");
        if(i!=-1) ReadValue(inString,i);
        ShowStatus(client);
        client.stop();
    }
}

bool Command(char* command)
{
  int i = inString.indexOf(command);
  if (i==-1) return false;
  return true;
}

void ReadValue(String input,int i)
{
    // string format is "?0=12345"
    //                  "?3=-12345"
    //                  "?G=12345"
    long val = 0;
    bool neg = false;
    int j = i + 3;
    int address = input[i+1];
    if((address >= '0' && address < '9') || (address >= 'A' && address < 'J')) 
    {
        char c = input[j];
        if(c == '-')
        {
            neg = true;
            j ++;
            c = input[j];
        }
        while(c >= '0' && c <= '9')
        {
            val = 10 * val + (c - '0');
            j ++;
            c = input[j];
        }
        if(neg) val = -val;
        if(address >= 'A') // address = 'A...'J' to write the total counters
        {
            sensors[address-'A']->NewTotal(val);
        }
        else // address = '0'...'9' to write the day counters
        {
            // let the sensor write the value to eeprom
            sensors[address-'0']->Update(val);
        }
    }
} 

void SaveValues()
{
    for(byte i = 0; i < NUMSENSORS; i++)
    {
        sensors[i]->Save();
    }
}

void ResetValues()
{
    for(byte i = 0; i < NUMSENSORS; i++)
    {
        sensors[i]->Reset();
    }
}

void ShowStatus(EthernetClient client)
{
    const char* br = "<br>";
    client << F("<html><style>td,th {padding:8;text-align:center;}</style>");
    client << F(" <br>"); 
    client << F(VERSION) << br;
    client << F(" <br>"); 
    client << F(" ********************************************");  
    client << F(" <br>"); 
    client << F(" <br>"); 
    client << F("<table border=\"4\" cellspacing=\"3\">");
    client << F("<tr><th>ID<th>SID<th>Actual<th>Peak<th>Today<th>Total</tr>");

    for(int i=0;i<NUMSENSORS;i++)
    {  
        sensors[i]->CalculateActuals();
        client << F("<tr><td>") << i;
        sensors[i]->Status(client);
        client << F("</tr>");
    }
    client << F("</table>");
       // hier begint alles onder de tabel
    client << F(" <br>"); 
    client << F(" ********************************************");  
    client << F(" <br>"); 
    client << F("PVO upload status:..") << pvResponse << " @ " << DateTime(pvResponseTime) << br;
    client << F(" ********************************************");
    client << F(" <br>");     
    client << F("DNS =...") << DnsStatus << br;
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("Actuel Date & Time:... ") << DateTime(now()) << br;
    client << F(" ********************************************"); 
    client << F(" <br>");  
    client << F("Time synced on:...") << DateTime(lastTimeUpdate) << " (in " << ntpRetry << "x)" <<  br;
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("The PVO S0 is running:...") << upTime/24 << "d.&." << upTime%24 << "h" << br;
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("Werkt de uploader niet, stekker er 20 min. uit laten.");
    client << F(" <br>"); 
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("De volgende dag waardes handmatig aanpassen.");
    client << F(" <br>"); 
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("Much pleasure with the PVO S0 uploader."); 
    client << F(" <br>"); 
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("Opensource: So please make a donation to a poor child foundation ");
    client << F(" <br>"); 
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("More info: PVOuploader@gmail.com"); 
    client << F(" <br>"); 
    client << F(" ********************************************"); 
    client << F(" <br>"); 
    client << F("Please become a member of the Pvoutput team: Arduino solar meter");
    client << F(" <br>");    
  }

int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
