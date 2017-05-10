#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <LDCorr/LDCAna.h>

#include <AsgTools/MessageCheck.h> // ASG status code check

#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>

#include <iostream>
#include <TMath.h>
#include <string>

#include <xAODEventInfo/EventInfo.h>

//eta, phi                              http://hep.uchicago.edu/~kkrizka/rootcoreapis/d9/d1d/classxAOD_1_1TrackParticle__v1.html
#include <xAODTracking/TrackParticle.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/Vertex.h> //     http://hep.uchicago.edu/~kkrizka/rootcoreapis/db/dd1/classxAOD_1_1Vertex__v1.html
#include "xAODTracking/VertexContainer.h"

//AFP
#include "xAODForward/AFPTrackContainer.h"

//trigger logic
#include <TrigConfxAOD/xAODConfigTool.h>
#include <TrigDecisionTool/TrigDecisionTool.h>

// //moze mult
// #include <xAODTruth/TruthParticle.h>
// #include <xAODTruth/TruthParticleContainer.h>
// #include <xAODTruth/TruthEvent.h>
// #include <xAODTruth/TruthEventContainer.h>

// this is needed to distribute the algorithm to the workers
ClassImp(LDCAna)

    void LDCAna::llicznik_nf(const xAOD::TrackParticleContainer *tracks, std::vector<MYEvent *> *myevents, TH2F *hcorr, int my_mult, const xAOD::EventInfo *event_info)
{

  int multipl = 0;
  int c_mult = 0;

  MYEvent *ev = new MYEvent();

  multipl = tracks->size();
  ev->multipl = multipl;

  // ev->number = event_info->eventNumber();
  ev->lumiblock = event_info->lumiBlock();

  // hmult->Fill(multipl);

  if (multipl > my_mult) //mozna od 0, ale po co
  {
    for (int i = 0; i < multipl; i++) //dla wszystkich sladow:
    {
      // if (tracks->at(i)->vertex())
      // {
      //   hzsin->Fill(sinztheta(tracks->at(i)),multipl);
      //   hd0->Fill(tracks->at(i)->d0(),tracks->at(i)->vz());
      // }
      if (event_sel2(tracks->at(i)))
      {
        c_mult++;
        // Info("execute","track %d passed, curr c_mult = %d", i, c_mult);
      }
    }

    if (c_mult >= my_mult)
    {
      // hmult2->Fill(multipl);
      // Info("llicznik()","Event passed with mult: %d >= %d", c_mult, my_mult);

      for (int i = 0; i < multipl - 1; i++) //dla wszystkich sladow:
      {
        if (event_sel2(tracks->at(i)))
        {
          double vertex_pos_z = (double)tracks->at(i)->vertex()->z();
          // Info("%f\n", vertex_pos_z);
          ev->vertex_pos = vertex_pos_z;
          // hvertposz->Fill(vertex_pos_z);

          double phii = tracks->at(i)->phi();
          double etai = tracks->at(i)->eta();
          double pt = tracks->at(i)->pt();

          hphi->Fill(phii);
          heta->Fill(etai);
          hpt->Fill(pt);

          ev->pt.push_back(pt);
          ev->phi.push_back(phii);
          ev->eta.push_back(etai);

          for (int j = i + 1; j < multipl; j++)
          // for (int j = 0; j < multipl; j++)
          {
            if (i == j)
              continue;
            if (!event_sel2(tracks->at(j)))
              continue;

            double phij = tracks->at(j)->phi();
            double etaj = tracks->at(j)->eta();

            double dphi = TMath::Min(2 * TMath::Pi() - fabs(phii - phij), fabs(phii - phij));
            double deta = etai - etaj;

            // hphi12l->Fill(phii,phij);
            // heta12l->Fill(etai,etaj);
            //
            // hdeta->Fill(deta);
            // hdphi->Fill(dphi);

            // if (fabs(deta > 2)) {
            //   hcorrlin1->Fill(dphi);
            // }

            hcorr->Fill(deta, dphi);
          }

          //po if (event_sel2(tracks->at(i+1)))
        }
        //po if (event_sel2(tracks->at(i)))
      } // poo for

      // Info("llicznik()","current accepted multiplicity: %d", c_mult );

      myevents->push_back(ev);

      if (!(myevents->size() % 1000))
      {
        Info("llicznik_nf()", "added %lu events in total to 'my event vector'", myevents->size());
      }
    }
    else
      delete ev;
  }
}

