
/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "BackProjection.hh"

#include "AstroUnits.hh"
#include "TDirectory.h"
#include "TRandom3.h"
#include "TMath.h"
#include "BasicComptonEvent.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

BackProjection::BackProjection()
  : m_EventReconstruction(0),
    m_PlaneNormal(0.0, 0.0, 1.0),
    m_PlanePoint(0.0*unit::cm, 0.0*unit::cm, 10.0*unit::cm),
    m_NumPixelX(256), m_NumPixelY(256),
    m_RangeX1(-4.0*unit::cm), m_RangeX2(+4.0*unit::cm),
    m_RangeY1(-4.0*unit::cm), m_RangeY2(+4.0*unit::cm),
    m_PixelUnit(unit::cm), m_PixelUnitName("cm")
{
}

BackProjection::~BackProjection() = default;

ANLStatus BackProjection::mod_define()
{
  register_parameter(&m_PlaneNormal, "plane_normal");
  register_parameter(&m_PlanePoint, "plane_point", 1, m_PixelUnitName);
  register_parameter(&m_NumPixelX, "num_pixel_x");
  register_parameter(&m_NumPixelY, "num_pixel_y");
  register_parameter(&m_RangeX1, "x_min", 1, m_PixelUnitName);
  register_parameter(&m_RangeX2, "x_max", 1, m_PixelUnitName);
  register_parameter(&m_RangeY1, "y_min", 1, m_PixelUnitName);
  register_parameter(&m_RangeY2, "y_max", 1, m_PixelUnitName);

  return AS_OK;
}

ANLStatus BackProjection::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module_NC("EventReconstruction", &m_EventReconstruction);
  
  mkdir();
  
  m_hist_bp_All  = new TH2D("h_bp_All","Back Projection (All)", m_NumPixelX, m_RangeX1, m_RangeX2, m_NumPixelY, m_RangeY1, m_RangeY2);

  const std::vector<HitPattern>& hitpat_vec = getDetectorManager()->getHitPatterns();
  m_hist_vec.resize(hitpat_vec.size());
  for (unsigned int i=0; i<hitpat_vec.size(); i++) {
    std::string hist_name = "h_bp_";
    std::string hist_title = "BackProjection (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    m_hist_vec[i] = new TH2D(hist_name.c_str(), hist_title.c_str(), m_NumPixelX, m_RangeX1, m_RangeX2, m_NumPixelY, m_RangeY1, m_RangeY2);
  }

  return AS_OK;
}

ANLStatus BackProjection::mod_analyze()
{
  const std::vector<BasicComptonEvent_sptr> events = getEventReconstructionModule()->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();

    static TRandom3 randgen;
    const int Times = 1000;
    const double FillWeight = fraction/double(Times);
  
    const vector3_t coneVertex = event->ConeVertex();
    const vector3_t coneAxis = event->ConeAxis();
    const vector3_t coneAxisOrtho = coneAxis.orthogonal();
    const double cosThetaE = event->CosThetaE();

    vector3_t cone1(coneAxis);
    double thetaE = TMath::ACos(cosThetaE);
    cone1.rotate(thetaE, coneAxisOrtho);

    vector3_t coneSample;
    double phi;
    vector3_t coneProjected;

    for (int t=0; t<Times; t++) {
      coneSample = cone1;
      phi = randgen.Uniform(0.0, TMath::TwoPi());
      coneSample.rotate(phi, coneAxis);
      bool bPositive = sectionConeAndPlane(coneVertex, coneSample, coneProjected);
  
      if (bPositive == false) {
        continue;
      }

      fillImage(coneProjected.x()/m_PixelUnit, coneProjected.y()/m_PixelUnit, FillWeight);
    }
  }
  
  return AS_OK;
}

void BackProjection::fillImage(double x, double y, double weight)
{
  // Filling histograms 
  // All
  m_hist_bp_All->Fill(x, y, weight);
  for(unsigned int i=0; i<m_hist_vec.size(); i++) {
    if(m_EventReconstruction->HitPatternFlag(i)) {
      m_hist_vec[i]->Fill(x, y, weight);
    }
  }
}

bool BackProjection::sectionConeAndPlane(const vector3_t& vertex, const vector3_t& cone, vector3_t& coneProjected)
{
  const double t = (m_PlaneNormal*(m_PlanePoint-vertex)) / (m_PlaneNormal*cone);
  coneProjected = vertex + t*cone;
  if (t < 0.) { return false; }
  return true;
}

} /* namespace comptonsoft */