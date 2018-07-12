#ifndef V7151decodeManager_H
#define V1751decodeManager_H 1
#include "Decodeoutput.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
class V1751decodeManager
{
    static const int NumChannel = 8;
    static const int TimeStampLength = 32;
    static const int SamplesPerLine = 3;
    static const int MaxBits = 10;
    static const int GapBits = 0;
    static const int SamplingRate = 1;
	public:
		V1751decodeManager(char * path);
		unsigned int GetBits(unsigned int a, const int& left, const int& right);
        void ReadOneLine();
		void ReadEvent(const int& channel);
		void ReadChannelAggregateHeader(int& channel_aggregate_size);
		void ReadChannelAggregate(const int& channel);
		void ReadBoardAggregateHeader(int& board_aggregate_size, int& channel_mask, int& board_aggregate_counter);
		void ReadBoardAggregate(const int& board_id);
		void DecodeOneBoard(const int& board_id);
		double CfdTiming(double * wavesamples);
		double GetPeak(double * wavesamples);
	public:	
        void SetRecordLength(int data) {RecordLength = data;};
        void SetPreTriggerSize(int data) {PreTriggerSize = data;};
        void SetBaselineLength(int data) {BaselineLength = data;};
        void SetPrecision(double data) {Precision = data;};
        void SetCFDfraction(double data) {CFDfraction = data;};
	private:
        int linenum;
		unsigned int currentline;
        int RecordLength;
        int PreTriggerSize;
        int BaselineLength;
		double Precision;
		double CFDfraction;
        FILE * inputfile;
        string datapath;
        string outputdir;
        ofstream choutput[NumChannel];
        unsigned int pretimestamp[NumChannel];
        unsigned int timecycle[NumChannel];
};
#endif
