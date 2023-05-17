
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

#include "PhysicsListManager.hh"
#include "AstroUnits.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_HP.hh"
#include "FTFP_BERT.hh"
#include "FTFP_BERT_HP.hh"
#include "FTFP_INCLXX.hh"
#include "FTFP_INCLXX_HP.hh"
#include "Shielding.hh"
#include "G4NuclearLevelData.hh"
#include "PhysicsListWithRadioactiveDecay.hh"

using namespace anlnext;

namespace comptonsoft
{

PhysicsListManager::PhysicsListManager()
  : m_PhysicsListName("CSPhysicsList"),
    m_EMPolarization(false),
    m_EMCustomized(false),
    m_EMOptionFluo(true),
    m_EMOptionAuger(false),
    m_EMOptionPIXE(false),
    m_ElectronRangeRatio(0.2),
    m_ElectronFinalRange(100.0*CLHEP::um),
    m_HadronHP(false),
    m_HadronModel("BIC"),
    m_NuclearLifeTimeThreshold(1.0e-12*CLHEP::second),
    m_RDEnabled(false),
    m_ParallelWorldEnabled(true),
    m_DefaultCut(0.0001*CLHEP::cm)
{
  add_alias(module_name());
}

ANLStatus PhysicsListManager::mod_define()
{
  anlgeant4::VANLPhysicsList::mod_define();
  register_parameter(&m_DefaultCut, "cut_value", CLHEP::cm, "cm");
  register_parameter(&m_PhysicsListName, "physics_list");
  register_parameter(&m_EMPolarization, "polarization");
  register_parameter(&m_EMCustomized, "customized_em");
  register_parameter(&m_EMOptionFluo, "em_option_fluo");
  register_parameter(&m_EMOptionAuger, "em_option_auger");
  register_parameter(&m_EMOptionPIXE, "em_option_pixe");
  register_parameter(&m_ElectronRangeRatio, "electron_range_ratio");
  register_parameter(&m_ElectronFinalRange, "electron_final_range", CLHEP::cm, "cm");
  register_parameter(&m_HadronHP, "hadron_hp");
  register_parameter(&m_HadronModel, "hadron_model");
  register_parameter(&m_NuclearLifeTimeThreshold, "nuclear_lifetime_threshold", CLHEP::second, "s");
  register_parameter(&m_RDEnabled, "radioactive_decay");
  register_parameter(&m_ParallelWorldEnabled, "parallel_world");

  return AS_OK;
}

ANLStatus PhysicsListManager::mod_pre_initialize()
{
  if (m_PhysicsListName == "CSPhysicsList") {
    if (!m_EMCustomized) {
      hide_parameter("em_option_fluo");
      hide_parameter("em_option_auger");
      hide_parameter("em_option_pixe");
      hide_parameter("electron_range_ratio");
      hide_parameter("electron_final_range");
    }
  }
  else {
    hide_parameter("polarization");
    hide_parameter("customized_em");
    hide_parameter("em_option_fluo");
    hide_parameter("em_option_auger");
    hide_parameter("em_option_pixe");
    hide_parameter("electron_range_ratio");
    hide_parameter("electron_final_range");
    hide_parameter("hadron_hp");
    hide_parameter("hadron_model");
    hide_parameter("radioactive_decay");
    hide_parameter("parallel_world");
  }
  
  return AS_OK;
}

ANLStatus PhysicsListManager::mod_initialize()
{
  CSPhysicsOption option;

  // EM physics model
  if (m_EMCustomized) {
    if (m_EMPolarization) {
      m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::CustomizedLivermorePolarized);
    }
    else {
      m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::CustomizedLivermore);
    }
    m_PhysicsOption.setEMOptions(m_EMOptionFluo, m_EMOptionAuger, m_EMOptionPIXE);
    m_PhysicsOption.setElectronRangeParameters(m_ElectronRangeRatio, m_ElectronFinalRange);
  }
  else {
    if (m_EMPolarization) {
      m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::LivermorePolarized);
    }
    else {
      m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::Livermore);
    }
  }

  // hardron high-precision model
  m_PhysicsOption.setHadronPhysicsHighPrecision(m_HadronHP);

  // hardron inelastic interaction model
  if (m_HadronModel=="BERT") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BERT);
  }
  else if (m_HadronModel=="BIC") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC);
  }
  else if (m_HadronModel=="INCLXX" || m_HadronModel=="INCL++") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::INCLXX);
  }
  else if (m_HadronModel=="BERT_HP") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BERT_HP);
  }
  else if (m_HadronModel=="BIC_HP") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC_HP);
  }
  else if (m_HadronModel=="BIC_AllHP") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC_AllHP);
  }
  else {
    std::cout << "PhysicsListManager: \n"
              << "Unknown hadron model is given. ===> " << m_HadronModel
              << std::endl;
    return AS_QUIT_ERROR;
  }

  // radioactive decay
  m_PhysicsOption.enableRadioactiveDecay(m_RDEnabled);

  // set geant4 parameters
  G4NuclearLevelData::GetInstance()->GetParameters()->SetMaxLifeTime(m_NuclearLifeTimeThreshold);

  return AS_OK;
}

G4VUserPhysicsList* PhysicsListManager::create()
{
  G4VUserPhysicsList* physicsList(nullptr);

  if (m_PhysicsListName == "CSPhysicsList") {
    CSPhysicsList* pl = new CSPhysicsList(m_PhysicsOption);
    pl->enableParallelWorld(m_ParallelWorldEnabled);
    physicsList = pl;
  }
  else if (m_PhysicsListName == "QGSP_BIC") {
    physicsList = new QGSP_BIC;
  }
  else if (m_PhysicsListName == "QGSP_BIC_HP") {
    physicsList = new QGSP_BIC_HP;
  }
  else if (m_PhysicsListName == "QGSP_BIC_HP_RD") {
    physicsList = new PhysicsListWithRadioactiveDecay<QGSP_BIC_HP>;
  }
  else if (m_PhysicsListName == "FTFP_BERT") {
    physicsList = new FTFP_BERT;
  }
  else if (m_PhysicsListName == "FTFP_BERT_HP") {
    physicsList = new FTFP_BERT_HP;
  }
  else if (m_PhysicsListName == "FTFP_BERT_HP_RD") {
    physicsList = new PhysicsListWithRadioactiveDecay<FTFP_BERT_HP>;
  }
  else if (m_PhysicsListName == "FTFP_INCLXX") {
    physicsList = new FTFP_INCLXX;
  }
  else if (m_PhysicsListName == "FTFP_INCLXX_HP") {
    physicsList = new FTFP_INCLXX_HP;
  }
  else if (m_PhysicsListName == "FTFP_INCLXX_HP_RD") {
    physicsList = new PhysicsListWithRadioactiveDecay<FTFP_INCLXX_HP>;
  }
  else if (m_PhysicsListName == "Shielding") {
    physicsList = new Shielding;
  }
  else {
    std::cout << "PhysicsListManager: unknown physics list is given: "
              << m_PhysicsListName
              << std::endl;
    return physicsList;
  }

  physicsList->SetDefaultCutValue(m_DefaultCut);

  return physicsList;
}

} /* namespace comptonsoft */