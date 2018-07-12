#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "CoinManager.h"
#include "constant.h"

namespace fs = boost::filesystem;
using namespace std;

CoinManager::CoinManager(char * path)
{
    datapath = string(path);
}

bool CoinManager::loadV1751chData(const int ch, vector<Event> & list)
{
    auto filename = datapath + "/751_ch/ch_" + to_string(ch) + ".txt";
    if (exists(fs::path(filename)) == false)
    {
        cout << filename << " does not exist!" << endl;
        return false;
    }
    ifstream inputfile(filename);
    long triggertime = 0;
    double tcfd = 0;
    int qlong = 0; 
    int qshort = 0;
    double peak = 0;
    while (!inputfile.eof())
    {
        inputfile >> triggertime >> tcfd >> qlong >> qshort >> peak;
        auto event = Event(triggertime, tcfd, qlong);
        list.push_back(event);
    }
    return true;
}

bool CoinManager::loadAPVchData(const int ch, vector<Event> & list)
{
    auto filename = datapath + "/APV_ch/ch_" + to_string(ch) + ".txt";
    if (exists(fs::path(filename)) == false)
    {
        cout << filename << " does not exist!" << endl;
        return false;
    }
    ifstream inputfile(filename);
    long triggertime = 0;
    double tcfd = 0;
    int qlong = 0; 
    while (!inputfile.eof())
    {
        inputfile >> triggertime >> tcfd >> qlong;
        auto event = Event(triggertime, tcfd, qlong);
        list.push_back(event);
    }
    return true;
}

bool CoinManager::loadchData(const int ch, vector<Event> & list, DGZtype dtype)
{
    if (dtype == DGZtype::V1751)
    {
        return loadV1751chData(ch, list);
    }
    if (dtype == DGZtype::APV)
    {
        return loadAPVchData(ch, list);
    }
}

void CoinManager::cointwoch(const vector<Event> & list1, 
                            const vector<Event> & list2, 
                            const int timewindow,
                            ofstream & output)
{
    double tof = 0;
    int pt = 0;
    for (int p1 = 0; p1 < list1.size(); p1++)
    {
        Event e1 = list1[p1];
        for (int p2 = pt; p2 < list2.size(); p2++)
        {   
            Event e2 = list2[p2];
            long delta_t = e2.triggertime - e1.triggertime;
            if (delta_t < -timewindow)
            {
                pt = p2;
                continue;
            }
            else if (delta_t > timewindow)
            {
                break;
            }
            else
            {
                tof = delta_t + e2.tcfd - e1.tcfd;
                output << tof << " " 
                       << e1.qlong << " " 
                       << e2.qlong << " "
                       << e1.triggertime << " "
                       << endl;                
            }
        }
    }
    output.close();
}

void CoinManager::coinPairs(DGZtype dtype, int & timewindow, vector<ChPair> & pairs, string & outputdir)
{
    vector<Event> list1;
    vector<Event> list2;
    for (auto i = pairs.begin(); i != pairs.end(); i++)
    {
        if (loadchData((*i).ch1, list1, dtype) && loadchData((*i).ch2, list2, dtype))
        {
            auto outputfile = outputdir + to_string((*i).ch1) + "_" + to_string((*i).ch2);
            ofstream output(outputfile);
            cointwoch(list1, list2, timewindow, output);
            cout << outputfile << " is generated " << endl;
        }
        list1.clear();
        list2.clear();
    }
}

vector<ChPair> CoinManager::genpairs(int s1list[], int sizes1, int s2list[], 
                                    int sizes2, int reflist[], int sizeref)
{
    vector<ChPair> pairs;
//  generate (s1, s2) pair 
    for (int i = 0; i < sizes1; i++)
    {
        for (int j = 0; j < sizes2; j++)
        {
            ChPair p(s1list[i], s2list[j]);
            pairs.push_back(p);
        }
    }
//  generate ref pair
    for (int i = 1; i < sizeref; i++)
    {
        ChPair p(reflist[0], reflist[i]);
        pairs.push_back(p);
    }
    return pairs;
}
    






















