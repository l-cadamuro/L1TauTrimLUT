// draws the shape of the cluster using the flag code as input

// "world" is measured in integers its extension is 2*RANGE 
// each TT has a width of STEP
// center of coordinates is bottom left part of seed TT

#define RANGE 8
#define STEP 1

bool CheckBit (int number, int bitpos)
{
    bool res = number & (1 << bitpos);
    return res;
}

// computes the N and E shifts according to the bit poisiton; i.e., the interface from shape encoding to N, S, E, etc..
void GetOffset (int bitpos, int& OffsN, int& OffsE)
{
    switch (bitpos)
    {
        case 0:
            OffsN = 1*STEP;
            OffsE = -1*STEP;
            break;
        case 1:
            OffsN = 1*STEP;
            OffsE = 0;
            break;
        case 2:
            OffsN = 1*STEP;
            OffsE = 1*STEP;
            break;
        case 3:
            OffsN = 0;
            OffsE = 1*STEP;
            break;
        case 4:
            OffsN = -1*STEP;
            OffsE = 1*STEP;
            break;
        case 5:
            OffsN = -1*STEP;
            OffsE = 0;
            break;
        case 6:
            OffsN = -1*STEP;
            OffsE = -1*STEP;
            break;
        case 7:
            OffsN = 0*STEP;
            OffsE = -1*STEP;
            break;
        case 8:
            OffsN = 2*STEP;
            OffsE = 0;
            break;
        case 9:
            OffsN = -2*STEP;
            OffsE = 0;
            break;
        default:
            cout << "Wrong bitpos!" << endl;
            break;
    }
}

void DrawClusterOverlap (int line, int bitmask)
{
    // extract all info from line number of LUT
    bool hasSS = (line >> 0) & 1;
    bool hasNN = (line >> 1) & 1;
    bool isWsec =  (line >> 2) & 1;
    bool isWmain = (line >> 3) & 1;
    bool b4 = (line >> 4) & 1;
    bool b5 = (line >> 5) & 1;
    bool b6 = (line >> 6) & 1;
    int pos = 0;
    if (b4) pos += 1;
    if (b5) pos += 2;
    if (b6) pos += 4;
    
    cout << b6 << b5 << b4 << endl;
    cout << "Sec position is: " << pos << endl;

    // -------------------------------------------------------------------------------------
    // draw cluster 1 - main
    // is is centered around 0

    TCanvas* c1 = new TCanvas ("c1", "c1", 400, 400);
    c1->Range (-1*RANGE + 0.5*STEP, -1*RANGE + 0.5*STEP, RANGE + 0.5*STEP, RANGE + 0.5*STEP);
    
    int OffsN = 0;
    int OffsE = 0;
    
    TBox* TTmain [11]; //i in 0..6: periferici; i = 7: seed
    for (int i = 0; i < 11; i++) TTmain[i] = 0;

    // draw seed
    TTmain[10] = new TBox (0,0,STEP,STEP);
    TTmain[10] ->SetLineColor(kWhite);
    TTmain[10] ->SetLineWidth(2);
    TTmain[10] ->SetFillColor (kRed);
    TTmain[10] ->Draw("lf");

    for (int iTT = 0; iTT < 10; iTT++)
    {
            GetOffset(iTT, OffsN, OffsE);
            TTmain[iTT] = new TBox (OffsE, OffsN, OffsE+STEP, OffsN+STEP);
            TTmain[iTT] ->SetLineColor(kWhite);
            TTmain[iTT] ->SetLineWidth(0);
            TTmain[iTT] ->SetFillColor(kCyan);
            TTmain[iTT] ->SetFillStyle(3002);
            
            bool toDraw = true;
            if (OffsN == 2 && hasNN == false) toDraw = false;  
            if (OffsN == -2 && hasSS == false) toDraw = false;  
            if (isWmain && OffsE == 1) toDraw = false;
            if (!isWmain && OffsE == -1) toDraw = false;
            
            if (toDraw) TTmain[iTT]->Draw("lf same");
    }
    
    // ----------------------------------------------------------
    // now do the same for secondary cluster
    // but change color if btmask is 0
    TBox* TTsec [11]; //i in 0..6: periferici; i = 7: seed
    for (int i = 0; i < 11; i++) TTsec[i] = 0;

    // draw seed
    int secEta = 0;
    int secPhi = 0;
    if (pos < 4) secPhi = 2;
    if (pos == 0) secPhi = 3;
    if (pos >= 4) secPhi = -2;
    if (pos == 7) secPhi = -3;
    if (pos == 1 || pos == 4) secEta = -1;
    if (pos == 3 || pos == 6) secEta = 1;


    TTsec[10] = new TBox (secEta*STEP,secPhi*STEP,secEta*STEP+STEP,secPhi*STEP+STEP);
    TTsec[10] ->SetLineColor(kWhite);
    TTsec[10] ->SetLineWidth(0);
    TTsec[10] ->SetFillColor (kYellow);
    TTsec[10] ->Draw("lf same");

    for (int iTT = 0; iTT < 10; iTT++)
    {
            GetOffset(iTT, OffsN, OffsE);
            TTsec[iTT] = new TBox (OffsE+secEta*STEP, OffsN+secPhi*STEP, OffsE+secEta*STEP+STEP, OffsN+secPhi*STEP+STEP);
            TTsec[iTT] ->SetLineColor(kWhite);
            TTsec[iTT] ->SetLineWidth(0);
            TTsec[iTT] ->SetFillColor(kOrange-1);
            
            bool toDraw = true;
            //if (OffsN == 2 && hasNN == false) toDraw = false;  
            //if (OffsN == -2 && hasSS == false) toDraw = false;  
            //if (isWsec  && OffsE == 1) toDraw = false;
            //if (!isWsec && OffsE == -1) toDraw = false;
            
            // now check flag!
            if (!CheckBit(bitmask, iTT))
            {
                TTsec[iTT] ->SetLineColor(kBlue);
                TTsec[iTT] ->SetLineWidth(2);
                TTsec[iTT] ->SetFillStyle(0); // transparent
            }

            if (toDraw) TTsec[iTT]->Draw("lf same");
    }



    //c1->Print ("c1.pdf", "pdf");

}