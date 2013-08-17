/*
 * doclone - a frontend for libdoclone
 * Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACTIONPANEL_H_
#define ACTIONPANEL_H_

#include <doclone/Operation.h>

#include <CursesView/Menu.h>

#include <string>
#include <vector>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class ActionPanel
 *
 * An ActionPanel is a Menu that contains a set of operations to be performed
 * and that can be marked as completed. For simulate this, the background of
 * completed operations is re-colored.
 *
 * \date October, 2011
 */
class ActionPanel : public Menu {
public:
	ActionPanel();

	void addOperation(Doclone::Operation &operation);
	void markOperationCompleted(Doclone::Operation &operation);
	void deleteOperation(Doclone::Operation &operation);
	void clearAll();

	void post();

	bool allOperationsCompleted();

    const std::vector<Doclone::Operation> &getOperations() const;
protected:
    void refresh();

    void setContent(const std::vector<std::string> &labels);

    /// Vector with the list of operations, this class doesn't use the descriptions
	std::vector<Doclone::Operation> _operations;
};
/**@}*/

#endif /* ACTIONPANEL_H_ */
