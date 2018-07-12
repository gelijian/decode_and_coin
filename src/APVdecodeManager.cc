#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <APVdecodeManager.h>
#include "constant.h"

namespace fs = boost::filesystem;
using namespace std;
APVdecodeManager::APVdecodeManager(char * path)
{
    currentline = 0;
    linenum = 0;
    tdc1 = 0;
    tdc2 = 0;
    tdc3 = 0;
    tdc4 = 0;
    tdcfp = 0;
    qlong = 0;
    ch = -1;
    datapath = string(path);
    outputdir = datapath + "/APV_ch/";
    inputfile = ifstream();
}

int APVdecodeManager::GetBits(int a, const int& left, const int& right)
{
	int b,c,d;
	if (left < right)
	{
		cout << "'left' should be larger than 'right'" << endl;
		return (-1);
	}
	b = a >> (right - 1);
	c = ~((~0) << (left - right + 1));
	d = b & c;
	return d;
}

void APVdecodeManager::ReadOneLine()
{
    auto left = inputfile.get();
    auto right = inputfile.get();
    int total = 0;
	currentline = ((total | left) << 8) | right;
    linenum = linenum + 1;
}

void APVdecodeManager::ReadEvent()
{
    ReadOneLine();
    auto flag = linenum % 5;
    switch(flag)
    {	
        case 1: 
            tdc1 = currentline; 
            break;
        case 2: 
            tdc2 = currentline; 
            break;
        case 3: 
            tdc3 = currentline; 
            break;
        case 4:	
            tdc4 = GetBits(currentline, 16, 9); 
            tdcfp = GetBits(currentline, 8, 1); 
            break;
        case 0: 
        {
            ch = GetBits(currentline, 16, 14);
            qlong = GetBits(currentline, 13, 1);
            long timetag = (tdc1 * pow40 + tdc2 * pow24 + tdc3 * pow8 + tdc4) * 2;
            double tcfd = tdcfp * 0.0078125;
            choutput[ch] << timetag << " "
                         << tcfd << " "
                         << qlong << endl;
            break;
        }
    }
}

void APVdecodeManager::DecodeOneBoard(const int& board_id)
{
    linenum = 0;
    
    // create output directory
    if (exists(fs::path(outputdir)) == false)
	{
        create_directory(fs::path(outputdir));
	}
    
    auto filename = datapath + "APVlist" + to_string(board_id) + ".dat";
    if (exists(fs::path(filename)) == false)
	{
        cout << filename << " does not exist!" << endl;
        return;
	}
    
    for (int i = 0; i < NumChannel; i++)
	{   
        int channel = (board_id -1) * NumChannel + i;
        auto chfile = outputdir + "ch_" + to_string(channel) + ".txt";
        choutput[i].open(chfile);
    }
    
    // decode the data
    inputfile.open(filename, ios::binary);
	cout << "decoding board " << board_id << endl;
	while (!inputfile.eof())
    {
        ReadEvent();
	}
	cout << "decoding board " << board_id << " finished" << endl;
    
    // close the files
    inputfile.close();
    for (int i = 0; i < NumChannel; i++)
	{
		choutput[i].close();
	}
}



	
