#include <iostream>
#include <stdlib.h>
#include "protoCluster.h"

using namespace std;

protoCluster::protoCluster ()
{
    for (int ieta = 0; ieta < 3; ieta++)
       for (int iphi = 0; iphi < 5; iphi++)
            _TT[ieta][iphi] = false;
}

pair<int,int> protoCluster::getEtaPhiIdx (int iEta, int iPhi)
{
    if (abs(iEta) > 1 || abs(iPhi) > 2) // out of bounds
    {
        cout << "Request for TT out of bounds" << endl;
        std::pair <int,int> dummy = std::make_pair (999, 999);
        return dummy;
    }

    std::pair <int,int> idxs = std::make_pair (iEta+1, iPhi+2);
    return idxs;
}

void protoCluster::initProto (bool isWest, bool hasNN, bool hasSS)
{
    // set the 4 TT in the corner (never used) to false directly
    setFlag (-1,  2, false);
    setFlag ( 1,  2, false);
    setFlag (-1, -2, false);
    setFlag ( 1,  2, false);

    // set the flags commong to West/East to 1
    setFlag (0,  1, true);
    setFlag (0, -1, true);
  
    if (hasNN) setFlag (0,  -2, true); // remember: firmware is inverted so N stays in -2
    else       setFlag (0,  -2, false);

    if (hasSS) setFlag (0, 2, true);    // same for S hat has LARGER phi
    else       setFlag (0, 2, false);

    if (isWest) // west is on the "left" --> -1
    {
        setFlag (-1,  1, true);
        setFlag (-1,  0, true);
        setFlag (-1, -1, true);
  
        setFlag (1,  1, false);
        setFlag (1,  0, false);
        setFlag (1, -1, false);
    }

    else
    {
        setFlag (-1,  1, false);
        setFlag (-1,  0, false);
        setFlag (-1, -1, false);
  
        setFlag (1,  1, true);
        setFlag (1,  0, true);
        setFlag (1, -1, true);
    }

    return;
}

void protoCluster::setFlag (int iEta, int iPhi, bool val)
{
    pair<int,int> idx = getEtaPhiIdx (iEta, iPhi);
    _TT[idx.first][idx.second] = val;
    return;
}

bool protoCluster::getFlag (int iEta, int iPhi)
{
    pair<int,int> idx = getEtaPhiIdx (iEta, iPhi);
    return _TT[idx.first][idx.second];
}

// used for the lut that trims the SECONDARY
bool protoCluster::overlap (protoCluster& neighbor, int dEta, int dPhi, int iEta, int iPhi)
{
    // if no overlap is possible due to too big dEta / dPhi, return false
    if (abs(dEta) > 2 || abs (dPhi) > 4) return false;

    // compute position of neighbor TT in THIS reference frame (dEta, dPhi are relative to the neighbor cluster)
    int thisiEta = iEta + dEta; 
    int thisiPhi = iPhi + dPhi;

    // if if this new position is out of bounds of neighbor return false
    if (abs(thisiEta) > 1 || abs(thisiPhi) > 2) return false;
    if (thisiEta == 0 && thisiPhi == 0) return true; // I'm checking the seed so overlap by default

    return (getFlag(thisiEta, thisiPhi)); // true if I have a valid TT here!
}

// used for the lut that trims the MAIN
bool protoCluster::overlapHere (protoCluster& neighbor, int dEta, int dPhi, int iEta, int iPhi)
{
    // if no overlap is possible due to too big dEta / dPhi, return false
    if (abs(dEta) > 2 || abs (dPhi) > 4) return false;

    // compute position of required TT in NEIGHBOR reference frame 
    int neighiEta = iEta - dEta; 
    int neighiPhi = iPhi - dPhi;

    // if if this new position is out of bounds of neighbor return false
    if (abs(neighiEta) > 1 || abs(neighiPhi) > 2) return false;
    if (neighiEta == 0 && neighiPhi == 0) return true; // I'm checking the seed so overlap by default

    // check overlap; useful to to the && because veto changes according to this cluster being West or not
    return (neighbor.getFlag(neighiEta, neighiPhi) && getFlag(iEta, iPhi));
}


