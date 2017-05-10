#ifndef MYEVENT
#define  MYEVENT

#include <vector>
#include <TH2.h>

#include <xAODTracking/TrackParticle.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/Vertex.h>
#include "xAODTracking/VertexContainer.h"
#include <TRandom3.h>

class MYEvent{

public:

  long unsigned int number;
  int lumiblock;
  int multipl;
  double vertex_pos;

  std::vector<double> phi;
  std::vector<double> theta;
  std::vector<double> eta;
  std::vector<double> pt;

  //pt do dodania

  double myEta(double teta);


  ///
  /// simplest constructor
  MYEvent();

   ~MYEvent(); // for satysfying vtable

//private:


  //ClassDef(MYEvent, 1);
};

bool compvec(MYEvent* a, MYEvent* b, double vtxdiff = 1);
bool comptrackspt(MYEvent* a, MYEvent* b, int iter1, int iter2, double ptdiff = 10);
bool event_sel(const xAOD::TrackParticle* tp);
bool event_sel2(const xAOD::TrackParticle* tp);
double sinztheta(const xAOD::TrackParticle* tp);



#endif
