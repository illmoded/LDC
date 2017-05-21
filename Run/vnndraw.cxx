double **fourier(TFile *plik, const int nglob)
{

    double **wspf = 0;
    wspf = new double *[nglob];
    for (int n = 0; n < nglob; n++)
    {
        wspf[n] = new double[2];
    }

    TH1F *hcorrlin1 = (TH1F *)plik->Get("hcorrlin1");
    TH1F *hcorrlin2 = (TH1F *)plik->Get("hcorrlin2");
    TH1F *hcorrlin3 = (TH1F *)hcorrlin1->Clone("hcorrlin3");
    hcorrlin1->Sumw2();
    hcorrlin2->Sumw2();
    hcorrlin3->Sumw2();
    hcorrlin3->Divide(hcorrlin1, hcorrlin2);

    /// FOURIER ///

    TH1F *hl = (TH1F *)hcorrlin3->Clone("hl");
    TH1F *hm = (TH1F *)hcorrlin3->Clone("hm");

    double m = hm->Integral();

    for (int n = 1; n <= nglob; n++)
    {
        for (int i_bin = 0; i_bin < hl->GetSize(); i_bin++)
        {
            double nfi = n * hcorrlin3->GetXaxis()->GetBinCenter(i_bin);
            double val = hcorrlin3->GetBinContent(i_bin) * cos(nfi);
            hl->SetBinContent(i_bin, val);
        }
        double l = hl->Integral();
        double vn = l / m;

        wspf[n - 1][0] = vn;
    }

    // niepewnosci fouriera

    for (int n = 1; n <= nglob; n++)
    {
        double lsum = 0;
        for (int i_bin = 0; i_bin < hl->GetSize(); i_bin++)
        {
            double nfi = n * hcorrlin3->GetXaxis()->GetBinCenter(i_bin);
            double dc = hcorrlin3->GetBinError(i_bin);
            double dc2 = dc * dc;
            double l1 = cos(nfi) - wspf[n - 1][0];
            lsum += l1 * l1 * dc2;
        }
        // std::cout << lsum << '\n';
        double l = sqrt(lsum);
        double vn_d = l / m;
        wspf[n - 1][1] = vn_d;
    }

    return wspf;
}

void vnndraw()
{

    TFile *fafp50p = new TFile("data_afp_50_plus.root");
    TFile *fafp80p = new TFile("data_afp_80_plus.root");
    TFile *fafp5080p = new TFile("data_afp_50to80.root");

    TFile *fnafp50p = new TFile("data_noafp_50_plus.root");
    TFile *fnafp80p = new TFile("data_noafp_80_plus.root");
    TFile *fnafp5080p = new TFile("data_noafp_50to80.root");

    const int N = 4;
    const int P = 6;

    double ***wsp = 0;
    wsp = new double **[6];

    wsp[0] = fourier(fafp50p, N);
    wsp[1] = fourier(fafp5080p, N);
    wsp[2] = fourier(fafp80p, N);

    wsp[3] = fourier(fnafp50p, N);
    wsp[4] = fourier(fnafp5080p, N);
    wsp[5] = fourier(fnafp80p, N);

    for (int i = 0; i < P; i++)
    {
        printf("plik %i\n", i + 1);

        for (int n = 0; n < N; n++)
        {
            printf("n= %i : %f ( %f ) \n", n + 1, wsp[i][n][0], wsp[i][n][1]);
        }
    }

    c1 = new TCanvas("c1", "error bars", 100, 10, 1000, 800);
    c1->SetLogy();
    c1->SetGrid();

    TGraphErrors *gr[P];
    for (int i = 0; i < P; i++) //tworzenie graphow
    {
        gr[i] = new TGraphErrors(N);
    }

    for (int i = 0; i < P; i++) //dla kazdego pliku
    {
        for (int n = 0; n < N; n++) //dla kazdego pktu
        {
            double x = n + 1;
            double y = fabs(wsp[i][n][0]);
            double dx = 0;
            double dy = wsp[i][n][1];
            gr[i]->SetPoint(n, x, y);
            gr[i]->SetPointError(n, dx, dy);
        }

        gr[i]->SetLineColor(i + 1);
        if (i == 5)
        {
            gr[i]->SetLineColor(9);
        }
        gr[i]->SetMarkerStyle(i);
        gr[i]->SetLineWidth(2);
    }

    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            gr[i]->Draw("ACP");
        }
        else
        {
            gr[i]->Draw("CP");
        }
    }
}