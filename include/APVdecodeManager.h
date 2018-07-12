#ifndef APVdecodeManager_H
#define APVdecodeManager_H 1
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
class APVdecodeManager
{
    static const int NumChannel = 8;
	public:
		APVdecodeManager(char * path);
		int GetBits(int a, const int& left, const int& right);
        void ReadOneLine();
		void ReadEvent();
		void DecodeOneBoard(const int& board_id);
        
	private:
        int currentline;
        int linenum;
        int tdc1, tdc2, tdc3, tdc4, tdcfp;
        int qlong;
        int ch;
        string datapath;
        string outputdir;
        ifstream inputfile;
        ofstream choutput[NumChannel];
};


#endif
