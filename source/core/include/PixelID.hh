
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

#ifndef COMPTONSOFT_PixelID_H
#define COMPTONSOFT_PixelID_H 1

#include "VoxelID.hh"

namespace comptonsoft {

/**
 * Class of a pixel or a pair of strips.
 *
 * @author Hirokazu Odaka
 * @date 2014-11-11
 * @date 2022-04-24 | become an alias of VoxelID
 */

using PixelID = VoxelID;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PixelID_H */