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

#include "SimDetectorUnit2DStrip.hh"

#include <iostream>
#include <memory>
#include <cmath>
#include <boost/format.hpp>
#include "TRandom3.h"

#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "WeightingPotentialStrip.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

SimDetectorUnit2DStrip::SimDetectorUnit2DStrip()
  : usingSymmetry_(true),
    CCEMapXStrip_(0),
    CCEMapYStrip_(0),
    WPMapXStrip_(0),
    WPMapYStrip_(0)
{
  setNumPixelsInWPCalculation(20.0);
}

SimDetectorUnit2DStrip::~SimDetectorUnit2DStrip() = default;

void SimDetectorUnit2DStrip::initializeEvent()
{
  RealDetectorUnit2DStrip::initializeEvent();
  DeviceSimulation::initializeEvent();
}

void SimDetectorUnit2DStrip::simulatePulseHeights()
{
  const int N = NumberOfRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    const double edep = rawhit->EnergyDeposit();
    const double localposx = rawhit->LocalPositionX();
    const double localposy = rawhit->LocalPositionY();
    const double localposz = rawhit->LocalPositionZ();
    const PixelID sp = findPixel(localposx, localposy);
    
    if (edep == 0.0) {
      DetectorHit_sptr xhit(new DetectorHit(*rawhit));
      xhit->setPixel(sp.X(), PixelID::Undefined);
      if (isXStripSideAnode()) {
        xhit->addFlags(flag::AnodeSide);
        xhit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
      }
      else if (isXStripSideCathode()) {
        xhit->addFlags(flag::CathodeSide);
        xhit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
      }
      
      DetectorHit_sptr yhit(new DetectorHit(*rawhit));
      yhit->setPixel(PixelID::Undefined, sp.Y());
      if (isYStripSideAnode()) {
        yhit->addFlags(flag::AnodeSide);
        yhit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
      }
      else if (isYStripSideCathode()) {
        yhit->addFlags(flag::CathodeSide);
        yhit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
      }
  
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
      continue;
    }

    if (DiffusionMode()==0) {
      const PixelID xsp(sp.X(), PixelID::Undefined);
      const PixelID ysp(PixelID::Undefined, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
    }
    else {
      const PixelID xsp(sp.X(), PixelID::Undefined);
      const PixelID ysp(PixelID::Undefined, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      const double xECharge = xhit->EnergyCharge();
      const double yECharge = yhit->EnergyCharge();

      const int numDivision = DiffusionDivisionNumber();
      const double edepDivision = edep/numDivision;
      const double xEChargeDivision = xECharge/numDivision;
      const double yEChargeDivision = yECharge/numDivision;
      
      double xDiffusionSigma = 0.0;
      double yDiffusionSigma = 0.0;
      if (isXStripSideAnode()) {
        xDiffusionSigma = DiffusionSigmaAnode(localposz);
        yDiffusionSigma = DiffusionSigmaCathode(localposz);
      }
      else if (isXStripSideCathode()) {
        xDiffusionSigma = DiffusionSigmaCathode(localposz);
        yDiffusionSigma = DiffusionSigmaAnode(localposz);
      }

      { // x-strip
        std::vector<DetectorHit_sptr> diffusionHits;
        for (int l=0; l<numDivision; l++) {
          const double dx = gRandom->Gaus(0.0, xDiffusionSigma);
          const double dy = gRandom->Gaus(0.0, xDiffusionSigma);
          PixelID spDiff = findPixel(localposx+dx, localposy+dy);
          spDiff.setY(PixelID::Undefined);
          
          auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                               [&](const DetectorHit_sptr& hit)-> bool {
                                 return (hit->Pixel()==spDiff);
                               });
          if (itHit != diffusionHits.end()) {
            DetectorHit_sptr hit = *itHit;
            hit->setEnergyDeposit( hit->EnergyDeposit() + edepDivision );
            hit->setEnergyCharge( hit->EnergyCharge() + xEChargeDivision );
          }
          else {
            DetectorHit_sptr hitDivision(new DetectorHit(*xhit));
            hitDivision->setEnergyDeposit(edepDivision);
            hitDivision->setEnergyCharge(xEChargeDivision);
            hitDivision->setPixel(spDiff);
            diffusionHits.push_back(hitDivision);
          }
        }
        
        for (auto& hit: diffusionHits) {
          insertSimulatedHit(hit);
        }
      }

      { // y-strip
        std::vector<DetectorHit_sptr> diffusionHits;
        for (int l=0; l<numDivision; l++) {
          const double dx = gRandom->Gaus(0.0, yDiffusionSigma);
          const double dy = gRandom->Gaus(0.0, yDiffusionSigma);
          PixelID spDiff = findPixel(localposx+dx, localposy+dy);
          spDiff.setX(PixelID::Undefined);
          
          auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                               [&](const DetectorHit_sptr& hit)-> bool {
                                 return (hit->Pixel()==spDiff);
                               });
          if (itHit != diffusionHits.end()) {
            DetectorHit_sptr hit = *itHit;
            (*itHit)->setEnergyDeposit( hit->EnergyDeposit() + edepDivision );
            (*itHit)->setEnergyCharge( hit->EnergyCharge() + yEChargeDivision );
          }
          else {
            DetectorHit_sptr hitDivision(new DetectorHit(*yhit));
            hitDivision->setEnergyDeposit(edepDivision);
            hitDivision->setEnergyCharge(yEChargeDivision);
            hitDivision->setPixel(spDiff);
            diffusionHits.push_back(hitDivision);
          }
        }

        for (auto& hit: diffusionHits) {
          insertSimulatedHit(hit);
        }
      }
    }
      
    // near strips
    if (ChargeCollectionMode()==3) {
      DetectorHit_sptr hit;
      PixelID nearStrip;
     
      nearStrip.set(sp.X()+2, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()+1, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()-1, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()-2, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
    
      nearStrip.set(PixelID::Undefined, sp.Y()+2);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(PixelID::Undefined, sp.Y()+1);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(PixelID::Undefined, sp.Y()-1);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(PixelID::Undefined, sp.Y()-2);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);   
    }
    else if (ChargeCollectionMode()>=4) {
      const int strip_range = 7;

      for (int delta_strip=-strip_range; delta_strip<=strip_range; delta_strip++) {
        if (delta_strip!=0) {
          PixelID nearStrip(sp.X()+delta_strip, PixelID::Undefined);
          DetectorHit_sptr hit = generateHit(*rawhit, nearStrip);
          hit->setEnergyDeposit(0.0);
          insertSimulatedHit(hit);
        }
      }

      for (int delta_strip=-strip_range; delta_strip<=strip_range; delta_strip++) {
        if (delta_strip!=0) {
          PixelID nearStrip(PixelID::Undefined, sp.Y()+delta_strip);
          DetectorHit_sptr hit = generateHit(*rawhit, nearStrip);
          hit->setEnergyDeposit(0.0);
          insertSimulatedHit(hit);
        }
      }
    }
  }
}

