#ifndef LDCorr_LDCAna_H
#define LDCorr_LDCAna_H

#include <EventLoop/Algorithm.h>
#include <TH2.h>
#include <LDCorr/MYEvent.h>

#include <xAODEventInfo/EventInfo.h>

#include <TrigConfxAOD/xAODConfigTool.h>
#include <TrigDecisionTool/TrigDecisionTool.h>

class LDCAna : public EL::Algorithm
{
	// put your configuration variables here as public variables.
	// that way they can be set directly from CINT and python.
  public:
	// float cutValue;

	// variables that don't get filled at submission time should be
	// protected from being send from the submission node to the worker
	// node (done by the //!)
  public:
	// Tree *myTree; //!
	// TH1 *myHist; //!

	void llicznik_nf(const xAOD::TrackParticleContainer *tracks, std::vector<MYEvent *> *myevents, TH2F *hcorr, int my_mult, const xAOD::EventInfo *event_info);
	void llicznik(const xAOD::TrackParticleContainer *tracks, std::vector<MYEvent *> *myevents, TH2F *hcorr, int my_mult, const xAOD::EventInfo *event_info);
	void lmianownik(std::vector<MYEvent *> myevents, TH2F *hcorr2);

	TH2F *hcorr;  //!
	TH2F *hcorr2; //!
	TH2F *hcorr3; //!

	TH2F *heta12l; //!
	TH2F *hphi12l; //!
	TH2F *heta12m; //!
	TH2F *hphi12m; //!
	TH2F *heta12c; //!
	TH2F *hphi12c; //!

	TH2F *hcorr_lm;  //!
	TH2F *hcorr2_lm; //!
	TH2F *hcorr3_lm; //!

	TH2F *hcorr3n; //!

	TH1 *hmult;  //!
	TH1 *hmult2; //!

	TH1 *heta;  //!
	TH1 *hdeta; //!
	TH1 *hphi;  //!
	TH1 *hdphi; //!
	TH1 *hpt;   //!

	TH1 *htrials; //!

	TH2 *hzsin; //!
	TH2 *hd0;   //!

	TH1 *hcorrlin1; //!
	TH1 *hcorrlin2; //!
	TH1 *hcorrlin3; //!

	TH1 *htrig; //!

	// TH1* hvertposz; //!

	int ev_iter;						//!
	MYEvent *ev;						//!
	std::vector<MYEvent *> myevents;	//!
	std::vector<MYEvent *> myevents_lm; //!

	std::unordered_map<std::string, int> triggerC; //!

	// std::vector<MYEvent*> myevents_aux; //!
	std::vector<MYEvent *> myevents_aux_lm; //!
	TRandom *myrandom;						//!

	Trig::TrigDecisionTool *trigDecTool;  //!
	TrigConf::xAODConfigTool *configTool; //!

	Trig::ChainGroup *trigger1; //!

	// this is a standard constructor
	LDCAna();

	// these are the functions inherited from Algorithm
	virtual EL::StatusCode setupJob(EL::Job &job);
	virtual EL::StatusCode fileExecute();
	virtual EL::StatusCode histInitialize();
	virtual EL::StatusCode changeInput(bool firstFile);
	virtual EL::StatusCode initialize();
	virtual EL::StatusCode execute();
	virtual EL::StatusCode postExecute();
	virtual EL::StatusCode finalize();
	virtual EL::StatusCode histFinalize();

	// this is needed to distribute the algorithm to the workers
	ClassDef(LDCAna, 1);
};

#endif
