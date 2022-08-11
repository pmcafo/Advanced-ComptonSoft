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

#ifndef COMPTONSOFT_DetectorHit_H
#define COMPTONSOFT_DetectorHit_H 1

#include <cstdint>
#include <memory>

// If you use boost::pool library for fast memory allocation,
// set DetectorHit_BoostPool as 1; otherwise 0.
#define DetectorHit_BoostPool 1

#if DetectorHit_BoostPool
#include <boost/pool/pool.hpp>
#endif

#include "CSTypes.hh"
#include "ChannelID.hh"
#include "VoxelID.hh"
#include "PixelID.hh"
#include "FlagDefinition.hh"

namespace comptonsoft {

/**
 * A class of a hit in a semiconductor or scintillation detector.
 * This class contains an energy deposited, position, time, detector ID, and so on.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-22
 * @date 2012-06-29 | rename DetectorID; add Instrument ID
 * @date 2014-09-18 | redesign.
 * @date 2016-08-19 | add trigger information, change channel ID classes
 * @date 2020-09-02 | add errors of energy/position/time
 * @date 2020-11-24 | add particle type
 * @date 2020-12-25 | add track ID
 * @date 2022-04-25 | introduce a voxel
 */
class DetectorHit
{
public:
  enum class MergedPosition { KeepLeft, Midpoint, EnergyWeighted, CopyRight };

public:
  DetectorHit() = default;
  ~DetectorHit();

  DetectorHit(const DetectorHit&) = default;
  DetectorHit(DetectorHit&&) = default;
  DetectorHit& operator=(const DetectorHit&) = default;
  DetectorHit& operator=(DetectorHit&&) = default;

  DetectorHit& operator+=(const DetectorHit& r) { return merge(r); }

  std::shared_ptr<DetectorHit> clone() const
  { return std::shared_ptr<DetectorHit>(new DetectorHit(*this)); }
  
  void setEventID(int64_t v) { eventID_ = v; }
  int64_t EventID() const { return eventID_; }

  void setTrackID(int v) { trackID_ = v; }
  int TrackID() const { return trackID_; }

  void setTimeIndicator(int64_t v) { ti_ = v; }
  int64_t TimeIndicator() const { return ti_; }
  void setTI(int64_t v) { ti_ = v; }
  int64_t TI() const { return ti_; }

  void setInstrumentID(int v) { instrumentID_ = v; }
  int InstrumentID() const { return instrumentID_; }

  void setDetectorChannelID(const DetectorBasedChannelID& v)
  { detectorChannelID_ = v; }
  void setDetectorChannelID(int detectorID, int section, int index)
  { detectorChannelID_.set(detectorID, section, index); }
  void setDetectorID(int detectorID)
  { detectorChannelID_.set(detectorID, ChannelID::Undefined, ChannelID::Undefined); }
  DetectorBasedChannelID DetectorChannelID() const { return detectorChannelID_; }
  int DetectorID() const { return detectorChannelID_.Detector(); }
  int DetectorSection() const { return detectorChannelID_.Section(); }
  int DetectorChannel() const { return detectorChannelID_.Channel(); }

  void setReadoutChannelID(const ReadoutBasedChannelID& v)
  { readoutChannelID_ = v; }
  void setReadoutChannelID(int readoutModuleID, int section, int index)
  { readoutChannelID_.set(readoutModuleID, section, index); }
  ReadoutBasedChannelID ReadoutChannelID() const { return readoutChannelID_; }
  int ReadoutModuleID() const { return readoutChannelID_.ReadoutModule(); }
  int ReadoutSection() const { return readoutChannelID_.Section(); }
  int ReadoutChannel() const { return readoutChannelID_.Channel(); }

  void setVoxel(const VoxelID& v) { voxel_ = v; }
  void setVoxel(int x, int y, int z) { voxel_.set(x, y, z); }
  int VoxelX() const { return voxel_.X(); }
  int VoxelY() const { return voxel_.Y(); }
  int VoxelZ() const { return voxel_.Z(); }
  VoxelID Voxel() const { return voxel_; }

  void setPixel(int x, int y) { voxel_.set(x, y); }
  void setPixel(const PixelID& v) { voxel_ = v; }
  int PixelX() const { return voxel_.X(); }
  int PixelY() const { return voxel_.Y(); }
  PixelID Pixel() const { return voxel_; }

  void setRawPHA(int32_t v) { rawPHA_ = v; }
  int32_t RawPHA() const { return rawPHA_; }
  void setPHA(double v) { PHA_ = v; }
  double PHA() const { return PHA_; }
  void setEPI(double v) { EPI_ = v; }
  double EPI() const { return EPI_; }
  void setEPIError(double v) { EPIError_ = v; }
  double EPIError() const { return EPIError_; }

  void setFlagData(uint64_t v) { flagData_ = v; }
  uint64_t FlagData() const { return flagData_; }
  void addFlagData(uint64_t f) { flagData_ |= f; }
  void clearFlagData(uint64_t f) { flagData_ &= ~f; }
  bool isFlagData(uint64_t f) const { return (flagData_&f)==f; }

  void setFlags(uint64_t v) { flags_ = v; }
  uint64_t Flags() const { return flags_; }
  void addFlags(uint64_t f) { flags_ |= f; }
  void clearFlags(uint64_t f) { flags_ &= ~f; }
  bool isFlags(uint64_t f) const { return (flags_&f)==f; }

  void setParticle(int v) { particle_ = v; }
  int Particle() const { return particle_; }

  void setRealTime(double v) { realTime_ = v; }
  double RealTime() const { return realTime_; }
  void setTimeGroup(int32_t v) { timeGroup_ = v; }
  int TimeGroup() const { return timeGroup_; }

  void setRealPosition(double x, double y, double z)
  { realPosition_.set(x, y, z); }
  void setRealPosition(const vector3_t& v) { realPosition_ = v; }
  double RealPositionX() const { return realPosition_.x(); }
  double RealPositionY() const { return realP