void LDCAna::llicznik(const xAOD::TrackParticleContainer *tracks, std::vector<MYEvent *> *myevents, TH2F *hcorr, int my_mult, const xAOD::EventInfo *event_info)
{
  int multipl = 0;
  int c_mult = 0;

  MYEvent *ev = new MYEvent();

  multipl = tracks->size();
  ev->multipl = multipl;
  // ev->number = event_info->eventNumber();
  ev->lumiblock = event_info->lumiBlock();

  hmult->Fill(multipl);

  if (multipl > my_mult) //mozna od 0, ale po co
  {
    for (int i = 0; i < multipl; i++) //dla wszystkich sladow:
    {
      if (tracks->at(i)->vertex())
      {
        hzsin->Fill(sinztheta(tracks->at(i)), multipl);
        hd0->Fill(tracks->at(i)->d0(), tracks->at(i)->vz());
      }
      if (event_sel2(tracks->at(i)))
      {
        c_mult++;
        // Info("execute","track %d passed, curr c_mult = %d", i, c_mult);
      }
    }

    if (c_mult >= my_mult)
    {
      hmult2->Fill(multipl);
      // Info("llicznik()","Event passed with mult: %d >= %d", c_mult, my_mult);

      /// ############################ TRIGGERS
      // jakie triggery przeszly
      //
      // auto chainGroup = trigDecTool->getChainGroup(".*");
      // for(auto &trig : chainGroup->getListOfTriggers())
      // {
      //   auto cg = trigDecTool->getChainGroup(trig);
      //   if (cg->isPassed())
      //   {
      //     triggerC[trig]++;
      //     // std::cout << trig  << "\t PASSSED" << '\n';
      //   }
      // }
      /// ############################ TRIGGERS

      for (int i = 0; i < multipl - 1; i++) //dla wszystkich sladow:
      {
        if (event_sel2(tracks->at(i)))
        {
          double vertex_pos_z = (double)tracks->at(i)->vertex()->z();
          // Info("%f\n", vertex_pos_z);
          ev->vertex_pos = vertex_pos_z;
          // hvertposz->Fill(vertex_pos_z);

          double phii = tracks->at(i)->phi();
          double etai = tracks->at(i)->eta();
          double pt = tracks->at(i)->pt();

          hphi->Fill(phii);
          heta->Fill(etai);
          hpt->Fill(pt);

          ev->pt.push_back(pt);
          ev->phi.push_back(phii);
          ev->eta.push_back(etai);

          // for (int j = i+1; j < multipl; j++)
          for (int j = 0; j < multipl; j++)
          {
            if (i == j)
            {
              continue;
            }
            if (!event_sel2(tracks->at(j)))
              continue;

            double phij = tracks->at(j)->phi();
            double etaj = tracks->at(j)->eta();

            double dphi = TMath::Min(2 * TMath::Pi() - fabs(phii - phij), fabs(phii - phij));
            double deta = etai - etaj;

            hphi12l->Fill(phii, phij);
            heta12l->Fill(etai, etaj);

            hdeta->Fill(deta);
            hdphi->Fill(dphi);

            if (fabs(deta > 2))
            {
              hcorrlin1->Fill(dphi);
            }

            hcorr->Fill(deta, dphi);
          }

          //po if (event_sel2(tracks->at(i+1)))
        }

        //po if (event_sel2(tracks->at(i)))
      } // poo for

      // Info("llicznik()","current accepted multiplicity: %d", c_mult );

      myevents->push_back(ev);

      if (!(myevents->size() % 1000))
      {
        Info("llicznik()", "added %lu events in total to 'my event vector'", myevents->size());
      }
    }
    else
      delete ev;
  }
}

