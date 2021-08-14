/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

#ifndef ANLGEANT4_UserActionAssemblySteppingAction_H
#define ANLGEANT4_UserActionAssemblySteppingAction_H 1

#include "G4UserSteppingAction.hh"
#include <list>

namespace anlgeant4
{

class VUserActionAssembly;

/**
 * User SteppingAction class for UserActionAssembly
 * @author M. Kouda, S. Watanabe, H. Odaka
 * @date 2003-01-10 (modified: S. Watanabe)
 * @date 2012-05-30 | Hirokazu Odaka | new design
 * @date 2017-07-29 | Hirokazu Odaka | rename class, introduce user action list
 */
class UserActionAssemblySteppingAction : public G4UserSteppingAction
{
public:
  explicit UserActionAssemblySteppingAction(const std::list<VUserActionAssembly*>& userActions);
  ~UserActionAssemblySteppingAction();

  void UserSteppingAction(const G4Step*) override;

private:
  std::list<VUserActionAssembly*> userActions_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_UserActionAssemblySteppingAction_H */
