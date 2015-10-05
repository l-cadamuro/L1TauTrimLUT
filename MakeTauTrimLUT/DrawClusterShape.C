// draws the shape of the cluster using the decimal code as input

// "world" is measured in integers its extension is 2*RANGE 
// each TT has a width of STEP
// center of coordinates is bottom left part of seed TT

#define RANGE 4
#define STEP 1

// checks if a bit in the flags variable is set or not
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
            OffsE = 0;
            break;
        case 1:
            OffsN = -1*STEP;
            OffsE = 0;
            break;
        case 2:
            OffsN = 0;
            OffsE = 1*STEP;
            break;
        case 3:
            OffsN = 1*STEP;
            OffsE = 1*STEP;
            break;
        case 4:
            OffsN = -1*STEP;
            OffsE = 1*STEP;
            break;
        case 5:
            OffsN = 2*STEP;
            OffsE = 0;
            break;
        case 6:
            OffsN = -2*STEP;
            OffsE = 0;
            break;
        default:
            cout << "Wrong bitpos!" << endl;
            break;
    }
}

void DrawClusterShape(int code = -1, int print = 0)
{
    // if not inserted as initializer
    if (code == -1)
    {
        cout << "Insert cluster decimal code: ";
        cin >> code;
    }

    TCanvas* c1 = new TCanvas ("c1", "c1", 200, 200);
    c1->Range (-1*RANGE + 0.5*STEP, -1*RANGE + 0.5*STEP, RANGE + 0.5*STEP, RANGE + 0.5*STEP);


    int OffsN = 0;
    int OffsE = 0;
    // boxes empty pointers
    TBox* TT [8]; //i in 0..6: periferici; i = 7: seed
    for (int i = 0; i < 8; i++) TT[i] = 0;

    // draw seed
    TT[7] = new TBox (0,0,STEP,STEP);
    TT[7] ->SetLineColor(kBlue);
    TT[7] ->SetFillColor (kYellow);
    TT[7] ->Draw("lf");

    for (int iTT = 0; iTT < 7; iTT++)
    {
        if (CheckBit(code, iTT))
        {
            GetOffset(iTT, OffsN, OffsE);
            TT[iTT] = new TBox (OffsE, OffsN, OffsE+STEP, OffsN+STEP);
            TT[iTT] ->SetLineColor(kBlue);
            TT[iTT] ->SetFillColor(kRed);
            TT[iTT]->Draw("lf same");
        }
    }
    if (print != 0)
    {
        TString title = "clus_";
        title += code;
        title += ".gif";
        c1->Print (title, "gif");
    }

}

