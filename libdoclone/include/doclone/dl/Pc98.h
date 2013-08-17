/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PC98_H_
#define PC98_H_

#include <doclone/DiskLabel.h>

namespace Doclone {

/**
 * \class Pc98
 * \brief Pc98 disk label
 *
 * \date June, 2012
 */
class Pc98 : public DiskLabel {
public:
	Pc98(const std::string &path);
};

} /* namespace Doclone */
#endif /* PC98_H_ */
