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

#include "TangoAlgorithm.hh"
#include <algorithm>
#include <cmath>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "ComptonConstraints.hh"
#include <boost/property_tree/json_parser.hpp>

namespace unit = anlgeant4::unit;

namespace comptonsoft {

using namespace compton_constraints;

TangoAlgorithm::TangoAlgorithm()
  : sigma_level_energy_margin_for_checkScatteringAngle_(5.0),
    process_mode(0), // 0: assume full deposit & escapoe simul, 1: first assume full dep, if no order is OK, then assume escape, 2: only full deposit, 3: only escape
    FOM_function_type(0), //0: exponential, 1: gaussian
    assume_initial_gammaray_energy(false),
    known_initial_gammaray_energy(1000.0 * unit::keV),
    assume_initial_direction(false),
    use_averaged_escaped_energy(true),
    ignore_out_of_costheta_candidate(true),
    use_original_escape_length_calc(false),
    use_costheta(false),
    par0_energy_resolution_(5.0),
    par1_energy_resolution_(0.5),
    par2_energy_resolution_(0.0),
    detector_length_scale(15.0), // cm
    escape_length_scale(15.0), // cm
    consider_position_resolution(true),
    position_resolution_x_(0.2 * unit::cm),
    position_resolution_y_(0.2 * unit::cm),
    position_resolution_z_(0.2 * unit::cm),
    escape_weight_(1.0),
    cross_section_filename_("crosssection.root")
{
  setParameterFile("parfile_TANGO.json");
}

TangoAlgorithm::~TangoAlgorithm() = default;

bool TangoAlgorithm::loadParameters(boost::property_tree::ptree& pt)
{
  sigma_level_energy_margin_for_checkScatteringAngle_ = pt.get<double>("TANGO.sigma_level_energy_margin_for_checkScatteringAngle");
  process_mode = pt.get<int>("TANGO.process_mode");
  FOM_function_type = pt.get<int>("TANGO.FOM_function_type");
  assume_initial_gammaray_energy = pt.get<bool>("TANGO.assume_initial_gammaray_energy");
  known_initial_gammaray_energy = pt.get<double>("TANGO.known_initial_gammaray_energy") * unit::keV;
  use_averaged_escaped_energy = pt.get<bool>("TANGO.use_averaged_escaped_energy");
  detector_length_scale = pt.get<double>("TANGO.detector_length_scale");
  escape_length_scale = pt.get<double>("TANGO.escape_length_scale");
  par0_energy_resolution_ = pt.get<double>("TANGO.energy_resolution.par0");
  par1_energy_resolution_ = pt.get<double>("TANGO.energy_resolution.par1");
  par2_energy_resolution_ = pt.get<double>("TANGO.energy_resolution.par2");
  consider_position_resolution = pt.get<bool>("TANGO.consider_position_resolution");
  position_resolution_x_ = pt.get<double>("TANGO.position_resolution.x") * unit::cm;
  position_resolution_y_ = pt.get<double>("TANGO.position_resolution.y") * unit::cm;
  position_resolution_z_ = pt.get<double>("TANGO.position_resolution.z") * unit::cm;
    
  try{
    assume_initial_direction = pt.get<bool>("TANGO.assume_initial_direction");
    if(assume_initial_direction){
      known_initial_direction.set( pt.get<double>("TANGO.known_initial_direction.x"),
                                   pt.get<double>("TANGO.known_initial_direction.y"),
                                   pt.get<double>("TANGO.known_initial_direction.z") );
    }
  }catch (...){

  }

  try{
    ignore_out_of_costheta_candidate = pt.get<bool>("TANGO.ignore_out_of_costheta_candidate");
  }catch (...){

  }

  try{
    use_original_escape_length_calc = pt.get<bool>("TANGO.use_original_escape_length_calc");
  }catch (...){

  }

  try{
    use_costheta = pt.get<bool>("TANGO.use_costheta");
  }catch (...){

  }

  escape_weight_ = pt.get<double>("TANGO.escape_weight");
  cross_section_filename_ = pt.get<std::string>("TANGO.cross_section_filename");
  
  cross_section_file_ = new TFile(cross_section_filename_.c_str(), "r");
  tg_cross_section_compton_ = (TGraph*)cross_section_file_->Get("compton");
  tg_cross_section_phot_abs_ = (TGraph*)cross_section_file_->Get("phot_abs");
  tg_cross_section_pair_ = (TGraph*)cross_section_file_->Get("pair");
  tg_cross_section_tot_ = (TGraph*)cross_section_file_->Get("tot_wo_coherent");

  std::cout << std::endl;
  std::cout << "--- TANGO ---" << std::endl;
  std::cout << "sigma_level_energy_margin_for_checkScatteringAngle_ : " << sigma_level_energy_margin_for_checkScatteringAngle_ << std::endl;
  std::cout << "process_mode : " << process_mode << std::endl;
  std::cout << "FOM_function_type: " << FOM_function_type << std::endl;
  std::cout << "assume_initial_gammaray_energy : " << assume_initial_gammaray_energy << std::endl;
  if (assume_initial_gammaray_energy) {
    std::cout << "known_initial_gammaray_energy (keV) : " << known_initial_gammaray_energy / unit::keV << std::endl;
  }
  std::cout << "use_averaged_escaped_energy : " << use_averaged_escaped_energy << std::endl;
  std::cout << "ignore_out_of_costheta_candidate: " << ignore_out_of_costheta_candidate << std::endl;
  std::cout << "detector_length_scale (cm) : " << detector_length_scale << std::endl;
  std::cout << "escape_length_scale (cm) : " << escape_length_scale << std::endl;
  std::cout << "par0_energy_resolution_ : " << par0_energy_resolution_ << std::endl;
  std::cout << "par1_energy_resolution_ : " << par1_energy_resolution_ << std::endl;
  std::cout << "par2_energy_resolution_ : " << par2_energy_resolution_ << std::endl;
  std::cout << "cross_section_filename_ : " << cross_section_filename_ << std::endl;
  std::cout << "consider_position_resolution : " << consider_position_resolution << std::endl;
  if (consider_position_resolution) {
    std::cout << "position_resolution_x_ (cm) : " << position_resolution_x_ / unit::cm << std::endl;
    std::cout << "position_resolution_y_ (cm) : " << position_resolution_y_ / unit::cm << std::endl;
    std::cout << "position_resolution_z_ (cm) : " << position_resolution_z_ / unit::cm << std::endl;
  }
  if (assume_initial_direction) {
    std::cout << "assume_initial_direction : " << assume_initial_direction << std::endl;
    std::cout << "known_initial_direction (x) : " << known_initial_direction.x() << std::endl;
    std::cout << "known_initial_direction (y) : " << known_initial_direction.y() << std::endl;
    std::cout << "known_initial_direction (z) : " << known_initial_direction.z() << std::endl;
  }
  std::cout << "use_costheta : " << use_costheta << std::endl;
  std::cout << "escape_weight : " << escape_weight_ << std::endl;

  if(ignore_length_inverse_square){
    std::cout << "!!!WARNING!!! ignore_length_inverse_square is set to TRUE !!! This is the original version of TANGO algorithm !!!" << std::endl;
  }
  if(Kroger_mode){
    std::cout << "!!!WARNING!!! Kroger_mode is set to TRUE !!!" << std::endl;
  }
  if(use_original_escape_length_calc){
    std::cout << "!!!WARNING!!! use_original_escape_length_calc is just for Yoneda et al. 2022. The detector size is assumed as 140x140x20 cm3. Do not use for general application." << std::endl;
  }
  if(!(FOM_function_type == 0 || FOM_function_type == 1 || FOM_function_type == 2)){
    std::cout << "FOM_function_type should be 0 or 1 or 2." << std::endl;
    return false;
  }

  return true;
}

void TangoAlgorithm::initializeEvent()
{
}

bool TangoAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  setTotalEnergyDepositsAndNumHits(hits);

  if (num_hits_ > MaxHits() || num_hits_< 2) {
    return false;
  }

  bool result = false;
  if (process_mode == 0 || process_mode == 1 || process_mode == 2) {
    result = reconstructFullDepositEvent(hits, baseEvent, eventsReconstructed);
  }

  if (process_mode == 0 || (process_mode == 1 && !result) || process_mode == 3) {
    result |= reconstructEscapeEvent(hits, baseEvent, eventsReconstructed);
  }

  if (result == true) {
    normalizeReconstructionFraction(eventsReconstructed);
    if (selecting_maximum_likelihood_order_) {
      selectMaximumLikelihoodOrder(eventsReconstructed);
    }
    return true;
  }
  return false;
}

bool TangoAlgorithm::
reconstructFullDepositEvent(const std::vector<DetectorHit_sptr>& hits,
                            const BasicComptonEvent& baseEvent,
                            std::vector<BasicComptonEven