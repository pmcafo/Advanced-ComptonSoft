
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

#ifndef COMPTONSOFT_ReadEventTree_H
#define COMPTONSOFT_ReadEventTree_H 1

#include "VCSModule.hh"
#include "InitialInformation.hh"

#include <vector>
#include <string>
#include <cstdint>
#include "DetectorHit_sptr.hh"

class TChain;

namespace comptonsoft {

class CSHitCollection;
class EventTreeIOWithInitialInfo;

/**
 * @author Hitokazu Odaka
 * @date 2015-11-14
 * @date 2019-04-22 | initialization in mod_begin_run()
 */
class ReadEventTree : public VCSModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadEventTree, 2.1);
public:
  ReadEventTree();
  ~ReadEventTree();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;

  int64_t NumEntries() const { return numEntries_; }

protected:
  virtual void insertHit(const DetectorHit_sptr& hit);
  
private:
  std::vector<std::string> fileList_;

  TChain* tree_;
  int64_t numEntries_ = 0;
  int64_t entryIndex_ = 0;

  CSHitCollection* hitCollection_;
  std::unique_ptr<EventTreeIOWithInitialInfo> treeIO_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadEventTree_H */