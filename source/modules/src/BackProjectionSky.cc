
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

#include "BackProjectionSky.hh"

#include "AstroUnits.hh"
#include "TDirectory.h"
#include "TRandom3.h"
#include "TMath.h"
#include "BasicComptonEvent.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

BackProjectionSky::BackProjectionSky()
  : m_Rotation(0.0)
{
  setUnit(unit::degree, "degree");
}

BackProjectionSky::~BackProjectionSky() = default;

ANLStatus BackProjectionSky::mod_define()
{
  BackProjection::mod_define();

  unregister_parameter("plane_normal");
  unregister_parameter("plane_point");
  register_parameter(&m_Rotation, "rotation_angle", unit::degree, "degree");

  return AS_OK;
}

ANLStatus BackProjectionSky::mod_initialize()
{
  BackProjection::mod_initialize();
  m_XAxis.set(std::cos(m_Rotation), std::sin(m_Rotation), 0.0);
  m_YAxis.set(-m_XAxis.y(), m_XAxis.x(), 0.0);
  m_ZAxis.set(0.0, 0.0, 1.0);

  return AS_OK;
}

ANLStatus BackProjectionSky::mod_analyze()
{
  const std::vector<BasicComptonEvent_sptr> events = getEventReconstructionModule()->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();

    static TRandom3 randgen;
    const int Times = 1000;
    const double FillWeight = fraction/Times;
  
    const vector3_t coneVertex = event->ConeVertex();
    const vector3_t coneAxis = event->ConeAxis();
    const vector3_t coneAxisOrtho = coneAxis.orthogonal();
    const double cosThetaE = event->CosThetaE();

    vector3_t cone1(coneAxis);
    double thetaE = TMath::ACos(cosThetaE);
    cone1.rotate(thetaE, coneAxisOrtho);

    vector3_t coneSample;
    double phi;
  
    for (int t=0; t<Times; t++) {
      coneSample = cone1;
      phi = randgen.Uniform(0.0, TMath::TwoPi());
      coneSample.rotate(phi, coneAxis);
      double ux = coneSample.dot(m_XAxis);
      double uy = coneSample.dot(m_YAxis);
      double uz = coneSample.dot(m_ZAxis);
      double x = std::atan2(ux, uz);
      double y = 0.5*CLHEP::pi - std::acos(uy);

      fillImage(x/PixelUnit(), y/PixelUnit(), FillWeight);
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */