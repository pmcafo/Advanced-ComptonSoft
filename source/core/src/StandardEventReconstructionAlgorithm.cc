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

#include "StandardEventReconstructionAlgorithm.hh"
#include <list>
#include <algorithm>
#include <numeric>
#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

StandardEventReconstructionAlgorithm::StandardEventReconstructionAlgorithm()
  : fluorescenceCut_(false)
{
}

StandardEventReconstructionAlgorithm::~StandardEventReconstructionAlgorithm() = default;

void StandardEventReconstructionAlgorithm::initializeEvent()
{
  setFluorescenceCut(false);
}

bool StandardEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  const int NumHits = hits.size();
  if (NumHits>MaxHits()) {
    return false;
  }

  bool result = false;
  auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);
  eventReconstructed->setTotalEnergyDeposit(total_energy_deposits(hits));

  if (NumHits==2) {
    const bool hit0Si = hits[0]->isFlags(flag::LowZHit);
    const bool hit1Si = hits[1]->isFlags(flag::LowZHit);
    const bool hit0CdTe = hits[0]->isFlags(flag::HighZHit);
    const bool hit1CdTe = hits[1]->isFlags(flag::HighZHit);

    if (hit0Si && hit1Si) {
      result = reconstruct2HitsSameMaterial(hits, *eventReconstructed);
    }
    else if (hit0CdTe && hit1CdTe) {
      result = reconstruct2HitsSameMaterial(hits, *eventReconstructed);
    }
    else if (hit0Si && hit1CdTe) {
      result = reconstruct2HitsSiCdTe(hits, *eventReconstructed);
    }
    else if (hit0CdTe && hit1Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(2);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[0];
      result = reconstruct2HitsSiCdTe(hitsOrdered, *eventReconstructed);
    }
  }
  else if (NumHits==3) {
    const bool hit0Si = hits[0]->isFlags(flag::LowZHit);
    const bool hit1Si = hits[1]->isFlags(flag::LowZHit);
    const bool hit2Si = hits[2]->isFlags(flag::LowZHit);
    const bool hit0CdTe = hits[0]->isFlags(flag::HighZHit);
    const bool hit1CdTe = hits[1]->isFlags(flag::HighZHit);
    const bool hit2CdTe = hits[2]->isFlags(flag::HighZHit);

    if (hit0Si && hit1Si && hit2CdTe) {
      result = reconstruct3HitsSiSiCdTe(hits, *eventReconstructed);
    }
    else if (hit0Si && hit1CdTe && hit2Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[0];
      hitsOrdered[1] = hits[2];
      hitsOrdered[2] = hits[1];
      result = reconstruct3HitsSiSiCdTe(hitsOrdered, *eventReconstructed);
    }
    else if (hit0CdTe && hit1Si && hit2Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[2];
      hitsOrdered[2] = hits[0];
      result = reconstruct3HitsSiSiCdTe(hitsOrdered, *eventReconstructed);
    }
    else if (hit0Si && hit1CdTe && hit2CdTe) {
      result = reconstruct3HitsSiCdTeCdTeByFOM(hits, *eventReconstructed);
    }
    else if (hit0CdTe && hit1Si && hit2CdTe) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[0];
      hitsOrdered[2] = hits[