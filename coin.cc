#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include "CoinManager.h"

namespace fs = boost::filesystem;
using namespace rapidjson;
using namespace std;

int main(int argc, char* argv[])
{
    CoinManager manager(argv[1]);
    auto shotpath = string(argv[1]);
    auto V1751dir = shotpath + "/751_toflist/";
    auto APVdir = shotpath + "/APV_toflist/";
    int timewindow = 300;
    vector<ChPair> pairs;
    vector<int> s1list;
    vector<int> s2list;
    vector<int> reflist;
    ifstream ifs("parameters.json");
    IStreamWrapper isw(ifs);
    Document d;
    d.ParseStream(isw);
    //*******  Coincidence for 751 data *****************//

    for (auto& v : d["V1751"]["s1list"].GetArray())
        s1list.push_back(v.GetInt());
    for (auto& v : d["V1751"]["s2list"].GetArray())
        s2list.push_back(v.GetInt());
    for (auto& v : d["V1751"]["reflist"].GetArray())
        reflist.push_back(v.GetInt());
    pairs = manager.genpairs(s1list, s2list, reflist);
    if (exists(fs::path(V1751dir)) == false)
    {
        create_directories(fs::path(V1751dir));
    }
    manager.coinPairs(DGZtype::V1751, timewindow, pairs, V1751dir);
    
    //~ //*******  Coincidence for APV data *****************//
    s1list.clear();
    s2list.clear();
    reflist.clear();
    pairs.clear();
    for (auto& v : d["APV"]["s1list"].GetArray())
        s1list.push_back(v.GetInt());
    for (auto& v : d["APV"]["s2list"].GetArray())
        s2list.push_back(v.GetInt());
    for (auto& v : d["APV"]["reflist"].GetArray())
        reflist.push_back(v.GetInt());
    pairs = manager.genpairs(s1list, s2list, reflist);
    
    if (exists(fs::path(APVdir)) == false)
    {
        create_directories(fs::path(APVdir));
    }
    manager.coinPairs(DGZtype::APV, timewindow, pairs, APVdir);

    cout << "finished" << endl;
    return 0;

}
