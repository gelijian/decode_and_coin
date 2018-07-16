#ifndef CoinManager_H
#define CoinManager_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

struct Event
{
    public: 
        Event(long t, double cfd, int q):triggertime(t), tcfd(cfd), qlong(q){};
        long triggertime;
        double tcfd;
        int qlong;
};

struct ChPair
{
    public:
        ChPair(int a, int b):ch1(a), ch2(b){};
        int ch1;
        int ch2;
};

enum DGZtype
{
    V1751,
    APV
};

class CoinManager
{
    public:
        CoinManager(char * path); 
        //~CoinManager();
        string datapath;
        bool loadV1751chData(const int ch, vector<Event> & list);
        bool loadAPVchData(const int ch, vector<Event> & list);
        bool loadchData(const int ch, vector<Event> & list, DGZtype dtype);
        void cointwoch(const vector<Event> & list1, 
                       const vector<Event> & list2, 
                       const int timewindow, 
                       ofstream & output);
        void coinPairs(DGZtype dtype, int & timewindow, 
                              vector<ChPair> & pairs, string & outputdir);
        vector<ChPair> genpairs(vector<int> s1list, vector<int> s2list, vector<int> reflist); 
};
#endif
