// program to make a tau trim LUT
// generates a bit mask to be applied on the secondary cluster
// in order to avoid overlap with the main cluster
//
// CONVENTIONS:
// A - secondary numbering follows the tau sketch: 0,1,2,3 in the N "T shape", 4,5,6,7 in the S "T shape"
// B - cluster TT numbers follow what is used in the firmware. In particular, clusters are trimmed so that
//     a flag isWest is also needed to create the bitmask
// C - INPUT: line number is formed by combining:
//     line# = [neigh addr] [isWestMain] [isWestSec] [hasNN] [hasSS]
//      7 bit      3 bit       1 bit        1 bit      1bit   1 bit  ==> 128 lines in total
// has NN, hasSS: refer to the SECONDARY cluster for the LUIT that trims the MAIN
// D - OUTPUT: bit mask to be applied on the secondary cluster
//     output is on 7 bits


#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include "protoCluster.h"

using namespace std;

bool CheckBit (int number, int bitpos)
{
    bool res = number & (1 << bitpos);
    return res;
}

// this combines all bits into line number
int getLineNumber (int neigAddr, bool isWmain, bool isWsec, bool hasNN, bool hasSS)
{
    if (neigAddr > 7 || neigAddr < 0) cout << "Error: neighbor address must be in [0,7]" << endl;
    int line = 0;
    
    if (hasSS)   line |= (1 << 0);
    if (hasNN)   line |= (1 << 1);
    if (isWsec)  line |= (1 << 2);
    if (isWmain) line |= (1 << 3);
    line |= (neigAddr << 4); // start from bit 4

    return line;
}

void printLUTIntegers (vector<pair<int, int> >& LUT, std::ostream& out, bool printLineNum)
{
    for (int i = 0; i < LUT.size(); i++)
    {
        if (printLineNum) out << LUT.at(i).first << " " << LUT.at(i).second << endl;
        else              out << LUT.at(i).second << endl;
    }
}

void printLUT (vector<pair<int, int> >& LUT, std::ostream& out, int nbitsToPrint, bool printLineNum)
{
    for (int i = 0; i < LUT.size(); i++)
    {
        if (printLineNum) out << LUT.at(i).first << " ";
        int flag = LUT.at(i).second;
        for (int ibit = nbitsToPrint-1; ibit >= 0; ibit--)
        {
            out << CheckBit (flag, ibit);
        }
        out << endl; 
    }
}


