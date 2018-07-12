#include <stdio.h>
#include <math.h>
#include <iostream>
#include "Decodeoutput.h"
#include "V1751decodeManager.h"
#include "APVdecodeManager.h"

using namespace std;

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        printf("No datapath input!\n");
        return -1;
    }
    char *datapath = argv[1];
    
    //**** decode V1751 data******//
    cout << "Decoding V1751 data..." << endl;
    V1751decodeManager m1(datapath);
    m1.SetRecordLength(36);
    m1.SetBaselineLength(6);
    m1.SetPreTriggerSize(16);
    m1.SetPrecision(0.01);
    m1.SetCFDfraction(0.5);
    for (int i = 1; i < 5; i++)
    {
        m1.DecodeOneBoard(i);
    }
    cout << "Decoding V1751 data finished" << endl;
    cout << "######################" << endl;
    cout << endl;
    
    //********* decode APV8508 data *******//
    APVdecodeManager m2(datapath);
    cout << "Decoding APV data..." << endl;
    for (int i = 1; i < 5; i++)
    {
        m2.DecodeOneBoard(i);
    }
    cout << "Decoding APV finished" << endl;
    cout << endl;
    return 0;
    
}
