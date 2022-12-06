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

#include "EventTreeIO.hh"
#include "AstroUnits.hh"
#include "TTree.h"
#include "DetectorHit.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

EventTreeIO::EventTreeIO()
  : tree_(nullptr)
{
}

EventTreeIO::~EventTreeIO() = default;

void EventTreeIO::defineBranches()
{
  tree_->Branch("eventid",        &eventid_,              "eventid/L");
  tree_->Branch("num_hits",       &num_hits_,             "num_hits/I");
  
  // measured data
  tree_->Branch("ti",             &ti_,                   "ti/L");
  tree_->Branch("instrument",     &instrument_,           "instrument/S");
  tree_->Branch("detector",       detector_.data(),       "detector[num_hits]/S");
  tree_->Branch("det_section",    det_section_.data(),    "det_section[num_hits]/S");
  tree_->Branch("readout_module", readout_module_.data(), "readout_module[num_hits]/S");
  tree_->Branch("section",        section_.data(),        "section[num_hits]/S");
  tree_->Branch("channel",        channel_.data(),        "channel[num_hits]/S");
  tree_->Branch("pixelx",         pixelx_.data(),         "pixelx[num_hits]/S");
  tree_->Branch("pixely",         pixely_.data(),         "pixely[num_hits]/S");
  tree_->Branch("pixelz",         pixelz_.data(),         "pixelz[num_hits]/S");
  tree_->Branch("rawpha",         rawpha_.data(),         "rawpha[num_hits]/I");
  tree_->Branch("pha",            pha_.data(),            "pha[num_hits]/F");
  tree_->Branch("epi",            epi_.data(),            "epi[num_hits]/F");
  tree_->Branch("flag_data",      &flag_data_,            "flag_data/l");
  tree_->Branch("flags",          &flags_,                "flags/l");
  
  // simulation
  tree_->Branch("trackid",        trackid_.data(),        "trackid[num_hits]/I");
  tree_->Branch("particle",       particle_.data(),       "particle[num_hits]/I");
  tree_->Branch("real_time",      real_time_.data(),      "real_time[num_hits]/D");
  tree_->Branch("time_trig",      time_trig_.data(),      "time_trig[num_hits]/D");
  tree_->Branch("time_group",     time_group_.data(),     "time_group[num_hits]/S");
  tree_->Branch("real_posx",      real_posx_.data(),      "real_posx[num_hits]/F");
  tree_->Branch("real_posy",      real_posy_.data(),      "real_posy[num_hits]/F");
  tree_->Branch("real_posz",      real_posz_.data(),      "real_posz[num_hits]/F");
  tree_->Branch("edep",           edep_.data(),           "edep[num_hits]/F");
  tree_->Branch("echarge",        echarge_.data(),        "echarge[num_hits]/F");
  tree_->Branch("process",        process_.data(),        "process[num_hits]/i");

  // reconstructed
  tree_->Branch("energy",         energy_.data(),         "energy[num_hits]/F");
  tree_->Branch("posx",           posx_.data(),           "posx[num_hits]/F");
  tree_->Branch("posy",           posy_.data(),           "posy[num_hits]/F");
  tree_->Branch("posz",           posz_.data(),           "posz[num_hits]/F");
  tree_->Branch("local_posx",     local_posx_.data(),     "local_posx[num_hits]/F");
  tree_->Branch("local_posy",     local_posy_.data(),     "local_posy[num_hits]/F");
  tree_->Branch("local_posz",     local_posz_.data(),     "local_posz[num_hits]/F");
  tree_->Branch("time",           time_.data(),           "time[num_hits]/D");
  tree_->Branch("grade",          &grade_,                "grade/I");
}

void EventTreeIO::setBranchAddresses()
{
  tree_->SetBranchAddress("eventid",        &eventid_);
  tree_->SetBranchAddress("num_hits",       &num_hits_);

  // measured data
  tree_->SetBranchAddress("ti",             &ti_);
  tree_->SetBranchAddress("instrument",     &instrument_);
  tree_->SetBranchAddress("detector",       detector_.data());
  tree_->SetBranchAddress("det_section",    det_section_.data());
  tree_->SetBranchAddress("readout_module", readout_module_.data());
  tree_->SetBranchAddress("section",        section_.data());
  tree_->SetBranchAddress("channel",        channel_.data());
  tree_->SetBranchAddress("pixelx",         pixelx_.data());
  tree_->SetBranchAddress("pixely",         pixely_.data());
  tree_->SetBranchAddress("pixelz",         pixelz_.data());
  tree_->SetBranchAddress("rawpha",         rawpha_.data());
  tree_->SetBranchAddress("pha",            pha_.data());
  tree_->SetBranchAddress("epi",            epi_.data());
  tree_->SetBranchAddress("flag_data",      &flag_data_);
  tree_->SetBranchAddress("flags",          &flags_);

  // simulation
  tree_->SetBranchAddress("trackid",        trackid_.data());
  tree_->SetBranchAddress("particle",       particle_.data());
  tree_->SetBranchAddress("real_time",      real_time_.data());
  tree_->SetBranchAddress("time_trig",      time_trig_.data());
  tree_->SetBranchAddress("time_group",     time_group_.data());
  tree_->SetBranchAddress("real_posx",      real_posx_.data());
  tree_->SetBranchAddress("real_posy",      real_posy_.data());
  tree_->SetBranchAddress("real_posz",      real_posz_.data());
  tree_->SetBranchAddress("edep",           edep_.data());
  tree_->SetBranchAddress("echarge",        echarge_.data());
  tree_->SetBranchAddress("process",        process_.data());

  // reconstructed
  tree_->SetBranchAddress("energy",         energy_.data());
  tree_->SetBranchAddress("posx",           posx_.data());
  tree_->SetBranchAddress("posy",           posy_.data());
  tree_->SetBranchAddress("posz",           posz_.data());
  tree_->SetBranchAddress("local_posx", 