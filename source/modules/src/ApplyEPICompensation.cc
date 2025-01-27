
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

#include "ApplyEPICompensation.hh"

#include "DeviceSimulation.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anlnext;

namespace comptonsoft
{

ApplyEPICompensation::ApplyEPICompensation()
{
}

ApplyEPICompensation::~ApplyEPICompensation() = default;

ANLStatus ApplyEPICompensation::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);
  return AS_OK;
}

ANLStatus ApplyEPICompensation::mod_analyze()
{
  const DetectorSystem* detectorManager = getDetectorManager();
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr hit: hits) {
      const int detectorID = hit->DetectorID();
      const DeviceSimulation* ds = detectorManager->getDeviceSimulationByID(detectorID);
      double EPI_value(0.0), EPI_error(0.0);
      std::tie(EPI_value, EPI_error) = ds->compensateEPI(hit->Pixel(), std::make_tuple(hit->EPI(), hit->EPIError()));
      hit->setEPI(EPI_value);
      hit->setEPIError(EPI_error);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */