#include "LDCorr/MYEvent.h"
#include "math.h"


MYEvent::MYEvent()
{
  vertex_pos = 0.0;
  number = 0;
  multipl = 0;
  lumiblock = 0;

  phi.reserve(250);
  theta.reserve(250);
  eta.reserve(250);
  pt.reserve(250);
}

MYEvent::~MYEvent()
{
}

double MYEvent::myEta(double teta)
{
  return -log(tan(teta/2.));
}

bool compvec(MYEvent* a, MYEvent* b, double vtxdiff)
{
  if (a->number == b->number) return false;
  // if (a->lumiblock != b->lumiblock) return false;
  if ( abs(a->lumiblock - b->lumiblock) > 10) return false;

  return ( fabs(a->vertex_pos - b->vertex_pos) < vtxdiff && fabs(a->multipl - b->multipl) <= (0.1 * a->multipl) );
}

bool comptrackspt(MYEvent* a, MYEvent* b,int iter1, int iter2, double ptdiff)
{
    return fabs(a->pt.at(iter1) - b->pt.at(iter2)) <= ptdiff;
}

bool event_sel(const xAOD::TrackParticle* tp)
{
  uint8_t trkPixelHits(0), trkSCTHits(0), trkExpectHit(0), trkExpectNextHit(0);

  tp->summaryValue(trkExpectHit, xAOD::expectInnermostPixelLayerHit);
  if (!trkExpectHit)
  {
    tp->summaryValue(trkExpectNextHit, xAOD::expectNextToInnermostPixelLayerHit);
    if (!trkExpectNextHit)
    return false;
  }

  tp->summaryValue(trkPixelHits, xAOD::numberOfPixelHits);
  if (trkPixelHits < 1)
  return false;

  tp->summaryValue(trkSCTHits, xAOD::numberOfSCTHits);
  if (tp->pt() > 0.1*1000 && tp->pt() < 0.3*1000){
    return trkSCTHits >= 2;
  }
  else if (tp->pt() > 0.3*1000 && tp->pt() < 0.4*1000){
    return trkSCTHits >= 4;
  }
  else if (tp->pt() > 0.4*1000) return trkSCTHits >= 6;
  else return true;
}

double sinztheta(const xAOD::TrackParticle* tp)
{
  return std::fabs(tp->z0() + tp->vz() - tp->vertex()->z())*(TMath::Sin(tp->theta()));
}

bool event_sel2(const xAOD::TrackParticle* tp)
{
  // printf("%f\n", std::fabs(tp->z0() + tp->vz() - tp->vertex()->z()) * std::fabs(TMath::Sin(tp->theta())));
  if  (
    tp->vertex()
    &&
    tp->vertex()->vertexType() == xAOD::VxType::PriVtx
    &&
    fabs(tp->vertex()->z()) < 15
    &&
    event_sel(tp)
    &&
    tp->pt() >= 0.1*1000
    &&
    tp->pt() >= 0.5*1000 && tp->pt() < 5*1000
    &&
    tp->d0() <= 1.5
    &&
    sinztheta(tp) < 1.5
    &&
    tp->eta() < 2.5
  )
  return true;
  else return false;

}