int main()
{
    protoCluster mainCluster;
    protoCluster secondaryCluster;

    vector<pair<int,int> > LUT; // index - bitmask

    // neigbors
    vector <pair<int, int> > neigPos (8); // numbering of neighbors; iEta, iPhi
    // neigPos.at(0) = make_pair (0,   3);   
    // neigPos.at(1) = make_pair (-1,  2);
    // neigPos.at(2) = make_pair (0,   2);
    // neigPos.at(3) = make_pair (1,   2);    
    // neigPos.at(4) = make_pair (-1, -2);
    // neigPos.at(5) = make_pair (0,  -2);
    // neigPos.at(6) = make_pair (1,  -2);    
    // neigPos.at(7) = make_pair (0,  -3);
    neigPos.at(0) = make_pair (0,   3);   
    neigPos.at(1) = make_pair (1,   2);
    neigPos.at(2) = make_pair (0,   2);
    neigPos.at(3) = make_pair (-1,  2);    
    neigPos.at(4) = make_pair (1,  -2);
    neigPos.at(5) = make_pair (0,  -2);
    neigPos.at(6) = make_pair (-1, -2);    
    neigPos.at(7) = make_pair (0,  -3);


    // clusters on which the mask is encoded;
    // order is the internal ring from NW then clockwise, then NN, then SS
    // --> it's the opposite in thw firmware but no problem, using only relative eta
    // this should work fine
    // anyway these cluster numbers are my conventions so don't affect final LUT
    // boh mi sono perso spero che funzioni
    vector <pair<int, int> > TTPos (10); // numbering of TT in cluster; iEta, iPhi
    // TTPos.at(0) = make_pair (-1,  1); // SW
    // TTPos.at(1) = make_pair (0,   1); // S
    // TTPos.at(2) = make_pair (1,   1); // SE
    // TTPos.at(3) = make_pair (1,   0); // E
    // TTPos.at(4) = make_pair (1,  -1); // NE
    // TTPos.at(5) = make_pair (0,  -1); // N
    // TTPos.at(6) = make_pair (-1, -1); // NW
    // TTPos.at(7) = make_pair (-1,  0); // W
    // TTPos.at(8) = make_pair (0,   2); // SS
    // TTPos.at(9) = make_pair (0,  -2); // NN
    TTPos.at(0) = make_pair (1,   1); // SW
    TTPos.at(1) = make_pair (0,   1); // S
    TTPos.at(2) = make_pair (-1,  1); // SE
    TTPos.at(3) = make_pair (-1,  0); // E
    TTPos.at(4) = make_pair (-1, -1); // NE
    TTPos.at(5) = make_pair (0,  -1); // N
    TTPos.at(6) = make_pair (1,  -1); // NW
    TTPos.at(7) = make_pair (1,   0); // W
    TTPos.at(8) = make_pair (0,   2); // SS
    TTPos.at(9) = make_pair (0,  -2); // NN

    // prepare LUT
    for (int iNeigh = 0; iNeigh < 8; iNeigh++)
    {
        for (int isWmain = 0; isWmain <= 1; isWmain++)
        {
            for (int isWsec = 0; isWsec <= 1; isWsec++)
            {
                for (int hasNN = 0; hasNN <= 1; hasNN++)
                {
                    for (int hasSS = 0; hasSS <= 1; hasSS++)
                    {
                        mainCluster.initProto (isWmain, true, true); // I don't care if main has NN or SS as I must veto using the secondary cluster flags
                        secondaryCluster.initProto (isWsec, hasNN, hasSS);
                        
                        int line = getLineNumber (iNeigh, isWmain, isWsec, hasNN, hasSS);
                        int dEta = neigPos.at(iNeigh).first;
                        int dPhi = neigPos.at(iNeigh).second;

                        int bitmask = 1024 - 1; // toggle all low 10 bits to 1
                        // loop on all the TT making a cluster
                        for (int iTT = 0; iTT < 10; iTT++)
                        {
                            int iEta = TTPos.at(iTT).first;
                            int iPhi = TTPos.at(iTT).second;
                            if (mainCluster.overlapHere (secondaryCluster, dEta, dPhi, iEta, iPhi))
                                bitmask &= ~(1 << iTT); // clears bit iTT
                        }

                        // save output
                        LUT.push_back (make_pair(line, bitmask));
                    }
                }
            }
        }
    }
    
    std::ofstream fOutUnreduced ("unreducedLUT_TrimMain.txt");
    printLUT (LUT, std::cout, 10, true);
    printLUT (LUT, fOutUnreduced, 10, true);    
    std::ofstream fOutUnreducedDec ("unreducedDecimalLUT_TrimMain.txt");
    printLUTIntegers (LUT, fOutUnreducedDec, true);    
    
    // --------------
    
    std::vector<pair<int, int> > LUTreduced; // same line number as before but mask is trimmed for isWest
    std::vector<pair <int, int> > mapIsWest(10); // first: isWest = true; second: isWest = false
    // order of this vector is the same used for numbering TT, so for each entry I remap the TT to a new numbering scheme
    // unused indices are -1
    mapIsWest.at(0) = make_pair (4,  -1);
    mapIsWest.at(1) = make_pair (1,   1);
    mapIsWest.at(2) = make_pair (-1,  4);
    mapIsWest.at(3) = make_pair (-1,  2);
    mapIsWest.at(4) = make_pair (-1,  3);
    mapIsWest.at(5) = make_pair (0,   0);
    mapIsWest.at(6) = make_pair (3,  -1);
    mapIsWest.at(7) = make_pair (2,  -1);
    mapIsWest.at(8) = make_pair (6,   6);
    mapIsWest.at(9) = make_pair (5,   5);

    for (int iL = 0; iL < LUT.size(); iL++)
    {
        int thisline = LUT.at(iL).first;
        int thismask = LUT.at(iL).second;
        bool isWest = CheckBit (thisline, 2); // isWest secondary is bit 2
        
        int outMask = 0;
        for (int oldbit = 0; oldbit < 10; oldbit++)
        {
            int newbit = (isWest ? mapIsWest.at(oldbit).first : mapIsWest.at(oldbit).second);
            if (newbit == -1) continue; // unused value
            if (CheckBit(thismask, oldbit)) outMask |= ( 1 << newbit) ;
        }

        LUTreduced.push_back (make_pair(thisline, outMask));
    }

    cout << " =========================================== " << endl;
    std::sort (LUTreduced.begin(), LUTreduced.end()); // not used because already ordered by construciton
    std::ofstream fOutTrimLUT ("trimLUT_TrimMain.txt");
    printLUT (LUTreduced, std::cout, 7, true);
    printLUT (LUTreduced, fOutTrimLUT, 7, true);    

    std::ofstream fOutFinalLUT ("trimLUT_TrimMain_VHDL.txt");
    std::reverse (LUTreduced.begin(), LUTreduced.end());
    printLUT (LUTreduced, fOutFinalLUT, 7, false); // reverted + no line number



}