const int nglob = 4; // ilosc v_nn do fitu

double fitf(double *x, double *par) //funkcja fitujaca
{
  //  Double_t arg = 0;
  //  if (par[2] != 0) arg = (x[0] - par[1])/par[2];
  const int nmax = nglob;
  double fitval = 0;

  int nstart = 1;
  int nn = nstart + nmax;

  for (int n = nstart; n <= nn; n++)
  {
    fitval += 2 * par[n - nstart + 1] * cos(n * x[0]);
  }

  fitval = par[0] * (1 + fitval); // + par[nn+1]; // mnoznik * suma vnn + stala

  return fitval;
}

// TH1F *hcorrlin3_lm = new TH1F();
//
// double fitf_periph(double *x, double *par)
// {
//   double fitval = 0;
//   int bin = hcorrlin3_lm->GetXaxis()->FindBin(x[0]);
//   double v = hcorrlin3_lm->GetBinContent(bin);
//   fitval = par[0]*v;
//   return fitval;
// }
//
// double fit_sum(double *x, double *par)
// {
//   return fitf_periph(x,par) + fitf(x,&par[nglob+1]);
// }

// ############################### main:

// void vnn(const char *name="dataafp.root")
void vnn(const char *name = "datanoafp.root")
{
  TFile *f = new TFile(name);

  TH1F *hcorrlin1 = (TH1F *)f->Get("hcorrlin1");
  TH1F *hcorrlin2 = (TH1F *)f->Get("hcorrlin2");

  TH1F *hcorrlin3 = (TH1F *)hcorrlin1->Clone("hcorrlin3");
  hcorrlin1->Sumw2();
  hcorrlin2->Sumw2();
  hcorrlin3->Sumw2();

  hcorrlin3->Divide(hcorrlin1, hcorrlin2);

  /// FOURIER ///

  TH1F *hl = (TH1F *)hcorrlin3->Clone("hl");
  TH1F *hm = (TH1F *)hcorrlin3->Clone("hm");

  const int dwa = 1;
  double m = hm->Integral();

  double par1[10] = {0};

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

    par1[n] = vn;

    // printf("FOURIER: v%d%d = %f / %f = %f\n",n,n,l,m,vn);
  }

  // niepewnosci fouriera

  double par1d[10] = {0};
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
    par1d[n] = vn_d;
  }

  /// FIT ///

  TCanvas *c1 = new TCanvas("c1", "fit", 500, 400);
  // Creates a Root function based on function fitf above
  TF1 *func = new TF1("fitf", fitf, 0, 3.14, nglob + dwa);

  double pars[10] = {0.94, 0, 0, 0, 0, 0, 1, 1, 1, 1}; // wiecej na wszelki wypadek

  for (int i = 1; i < nglob; i++)
  {
    pars[i] = par1[i]; //par z fouriera jako startowe...
  }

  for (int i = 0; i < nglob + dwa; i++)
  {
    func->SetParameter(i, pars[i]);
  }
  // hcorrlin3->Fit("fit_sum");

  // func->SetParLimits(0,0.9860,0.9861);
  // func->SetParNames("G","v1","v2","const");

  hcorrlin3->Fit("fitf", "r");
  // hcorrlin3->SetFillColor(kYellow);
  hcorrlin3->Draw("E1 SAME");

  double par2[10];
  double par2d[10];
  for (int i = 1; i <= nglob; i++)
  {
    par2[i] = func->GetParameter(i);
    par2d[i] = func->GetParError(i);
    printf("v%d%d:\n FOURIER = %f (error = %f)\n FIT = %f (error = %f)\n diff = %f  (%f percent)\n", i, i, par1[i], par1d[i], par2[i], par2d[i], fabs(par1[i] - par2[i]), fabs(par1[i] - par2[i]) / par1[i] * 100);
  }
}
