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

#include "AllSkyPrimaryGen.hh"

#include <cmath>
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;


namespace comptonsoft
{

AllSkyPrimaryGen::AllSkyPrimaryGen()
  : filename_("allsky.fits"),
    hdu_index_map_(2),
    hdu_index_energy_(3),
    origin_longitude_(0.0),
    origin_latitude_(0.0),
    pole_longitude_(0.0),
    pole_latitude_(90.0*unit::degree)
{
  add_alias("AllSkyPrimaryGen");
}

AllSkyPrimaryGen::~AllSkyPrimaryGen() = default;

ANLStatus AllSkyPrimaryGen::mod_define()
{
  ANLStatus status = IsotropicPrimaryGen::mod_define();
  if (status != AS_OK) {
    return status;
  }
 
  define_parameter("filename", &mod_class::filename_);
  define_parameter("hdu_index_map", &mod_class::hdu_index_map_);
  define_parameter("hdu_index_energy", &mod_class::hdu_index_energy_);
  define_parameter("origin_longitude_", &mod