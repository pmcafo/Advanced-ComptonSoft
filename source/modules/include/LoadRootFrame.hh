
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

#ifndef COMPTONSOFT_LoadRootFrame_H
#define COMPTONSOFT_LoadRootFrame_H 1

#include <cstdint>
#include <boost/multi_array.hpp>
#include <TChain.h>
#include <anlnext/BasicModule.hh>
#include "FrameData.hh"


namespace comptonsoft {

class FrameData;
using raw_image_t = boost::multi_array<uint16_t, 2>;


/**
 * LoadRootFrame
 *
 * @author Tsubasa Tamba
 * @date 2019-07-22
 * @date 2020-04-01 | Hirokazu Odaka | upgrade for new ConstrcutFrame
 */
class LoadRootFrame : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(LoadRootFrame, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  LoadRootFrame();
  
protected:
  LoadRootFrame(const LoadRootFrame&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  int detector_id_ = 0;
  std::vector<std::string> files_;
  std::string treename_;
  std::string branchname_;
  size_t numEntries_ = 0;
  TChain* frametree_;

  FrameData* frame_ = nullptr;
  raw_image_t rawPH_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadRootFrame_H */