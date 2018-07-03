#include "sim800.h"

void SIM800::preInit(int baudRate)
{
	serialSIM800.begin(baudRate);
    delay(10);
	while(sendATTest() != 0)
		Serial.print(".");
	Serial.println();
	Serial.println("Init O.K!");         
}

int SIM800::checkReadable(void)
{
    return serialSIM800.available();
}

int SIM800::readBuffer(char *buffer,int count, unsigned int timeOut)
{
    int i = 0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();
    while(1) {
        while (serialSIM800.available()) {
            char c = serialSIM800.read();
            if (c == '\r' || c == '\n') c = '$';                            
            buffer[i++] = c;
            if(i > count-1)break;
        }
        if(i > count-1)break;
        timerEnd = millis();
        if(timerEnd - timerStart > 1000 * timeOut) {
            break;
        }
    }
    delay(500);
    while(serialSIM800.available()) {   // display the other thing..
        serialSIM800.read();
    }
    return 0;
}

void SIM800::cleanBuffer(char *buffer, int count)
{
    for(int i=0; i < count; i++) {
        buffer[i] = '\0';
    }
}

void SIM800::sendCmd(const char* cmd)
{
    serialSIM800.write(cmd);
}

int SIM800::sendATTest(void)
{
    int ret = sendCmdAndWaitForResp("AT\r\n","OK",DEFAULT_TIMEOUT);
    return ret;
}

int SIM800::waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum=0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();
    
    while(1) {
        if(serialSIM800.available()) {
            char c = serialSIM800.read();
            sum = (c==resp[sum]) ? sum+1 : 0;
            if(sum == len)break;
        }
        timerEnd = millis();
        if(timerEnd - timerStart > 1000 * timeout) {
            return -1;
        }
    }

    while(serialSIM800.available()) {
        serialSIM800.read();
    }

    return 0;
}

void SIM800::sendEndMark(void)
{
    serialSIM800.println((char)26);
}


int SIM800::sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout)
{
    sendCmd(cmd);
    return waitForResp(resp,timeout);
}

void SIM800::serialDebug(void)
{
    while(1) {
        if(serialSIM800.available()){
            Serial.write(serialSIM800.read());
        }
        if(Serial.available()){     
            serialSIM800.write(Serial.read()); 
        }
    }
}
