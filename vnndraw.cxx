void vnndraw()
{

    TFile *fafp50p = new TFile("data_afp_50_plus.root");
    TFile *fafp80p = new TFile("data_afp_80_plus.root");
    TFile *fafp5080p = new TFile("data_afp_50to80.root");

    TFile *fnafp50p = new TFile("data_noafp_50_plus.root");
    TFile *fnafp80p = new TFile("data_noafp_80_plus.root");
    TFile *fnafp5080p = new TFile("data_noafp_50to80.root");

    const int N = 4;

    double ***wsp = 0;
    wsp = new double **[6];

    wsp[0] = fourier(fafp50p, N);
    wsp[1] = fourier(fafp5080p, N);
    wsp[2] = fourier(fafp80p, N);

    wsp[3] = fourier(fafp50p, N);
    wsp[4] = fourier(fafp5080p, N);
    wsp[5] = fourier(fafp80p, N);



}


double** fourier(const TFile* plik, const int nglob)
{

    double **wspf = 0;
    wspf = new double*[nglob];
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

        wspf[n-1][0] = vn;
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
      double l1 = cos(nfi) - par1[n];
      lsum += l1 * l1 * dc2;
    }
    // std::cout << lsum << '\n';
    double l = sqrt(lsum);
    double vn_d = l / m;
    wspf[n - 1][1] = vn_d;
  }

  return wspf;
}