void LDCAna::lmianownik(std::vector<MYEvent *> myevents, TH2F *hcorr2)
{
  TRandom *myrandom = new TRandom3();
  Info("lmianownik()", "counting 'denominator events' from %lu events", myevents.size());

  unsigned long int count = 0;

  for (unsigned int ievent = 0; ievent < myevents.size(); ievent++)
  {
    myevents.at(ievent)->number = ievent;
  }

  for (unsigned int ievent = 0; ievent < myevents.size(); ievent++)
  {
    std::vector<MYEvent *> myevents_aux = myevents;

    if (!(ievent % 10000))
    {
      Info("lmianownik()", "at %i event of %lu\n", ievent, myevents.size());
    }

    MYEvent *ev1 = myevents.at(ievent);

    //oni chyba robili mianownik po kilka razy, to ma jakos zmniejszac blad
    for (int rep = 0; rep < 1; rep++)
    {
      unsigned int mysize = myevents_aux.size();
      unsigned int myrandomint = myrandom->Integer(mysize); //losowanie podobno ma być z najbliższych, posortowanych czasem, gdzie jest czas?
      MYEvent *ev2 = myevents_aux.at(myrandomint);

      int t = 0;
      const int t_max = 100000;
      while (t <= t_max) //mozna wiecej, moze ~wszystkich ev?
      {
        // if (!(t%10000) && t !=0)
        // {
        //   Info("lmianownik()","t = %d \n 1st event id: %lu; lumiblock %d \n 2nd event id: %lu; lumiblock %d",t, ev1->number, ev1->lumiblock, ev2->number, ev2->lumiblock);
        // }

        if (t == t_max)
        {
          count++;
          if (count % 10 == 0)
          {
            Info("lmianownik()", "number of max trials reached for event: %d\n", ievent);
          }
        }
        //mysize = myevents.size();
        myrandomint = myrandom->Integer(mysize);
        ev2 = myevents_aux.at(myrandomint);

        if (compvec(ev1, ev2)) //i inne warunki, np na jakosc, blad itd
        //na razie sprawdza z wierzcholka i multiplicity
        {
          for (unsigned int itrack = 0; itrack < ev1->phi.size(); itrack++)
          {
            double phii = ev1->phi.at(itrack);
            double etai = ev1->eta.at(itrack); // error

            for (unsigned int jtrack = 0; jtrack < ev2->phi.size(); jtrack++)
            {

              //if (comptrackspt( ev1, ev2, itrack, jtrack )) //tu dodac czas - to sprawdza czy tracki sa podobne (pt)
              // tego jednak nie ma

              // if (!((count)%1000000)) {
              //   Info("finalize()","at %lu combination",count);
              // }

              double phij = ev2->phi.at(jtrack);
              double etaj = ev2->eta.at(jtrack); /// <------------ error

              double dphi = TMath::Min(2 * TMath::Pi() - fabs(phii - phij), fabs(phii - phij));
              double deta = etai - etaj;
              hcorr2->Fill(deta, dphi);

              hphi12m->Fill(phii, phij);
              heta12m->Fill(etai, etaj);

              if (fabs(deta > 2))
              {
                hcorrlin2->Fill(dphi);
              }
            }
          }
          myevents_aux.erase(myevents_aux.begin() + myrandomint);
          if (!((int)(myevents_aux.size()) % 1000))
          {
            Info("lmianownik()", "erased %i event", myrandomint);
            //to nigdy sie nie wyswietla, jakby przeliczac mianownik 1000 razy, to jest szansa
          }
          htrials->Fill(t);
          break;
        }
        else
        {
          t++;
        }
      }
    }
  }
  Info("lmianownik()", "reached max trials for %lu events of %lu total", count, myevents.size());
  Info("lmianownik()", "%s", "clearing 'event vector'");
  for (unsigned int i = 0; i < myevents.size(); ++i)
  {
    delete myevents[i];
  }
  myevents.clear();
}

LDCAna::LDCAna()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}

