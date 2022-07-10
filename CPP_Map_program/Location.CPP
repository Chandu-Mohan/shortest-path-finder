#define _USE_MATH_DEFINES

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <queue>
#include <algorithm>

using namespace std;
using namespace rapidxml;

xml_document<> doc;
xml_node<> *root_node = NULL;

typedef struct locNode {
    long long int id;
    string visible;
    string version;
    string changeset;
    string timestamp;
    string user;
    long long int uid;
    double lat;
    double lon;
    string Name;
    vector<long long int> neighbour;
} locNode;

typedef struct wayNode {
    long long int id;
    string visible;
    string version;
    string changeset;
    string timestamp;
    string user;
    long long int uid;

    vector<long long int> locIndexs;
} wayNode;

//Haversine formula to find distance between two points on a sphere.
double GetDistance(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = (lat1) * M_PI / 180.0;
    lat2 = (lat2) * M_PI / 180.0;
    double a = pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2) + pow(sin(dLat / 2), 2);
    double radius = 6500;
    double c = 2 * asin(sqrt(a));
    return radius * c;
}

int main() {
    //parsing the file
    ifstream theFile("map.osm");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]);
    root_node = doc.first_node("osm");
    unordered_map<long long int, long long int> mapp;
    vector<locNode> Data;
    vector<locNode> Namedata;
    vector<wayNode> Data1;
    locNode temp;
    long long int i = 0, l = 0, n;
    double d;


    xml_node<> *wn = root_node->first_node("way");
    //Scaning all the nodes into Data Array;
    i = 0;
    for (xml_node<> *location_node = root_node->first_node("node");
         location_node != wn; location_node = location_node->next_sibling()) {
        temp.Name = "\0";
        temp.id = stoll(location_node->first_attribute("id")->value());
        temp.visible = location_node->first_attribute("visible")->value();
        temp.version = location_node->first_attribute("version")->value();
        temp.changeset = location_node->first_attribute("changeset")->value();
        temp.timestamp = location_node->first_attribute("timestamp")->value();
        temp.user = location_node->first_attribute("user")->value();
        temp.uid = stoll(location_node->first_attribute("uid")->value());
        temp.lat = stod(location_node->first_attribute("lat")->value());
        temp.lon = stod(location_node->first_attribute("lon")->value());


        for (xml_node<> *location_name_node = location_node->first_node(
                "tag"); location_name_node; location_name_node = location_name_node->next_sibling()) {

            if (strcmp(location_name_node->first_attribute("k")->value(), "name") == 0) {
                temp.Name = location_name_node->first_attribute("v")->value();
                Namedata.push_back(temp);
                break;
            }
        }
        mapp.insert(make_pair(temp.id, i));
        Data.push_back(temp);
        i++;
    }
    xml_node<> *rn = root_node->first_node("relation");
    //scanning all ways into Data1 vector;
    i = 0;
    for (xml_node<> *way_node = wn; way_node != rn; way_node = way_node->next_sibling()) {
        wayNode temp1;
        temp1.id = stoll(way_node->first_attribute("id")->value());
        temp1.visible = way_node->first_attribute("visible")->value();
        temp1.version = way_node->first_attribute("version")->value();
        temp1.changeset = way_node->first_attribute("changeset")->value();
        temp1.timestamp = way_node->first_attribute("timestamp")->value();
        temp1.user = way_node->first_attribute("user")->value();
        temp1.uid = stoll(way_node->first_attribute("uid")->value());
        int j = 0;
        for (xml_node<> *nodes = way_node->first_node("nd");
             nodes && !strcmp(nodes->name(), "nd"); nodes = nodes->next_sibling()) {
            temp1.locIndexs.push_back(mapp[stoll(nodes->first_attribute("ref")->value())]);
            j++;

        }
        //filling the neighbours fron ways;
        if (temp1.locIndexs.size() > 1) {
            for (int k = 1; k < temp1.locIndexs.size(); k++) {
                Data[temp1.locIndexs[k]].neighbour.push_back(temp1.locIndexs[k - 1]);
                Data[temp1.locIndexs[k - 1]].neighbour.push_back(temp1.locIndexs[k]);
            }
        }
        Data1.push_back(temp1);
        i++;
    }
    /*for ( i = 0; i < Data.size(); i++)
        {
            for (int  j = 0; j < Data[i].neighbour.size(); j++)
            {
                cout<<Data[i].neighbour[j]<<" ";
            }
            cout<<"\n";
        }*/

    //Code for searching the name parts;
    string s;
    cout<<"Type Name";
    cin>>s;

    for ( i = 0; i < Namedata.size(); i++)
    {
        l=Namedata[i].Name.size();
            for(long long int j=0;j<=Namedata[i].Name.size()-s.size();j++)
            {
                long long int n;
                for(n=0;n<s.size();n++)
                {
                    if(s[n]== Namedata.at(i).Name[j+n] || s[n] == (Namedata.at(i).Name[j+n])+32 || s[n] == (Namedata.at(i).Name[j+n])-32 ) continue;
                    else break;
                }
                if(n==s.size())
                {
                    if(n==0) cout << "\nNodes that are matching with the given substring is/are\n" << endl ;
                    n=1;
                    cout << "Name:      " << Namedata[i].Name << "\n";
                    cout << "Node ID:   " << Namedata[i].id <<"\n";
                    cout << "Latitude:  " << fixed << Namedata[i].lat << "\n" ;
                    cout << "Longitude: " << fixed << Namedata[i].lon << "\n";
                    
                    break;
                }
            }
        
    }
    //Code for finding the nearst k nodes;
    long long int r, k;
    cout<<"Type the ID of the Node: ";
    cin >> r ;
    cout<<"Type Number of nearest nodes required: ";
    cin>>k;
    long long int index = mapp[r];
    priority_queue<pair<double, long long int>> nearstK;
    for (i = 0; i < Data.size(); i++) {
        nearstK.push(make_pair(GetDistance(Data[i].lat, Data[i].lon, Data[index].lat, Data[index].lon), i));
        if (nearstK.size() > k + 1) {//popping out if Size exeeds k+1;
            nearstK.pop();
        }
    }
    cout << "The nearst k nodes are :\n";
    while (nearstK.size() != 1) {
        cout << nearstK.top().first << "  " << Data[nearstK.top().second].id << "\n";
        nearstK.pop();
    }

    //Code for finding the short distance between two nodes
    long long int firstId, secondId, firstIndex, secondIndex;
    cout<<"Give first ID: ";
    cin >> firstId;
    cout<<"Give second ID: ";
    cin >> secondId;
    firstIndex = mapp[firstId];
    secondIndex = mapp[secondId];
    //I used Dijkstra's Algorithm to find the minimum distance 
    vector<pair<double, long long int>> minHeap;
    unordered_map<long long int, double> mindistmap;
    unordered_map<long long int, long long int > parentmap;

    for (i = 0; i < Data[firstIndex].neighbour.size(); i++) {
        minHeap.push_back(make_pair(GetDistance(Data[firstIndex].lat, Data[firstIndex].lon, Data[Data[firstIndex].neighbour[i]].lat,
                            Data[Data[firstIndex].neighbour[i]].lon), Data[firstIndex].neighbour[i]));
    }
    mindistmap.insert(make_pair(firstIndex, 0));

    while (minHeap.size() != 0) {
        double min=50000;
        long long int  t =0;
        for(long long int f=0;f<minHeap.size();f++){
            if(min>minHeap[f].first){
                t=f;
                min=minHeap[f].first;
            }
        }
        l=minHeap[t].second;
        d=minHeap[t].first;
        if (l == secondIndex) {
            cout << "\nDistance " << d << " Kilo meters \n";
            break;
        }
        minHeap.erase(minHeap.begin()+t);
        for (i = 0; i < Data[l].neighbour.size(); i++) {
            long long int j;
            for (j = 0; j < minHeap.size(); j++) {
                if (minHeap[j].second == Data[l].neighbour[i] ) {
                    if(minHeap[j].first > (d + GetDistance(Data[l].lat, Data[l].lon,Data[Data[l].neighbour[i]].lat,Data[Data[l].neighbour[i]].lon))){
                        minHeap[j].first = d + GetDistance(Data[l].lat, Data[l].lon, Data[Data[l].neighbour[i]].lat,
                                                        Data[Data[l].neighbour[i]].lon);
                        break;         
                    }
                    
                }
            }
            if(j==minHeap.size()){
                minHeap.push_back(make_pair((d+GetDistance(Data[l].lat, Data[l].lon, Data[Data[l].neighbour[i]].lat,Data[Data[l].neighbour[i]].lon)), Data[l].neighbour[i]));
            }
        }
    }

    return 0;
}