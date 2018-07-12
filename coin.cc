#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "CoinManager.h"

namespace fs = boost::filesystem;
using namespace std;

int main(int argc, char* argv[])
{
    CoinManager manager(argv[1]);
    auto shotpath = string(argv[1]);
    auto V1751dir = shotpath + "/751_toflist/";
    auto APVdir = shotpath + "/APV_toflist/";
    int timewindow = 300;
    vector<ChPair> pairs;
    
    //*******  Coincidence for 751 data *****************//
    int s1_751[] = {0, 1, 2, 3, 4};
    int s2_751[] = {5, 6, 
                    8, 9, 10, 11, 12, 13, 14};
    int ref_751[] = {7, 15};
    pairs = manager.genpairs(s1_751, 5, s2_751, 9, ref_751, 2);
    
    //~ int s1_751[] = {0, 7, 8, 16, 24};
    //~ int s2_751[] = {1, 2, 3, 4, 5, 6, 
                    //~ 9, 10,
                    //~ 17, 18, 19, 20, 21, 22, 23,
                    //~ 25, 26, 27, 28, 29, 30, 31};
    //~ int ref_751[] = {0, 8, 16, 24};
    //~ pairs = manager.genpairs(s1_751, 5, s2_751, 22, ref_751, 4);

    if (exists(fs::path(V1751dir)) == false)
    {
        create_directories(fs::path(V1751dir));
    }
    manager.coinPairs(DGZtype::V1751, timewindow, pairs, V1751dir);
    
    //~ //*******  Coincidence for APV data *****************//
    pairs.clear();
    int s1_APV[] = {0, 1, 2, 3, 4};
    int s2_APV[] = {5, 6, 
                    8, 9, 10, 11, 12, 13, 14};
    int ref_APV[] = {7, 15};
    pairs = manager.genpairs(s1_APV, 5, s2_APV, 9, ref_APV, 2);
    //~ int s1_APV[] = {0, 8, 16, 24, 31};
    //~ int s2_APV[] = {1, 2, 3, 4, 5, 6, 7,
                    //~ 9, 10, 11, 12, 13, 14, 15,
                    //~ 17, 18, 19, 20, 21, 22, 23,
                    //~ 25, 26, 27, 28, 29, 30};
    //~ int ref_APV[] = {0, 8, 16, 24};
    //~ pairs = manager.genpairs(s1_APV, 5, s2_APV, 27, ref_APV, 4);
    
    if (exists(fs::path(APVdir)) == false)
    {
        create_directories(fs::path(APVdir));
    }
    manager.coinPairs(DGZtype::APV, timewindow, pairs, APVdir);

    cout << "finished" << endl;
    return 0;

}
