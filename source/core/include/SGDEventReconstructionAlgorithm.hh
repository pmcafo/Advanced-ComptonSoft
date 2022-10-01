
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

#ifndef COMPTONSOFT_SGDEventReconstructionAlgorithm_H
#define COMPTONSOFT_SGDEventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"

namespace comptonsoft {

/**
 * A class of a standard Compton event reconstuction algorithm.
 * @author Hirokazu Odaka
 * @date 2011-02-16
 * @date 2014-11-17
 */
class SGDEventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  SGDEventReconstructionAlgorithm();
  virtual ~SGDEventReconstructionAlgorithm();
  SGDEventReconstructionAlgorithm(const SGDEventReconstructionAlgorithm&) = default;
  SGDEventReconstructionAlgorithm(SGDEventReconstructionAlgorithm&&) = default;
  SGDEventReconstructionAlgorithm& operator=(const SGDEventReconstructionAlgorithm&) = default;
  SGDEventReconstructionAlgorithm& operator=(SGDEventReconstructionAlgorithm&&) = default;

  void initializeEvent() override;

  /**
   * perform the event reconstruction.
   * @param hits vector of hits to reconstruct.
   * @param baseEvent event based for new reconstructed events.
   * @param eventsReconstructed events reconstructed.
   * @return true if the reconstruction is successful.
   */
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   const BasicComptonEvent& baseEvent,
                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed) override;

  bool FluorescenceCut() const { return fluorescenceCut_; }
  void setFluorescenceCut(bool v) { fluorescenceCut_ = v; }
  bool checkFluorescence(DetectorHit_sptr fluorescence,
                         DetectorHit_sptr absorption) const;

protected:
  bool reconstruct2HitsSiCdTe(const std::vector<DetectorHit_sptr>& hits,
                              BasicComptonEvent& eventReconstructed);
  bool reconstruct2HitsSameMaterial(const std::vector<DetectorHit_sptr>& hits,
                                    BasicComptonEvent& eventReconstructed);
  bool reconstruct3HitsSiSiCdTe(const std::vector<DetectorHit_sptr>& hits,
                                BasicComptonEvent& eventReconstructed);
  bool reconstruct3HitsSiCdTeCdTe(const std::vector<DetectorHit_sptr>& hits,
                                  BasicComptonEvent& eventReconstructed);
  bool reconstruct3HitsSiCdTeCdTeByFOM(const std::vector<DetectorHit_sptr>& hits,
                                       BasicComptonEvent& eventReconstructed);

  /**
   * Extract a partial set of hits that has a total energy agreeing with energy.
   * 
   */
  bool extractEventsByTotalEnergy(const std::vector<DetectorHit_sptr>& hits,
                                  double energy, double deltaE, int maxHits,
                                  std::vector<DetectorHit_sptr>& output);

private:
  bool fluorescenceCut_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SGDEventReconstructionAlgorithm_H */