EL::StatusCode LDCAna::setupJob(EL::Job &job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();
  job.options()->setString(EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_athena);

  ANA_CHECK_SET_TYPE(EL::StatusCode); // set type of return code you are
  // expecting (add to top of each function
  // once)
  ANA_CHECK(xAOD::Init());

  // std::vector <MYEvent> *myevents = new std::vector <MYEvent>();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::histInitialize()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  const int nbin = 30;

  hcorr = new TH2F("l", "l;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());
  hcorr2 = new TH2F("m", "m;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());
  hcorr3 = new TH2F("c", "c;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());

  hcorr_lm = new TH2F("l_lm", "l;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());
  hcorr2_lm = new TH2F("m_lm", "m;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());
  hcorr3_lm = new TH2F("c_lm", "c;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());

  heta12l = new TH2F("eta12l", "eta12;eta1;eta2", nbin, -2.5, 2.5, nbin, -2.5, 2.5);
  hphi12l = new TH2F("phi12l", "phi12;phi1;phi2", nbin, -TMath::Pi(), TMath::Pi(), nbin, -TMath::Pi(), TMath::Pi());
  heta12m = new TH2F("eta12m", "eta12;eta1;eta2", nbin, -2.5, 2.5, nbin, -2.5, 2.5);
  hphi12m = new TH2F("phi12m", "phi12;phi1;phi2", nbin, -TMath::Pi(), TMath::Pi(), nbin, -TMath::Pi(), TMath::Pi());
  heta12c = new TH2F("eta12c", "eta12;eta1;eta2", nbin, -2.5, 2.5, nbin, -2.5, 2.5);
  hphi12c = new TH2F("phi12c", "phi12;phi1;phi2", nbin, -TMath::Pi(), TMath::Pi(), nbin, -TMath::Pi(), TMath::Pi());

  hcorr3n = new TH2F("cn", "c_norm;deta;dphi", nbin, -5, 5, nbin, 0, TMath::Pi());

  // hcorr = new TH2F("l","l;deta;dphi",nbin,-6,6,nbin,-1,4);
  // hcorr2 = new TH2F("m","m;deta;dphi",nbin,-6,6,nbin,-1,4);
  // hcorr3 = new TH2F("c","c;deta;dphi",nbin,-6,6,nbin,-1,4);
  // hcorr3n = new TH2F("cn","c_norm;deta;dphi",nbin,-6,6,nbin,-1,4);

  hmult = new TH1F("hmult", "", 50, 1, -1);
  hmult2 = new TH1F("hmult2", "", 50, 1, -1);

  heta = new TH1F("heta", "", 50, 1, -1);
  hdeta = new TH1F("hdeta", "", 50, 1, -1);
  hphi = new TH1F("hphi", "", 50, 1, -1);
  hpt = new TH1F("hpt", "", 50, 1, -1);
  hdphi = new TH1F("hdphi", "", 50, 1, -1);

  htrials = new TH1F("htrials", "", 20, 1, -1);

  hzsin = new TH2F("zsin", "cut;zsin;mult", 50, 0, 1.5, 50, 60, 220);
  hd0 = new TH2F("d0vz", ";d0;vz", 50, -1, 1, 50, -70, 70);

  hcorrlin1 = new TH1F("hcorrlin1", "", 50, 0, TMath::Pi());
  hcorrlin2 = new TH1F("hcorrlin2", "", 50, 0, TMath::Pi());
  hcorrlin3 = new TH1F("hcorrlin3", "", 50, 0, TMath::Pi());

  /// TRIGGERY
  // htrig = new TH1F();
  // wk()->addOutput(htrig);

  // hvertposz = new TH1F("hvertposz","",50,1,-1);

  //  std::vector<MYEvent*> myevents; //!

  wk()->addOutput(hcorr);
  wk()->addOutput(hcorr2);
  wk()->addOutput(hcorr3);

  wk()->addOutput(heta12l);
  wk()->addOutput(hphi12l);
  wk()->addOutput(heta12m);
  wk()->addOutput(hphi12m);
  wk()->addOutput(heta12c);
  wk()->addOutput(hphi12c);

  wk()->addOutput(hcorr_lm);
  wk()->addOutput(hcorr2_lm);
  wk()->addOutput(hcorr3_lm);

  wk()->addOutput(hcorr3n);

  wk()->addOutput(hmult);
  wk()->addOutput(hmult2);

  wk()->addOutput(heta);
  wk()->addOutput(hdeta);
  wk()->addOutput(hphi);
  wk()->addOutput(hpt);
  wk()->addOutput(hdphi);
  wk()->addOutput(htrials);

  // wk()->addOutput(hvertposz);
  wk()->addOutput(hzsin);
  wk()->addOutput(hd0);

  wk()->addOutput(hcorrlin1);
  wk()->addOutput(hcorrlin2);
  wk()->addOutput(hcorrlin3);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::fileExecute()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed

  // xAOD::TEvent* event = wk()->xaodEvent();

  // Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int
  //myevents->reserve(event->getEntries());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::changeInput(bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::initialize()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  /// ############################ TRIGGERS
  // xAOD::TEvent* event = wk()->xaodEvent();
  //
  // configTool = new TrigConf::xAODConfigTool("xAODConfigTool");
  // ToolHandle<TrigConf::ITrigConfigTool> configHandle(configTool);
  // configHandle->initialize();
  // // The decision tool
  // trigDecTool = new Trig::TrigDecisionTool("TrigDecTool");
  // trigDecTool->setProperty("ConfigTool",configHandle);
  // //trigDecTool.setProperty("OutputLevel", MSG::VERBOSE);
  // trigDecTool->setProperty("TrigDecisionKey","xTrigDecision");
  // trigDecTool->initialize();

  // std::unordered_map<std::string,int> triggerC;
  /// ############################ TRIGGERS

  //*****************************************************************

  ev_iter = 0;

  std::vector<MYEvent *> myevents;
  std::vector<MYEvent *> myevents_lm;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::execute()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  xAOD::TEvent *event = wk()->xaodEvent();

  ANA_CHECK_SET_TYPE(EL::StatusCode);

  const xAOD::EventInfo *event_info = 0;

  // AFP
  const xAOD::AFPTrackContainer *afpTrackContainer = 0;

  ANA_CHECK(event->retrieve(event_info, "EventInfo")); //te same lumiblocki

  const xAOD::TrackParticleContainer *tracks = nullptr;

  int multipl = 0;

  const int my_mult_low = 3;
  const int my_mult = 60;
  const int my_mult_high = 80;

  // auto trigger1 = trigDecTool->getChainGroup("HLT_mb_sp900_trk50_hmt_L1TE5");

  // if ( event->retrieve(tracks, "InDetTrackParticles").isSuccess() && trigger1->isPassed() )
  // if ( event->retrieve(tracks, "InDetTrackParticles").isSuccess())
  if (event->retrieve(tracks, "InDetTrackParticles").isSuccess() && event->retrieve(afpTrackContainer, "AFPTrackContainer").isSuccess())
  {
    if (afpTrackContainer->size() == 1)
    {
      multipl = tracks->size();
      if (multipl > my_mult && multipl < my_mult_high)
      {
        llicznik(tracks, &myevents, hcorr, my_mult, event_info);
      }
      else if (my_mult_low < multipl && multipl < my_mult)
      {
        llicznik_nf(tracks, &myevents_lm, hcorr_lm, my_mult_low, event_info);
      }
    }
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::postExecute()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::finalize()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  //xAOD::TEvent* event = wk()->xaodEvent();

  Info("finalize()", "size of 'myevents': %lu", myevents.size());
  Info("finalize()", "size of 'myevents_lm': %lu", myevents_lm.size());

  lmianownik(myevents, hcorr2);
  lmianownik(myevents_lm, hcorr2_lm);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode LDCAna::histFinalize()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  heta12c->Divide(heta12l, heta12m);
  hphi12c->Divide(hphi12l, hphi12m);

  hcorr3->Divide(hcorr, hcorr2);

  hcorr3_lm->Divide(hcorr_lm, hcorr2_lm);

  // zabawy z normalizacja

  // double scale = hcorr3_lm->GetEntries();
  // hcorr3_lm->Scale(1/scale);
  //
  // scale = hcorr3->GetEntries();
  // hcorr3->Scale(1/scale);

  hcorr3n->Add(hcorr3, hcorr3_lm, 1, -1);

  hcorrlin3->Divide(hcorrlin1, hcorrlin2);

  // Double_t scale = 1/hcorr->Integral();
  // hcorr->Scale(scale);
  //
  // Double_t scale2 = 1/hcorr2->Integral();
  // hcorr2->Scale(scale2);

  // hcorr3 = (TH2F*)hcorr->Clone("c");
  return EL::StatusCode::SUCCESS;
}
