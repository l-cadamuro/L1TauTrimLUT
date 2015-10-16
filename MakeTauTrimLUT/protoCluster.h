#ifndef PROTOCLUSTER_H
#define PROTOCLUSTER_H

#include <utility>

// this is the usual square + 2 phi extensions cluster
class protoCluster
{
    public:
        protoCluster();      
        std::pair<int, int> getEtaPhiIdx (int iEta, int iPhi); //get pair of indices to read _TT with input relative to center
        void initProto (bool isWest, bool hasNN, bool hasSS); // set TT flags to true/false according to the isWest position
        void setFlag (int iEta, int iPhi, bool val); // use relative position
        bool getFlag (int iEta, int iPhi); // use relative position

        // check if the TT of the neighbor cluster, at the position iEta, iPhi relative to the NEIGHBOR
        //    is shared with THIS cluster.
        // true if there is overlap
        // dEta, dPhi are the position of the neigbor with sign w.r.t. this cluster
        // so e.g. dEta = +1, dPhi = -2 means 1 eta unit UP (N) and 2 phi units left (E)
        bool overlap (protoCluster& neighbor, int dEta, int dPhi, int iEta, int iPhi);

        // same as before, but used for the second version of the LUT created
        // iEta, iPhi refer to the TT position relative to THIS cluster
        // dEta, dPhi is the neihbor seed position relative to THIS cluster
        // returns true by checking at the asked TT :
        // THIS.at(TT) && NEIGH.at(TT) -- so veto only if both are active
        bool overlapHere (protoCluster& neighbor, int dEta, int dPhi, int iEta, int iPhi);


        bool _TT [3][5]; // active TT in [eta] [phi]
};

#endif