DetectorHit_sptr SimDetectorUnit2DStrip::generateHit(const DetectorHit& rawhit,
                                                     const PixelID& sp)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  if (sp.isXStrip()) {
    if (isXStripSideAnode()) {
      hit->addFlags(flag::AnodeSide);
      hit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
    }
    else if (isXStripSideCathode()) {
      hit->addFlags(flag::CathodeSide);
      hit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
    }
  }
  else if (sp.isYStrip()) {
    if (isYStripSideAnode()) {
      hit->addFlags(flag::AnodeSide);
      hit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
    }
    else if (isYStripSideCathode()) {
      hit->addFlags(flag::CathodeSide);
      hit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
    }
  }
  
  if (checkRange(sp)) {
    hit->setPixel(sp);
  }
  else {
    hit->setPixel(PixelID::OnGuardRing, PixelID::OnGuardRing);
    return hit;
  }

  applyQuenching(hit);

  const double edep = hit->EnergyDeposit();
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();
  const double energyCharge = calculateEnergyCharge(sp,
                                                    edep,
                                                    localposx,
                                                    localposy,
                                                    localposz);
  hit->setEnergyCharge(energyCharge);
  return hit;
}

double SimDetectorUnit2DStrip::
ChargeCollectionEfficiency(const PixelID& sp,
                           const double x, const double y, const double z) const
{
  const vector3_t pixelCenter = LocalPosition(sp);
  
  double cce = 1.0;
  if (sp.isXStrip()) {
    const double xInPixel = x - pixelCenter.x();
    const int ix = CCEMapXStrip_->GetXaxis()->FindBin(xInPixel/unit::cm);
    const int iz = CCEMapXStrip_->GetYaxis()->FindBin(z/unit::cm);
    cce =  CCEMapXStrip_->GetBinContent(ix, iz);
  }
  else if (sp.isYStrip()) {
    const double yInPixel = y - pixelCenter.y();
    const int iy = CCEMapYStrip_->Get