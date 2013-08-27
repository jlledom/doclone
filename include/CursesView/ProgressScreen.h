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

#ifndef PROGRESSSCREEN_H_
#define PROGRESSSCREEN_H_

#include <pthread.h>

#include <string>
#include <vector>

#include <doclone/observer/AbstractObserver.h>

#include <CursesView/BaseScreen.h>
#include <CursesView/ActionPanel.h>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class ProgressScreen
 *
 * This is the final screen of the program. In this screen, the user can view
 * the progress of the execution, the list of errors and events, the list of
 * pending or completed operations and a progress bar with the percentage of
 * data transferred.
 *
 * It is an specialization of Doclone::AbstractObserver, so it will receive all
 * the events and errors of libdoclone.
 *
 * \date October, 2011
 */
class ProgressScreen : public BaseScreen, public Doclone::AbstractObserver {
public:
	ProgressScreen();

	int run();

	void notify(Doclone::dcTransferEvent event, uint64_t numBytes);
	void notify(Doclone::dcOperationEvent event,
			Doclone::dcOperationType type, const std::string &target);
	void notify(Doclone::dcEvent event, const std::string &target);
	void notify(const std::string &str);

	~ProgressScreen();

protected:
	void readKeyboard();
	int getNextScreen();

	void showCloseButton();

	/// This window simulates an empty progress bar
	WINDOW *_progressBar;

	/// The panel with the list of pending/completed operations
	ActionPanel *_actionsPanel;
	/// The menu with the list of event and errors
	Menu *_eventsMenu;

	/// Private list with the messages of the actions
	std::vector<std::string> _actions;
	/// Private list with the messages of the events
	std::vector<std::string> _events;

	/// The total amount of bytes to be transferred
	uint64_t _totalSize;
	/// Current percentage of data transferred
	int _percent;
	/// Width in columns of the progress bar window
	int _barWidth;

	/// X position of the label that shows the percentage
	int _percentTextPosX;

	/// The functions of libdoclone will run in a thread.
	pthread_t _thread;
};
/**@}*/

void *executedoclone(void *ptr);

#endif /* PROGRESSSCREEN_H_ */
