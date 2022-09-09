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

#ifndef COMPTONSOFT_MultiChannelData_H
#define COMPTONSOFT_MultiChannelData_H 1

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <algorithm>
#include "CSTypes.hh"
#include "FlagDefinition.hh"
#include "ChannelID.hh"
#include "VGainFunction.hh"

namespace comptonsoft {

/**
 * A class of data output from one ASIC.
 *
 * @author Hirokazu Odaka
 * @date 2007-12-xx
 * @date 2009-05-20 | change convertPHA2PI(), extrapolation calibration range
 * @date 2012-06-29 | simplification
 * @date 2014-09-07 | switch from array to vectors, introduce ElectrodeSide, change API.
 * @date 2016-05-02 | PHA randomization
 * @date 2016-08-19 | threshold: scalar value to vector
 * @date 2016-11-11 | add time and flags
 * @date 2019-10-08 | use shared_ptr for the gain function; delete the assignment operators.
 * @date 2020-03-26 | adapt a change of VGainFunction
 */
class MultiChannelData
{
public:
  MultiChannelData(std::size_t num_channels, ElectrodeSide eside);
  virtual ~MultiChannelData();
  MultiChannelData(const MultiChannelData&) = default;
  MultiChannelData(MultiChannelData&&) = default;

  std::size_t NumberOfChannels() const { return NumChannels_; }
  ElectrodeSide getElectrodeSide() const { return ElectrodeSide_; }
  bool isAnodeSide() const
  { return ElectrodeSide_==ElectrodeSide::Anode; }
  bool isCathodeSide() const
  { return ElectrodeSide_==ElectrodeSide::Cathode; }

  void setReadoutID(int readoutModuleID, int section)
  { return readoutID_.set(readoutModuleID, section, ChannelID::Undefined); }
  ReadoutBasedChannelID ReadoutID() const
  { return readoutID_; }

  bool isPrioritySide() const { return prioritySide_; }
  void setPrioritySide(bool v=true) { prioritySide_ = v; }

  void setUseNegativePulse(bool val)
  { useNegativePulse_ = val; }
  bool getUseNegativePulse() const
  { return useNegativePulse_; }

  void resetThresholdEnergyVector(double val)
  { std::fill(thresholdEnergyVector_.begin(), thresholdEnergyVector_.end(), val); }
  void setThresholdEnergy(std::size_t i, double val) { thresholdEnergyVector_[i] = val; }
  void setThresholdEnergyVector(const std::vector<double>& v) { thresholdEnergyVector_ = v; }
  double getThresholdEnergy(std::size_t i) const { return thresholdEnergyVector_[i]; }
  void getThresholdEnergyVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(thresholdEnergyVector_.begin(), thresholdEnergyVector_.end(), v.begin());
  }

  void resetNegativeThresholdEnergyVector(double val)
  { std::fill(negativeThresholdEnergyVector_.begin(), negativeThresholdEnergyVector_.end(), val); }
  void setNegativeThresholdEnergy(std::size_t i, double val) { negativeThresh