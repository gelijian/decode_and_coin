#include <stdio.h>
#include <math.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_statistics.h>
#include "V1751decodeManager.h"
#include "constant.h"

namespace fs = boost::filesystem;
using namespace std;

V1751decodeManager::V1751decodeManager(char * path)
{
    linenum = 0; 
    currentline = 0;
    RecordLength = 0;
    PreTriggerSize = 0;
    //cfd arguments
    BaselineLength = 0;
	Precision = 0;
	CFDfraction = 0; 
    datapath = string(path);
    outputdir = datapath + "/751_ch/";
	inputfile = NULL;
}

unsigned int V1751decodeManager::GetBits(unsigned int a, const int& left, const int& right)
{
	unsigned int b,c,d;
	if (left < right)
	{
		cout <<"'left' should be larger than 'right'\n" << endl;
		return 0;
	}
	b = a >> (right - 1);
	c = ~((~0) << (left - right + 1));
	d = b & c;
	return d;
}

void V1751decodeManager::ReadOneLine()
{
    fread(&currentline, 4, 1, inputfile);
    linenum = linenum + 1;
}

void V1751decodeManager::ReadEvent(const int& channel)
{
	//get timestamp
    ReadOneLine();
	unsigned int timestamp = currentline * SamplingRate;;
	//get wavesamples
	auto * wavesamples = new double[RecordLength];
	for (int i = 0; i < RecordLength / SamplesPerLine; i++)
	{
		ReadOneLine();
        for (int j = 0; j < SamplesPerLine; j++)
        {
            auto start = j * (MaxBits + GapBits) + 1;
            auto stop = start + MaxBits - 1;
            wavesamples[i * SamplesPerLine + j] = GetBits(currentline, stop, start);
        }
	}
	//get baseline;
	ReadOneLine();
	int baseline = GetBits(currentline, 12, 1);
	//get qshort and qlong
	ReadOneLine();
	int qlong = GetBits(currentline, 32, 17);
	int qshort = GetBits(currentline, 15, 1);
    auto cfdtime = CfdTiming(wavesamples);
    auto peak = GetPeak(wavesamples);
    if (timestamp < pretimestamp[channel])
    {
        timecycle[channel] += 1;
    }
    pretimestamp[channel] = timestamp;
    long timetag = timestamp + timecycle[channel] * pow32;;
    choutput[channel] << timetag << " " 
                      << cfdtime << " "
                      << qlong << " "
                      << qshort << " " 
                      << peak << endl;    
}

void V1751decodeManager::ReadChannelAggregateHeader(int& channel_aggregate_size)
{
	//get channel_aggregate_size
	ReadOneLine();
	channel_aggregate_size = GetBits(currentline, 15, 1);
    
	//get num_samples
	ReadOneLine();
}

void V1751decodeManager::ReadChannelAggregate(const int& channel)
{
	int channel_aggregate_size = 0;
	int num_samples = 0;
	ReadChannelAggregateHeader(channel_aggregate_size);
	int eventsize = RecordLength / SamplesPerLine + 3;
	int num_event = (channel_aggregate_size - 2) / eventsize;
	for (int i = 0; i < num_event; i++)
	{
		ReadEvent(channel);
	}	
}

void V1751decodeManager::ReadBoardAggregateHeader(int& board_aggregate_size, int& channel_mask, int& board_aggregate_counter)
{
	//get board_aggregate_size (in lwords)
	ReadOneLine();
	if (GetBits(currentline, 32, 29) == 10)
	{
		board_aggregate_size = GetBits(currentline, 28, 1);
	}
	else
	{
		ReadOneLine();
		board_aggregate_size = GetBits(currentline, 28, 1);
	}
    
	//get channel_mask
    ReadOneLine();
	channel_mask = GetBits(currentline, 8, 1);
	
	//get board_aggregate_counter
    ReadOneLine();
	board_aggregate_counter = GetBits(currentline, 23, 1);

    //get board_aggregate_timetag
	ReadOneLine();
	
    if (board_aggregate_counter % 100 == 0)
    {
        cout << "board_aggregate_counter : " << board_aggregate_counter << endl;
    }
}

void V1751decodeManager::DecodeOneBoard(const int& board_id)
{

    linenum = 0;
    
    //create output directory
    if (exists(fs::path(outputdir)) == false)
	{
        create_directory(fs::path(outputdir));
	}
    
    string filename = datapath + "V1751_" + to_string(board_id) + ".bin";
    if (exists(fs::path(filename)) == false)
	{
        cout << filename << " does not exist!" << endl;
        return;
	}
    
    for (int i = 0; i < NumChannel; i++)
	{   
        auto channel = (board_id -1) * NumChannel + i;
        auto chfile = outputdir + "ch_" + to_string(channel) + ".txt";
        choutput[i].open(chfile);
        pretimestamp[i] = 0;
        timecycle[i] = 0;
    }
    //decode the data
	inputfile = fopen(filename.c_str(), "rb");
	cout << "decoding board " << board_id << endl;
	auto board_aggregate_size = 0;
	auto channel_mask = 0;
	auto board_aggregate_counter = 0;
	while (1)
    {
		ReadBoardAggregateHeader(board_aggregate_size, channel_mask, board_aggregate_counter);
		if (feof(inputfile))
		{
			break;
		}
		for (int i = 0; i < NumChannel; i++)
		{
			if (GetBits(channel_mask, i + 1, i + 1) == 1)
			{
				int channel = i;
				ReadChannelAggregate(channel);
			}
		}	
	}
    cout << "decoding board " << board_id << " finished" << endl;
    cout << endl;
    
    //close the ouput file
    fclose(inputfile);
    for (int i = 0; i < NumChannel; i++)
	{
		choutput[i].close();
	}
}

double V1751decodeManager::CfdTiming(double * wavesamples)
{
    double * time = new double[RecordLength];
    for (int i = 0; i < RecordLength; i++)
    {
        time[i] = i * SamplingRate;
    }
    
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *linear = gsl_spline_alloc(gsl_interp_linear, RecordLength);
    gsl_spline_init(linear, time, wavesamples, RecordLength);
    
    double baseline = gsl_stats_mean(wavesamples, 1, BaselineLength);
    double peak = gsl_stats_min(wavesamples, 1, RecordLength) - baseline;
    double peak_index = gsl_stats_min_index(wavesamples, 1, RecordLength);
    
    int interp_length = (int)(peak_index * SamplingRate / Precision);
    double * x_interp = new double[interp_length];
    double * y_interp = new double[interp_length];
    double * z_interp = new double[interp_length];
    
	for (int i = 0; i < interp_length; i++)
	{	
        x_interp[i] = i * Precision;
		y_interp[i] = gsl_spline_eval (linear, x_interp[i], acc);
        z_interp[i] = fabs(y_interp[i] - baseline - peak * CFDfraction);
	}

    int cfd_index;
    cfd_index = gsl_stats_min_index(z_interp, 1, interp_length);
    double cfd_time;
    cfd_time = cfd_index * Precision - PreTriggerSize * SamplingRate;
    
    delete[] x_interp;
    delete[] y_interp;
    delete[] z_interp;
    
	gsl_spline_free (linear);
	gsl_interp_accel_free (acc);
	return cfd_time;
}

double V1751decodeManager::GetPeak(double * wavesamples)
{
    double baseline = gsl_stats_mean(wavesamples, 1, BaselineLength);
    double peak = gsl_stats_min(wavesamples, 1, RecordLength) - baseline;
    return fabs(peak);
}


	
