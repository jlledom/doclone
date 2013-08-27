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

#include <CursesView/ProgressScreen.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>

#include <sstream>
#include <algorithm>

#include <doclone/Clone.h>

#include <CursesView/CursesView.h>
#include <CursesView/ScreenFactory.h>
#include <doclone/DataTransfer.h>

ProgressScreen::ProgressScreen() : _actions(), _events(), _totalSize(),
	_percent(), _percentTextPosX(), _thread() {
	// Refresh the text at the top bar
	this->_topBarNotice=_("Working...");

	std::string aRemain=_("Actions remaining:");

	mvwprintw(this->_win, 2, 2, aRemain.c_str());

	std::string events=_("Alerts & events:");

	int menusWidth=(BaseScreen::WIN_WIDTH-6)/2;
	mvwprintw(this->_win, 2, menusWidth+4, events.c_str());

	this->_actionsPanel = new ActionPanel();
	this->_actionsPanel->setWindow(this->_win);
	this->_actionsPanel->setColors(
				COLOR_PAIR(CursesView::MENU_BKGD),
				COLOR_PAIR(CursesView::SELECTED_OPTION),
				COLOR_PAIR(CursesView::PANEL_UNSELECTABLE_OPTION));
	this->_actionsPanel->setMargin(false);
	this->_actionsPanel->setHeight(9);
	this->_actionsPanel->setWidth(menusWidth);
	this->_actionsPanel->setPosX(2);
	this->_actionsPanel->setPosY(3);
	this->_actionsPanel->post();

	this->_eventsMenu = new Menu();
	this->_eventsMenu->setWindow(this->_win);
	this->_eventsMenu->setColors(
				COLOR_PAIR(CursesView::MENU_BKGD),
				COLOR_PAIR(CursesView::SELECTED_OPTION),
				COLOR_PAIR(CursesView::UNSELECTABLE_OPTION));
	this->_eventsMenu->setMargin(false);
	this->_eventsMenu->setHeight(9);
	this->_eventsMenu->setWidth(menusWidth);
	this->_eventsMenu->setPosX(menusWidth+4);
	this->_eventsMenu->setPosY(3);
	this->_eventsMenu->post();

	std::string tTrans=_("Transferred:");

	mvwprintw(this->_win, 14, 2, tTrans.c_str());
	this->_percentTextPosX = tTrans.length()+3;

	this->_barWidth = BaseScreen::WIN_WIDTH-4;
	this->_progressBar = derwin(this->_win, 2, this->_barWidth, 15, 2);
	wbkgd(this->_progressBar, COLOR_PAIR(CursesView::MENU_BKGD));
	wrefresh(this->_progressBar);

	this->_menuCancel->post();

	this->_listWidgets.push_back(this->_menuCancel);
	this->_listWidgets.push_back(this->_eventsMenu);

	this->_it = this->_listWidgets.begin();

	wrefresh(this->_win);
}

/**
 * This class overrides this function because it needs to call executeGenetic()
 * before listening the user events.
 *
 * \return The code of the next screen to be showed. In this case 0 (None)
 */
int ProgressScreen::run() {
	(*this->_it)->focus();
	this->show();

	// Listen the messages from the library
	Doclone::Clone *dcl = Doclone::Clone::getInstance();
	Doclone::DataTransfer *trans = Doclone::DataTransfer::getInstance();
	Doclone::Logger *log = Doclone::Logger::getInstance();
	trans->addObserver(this);
	dcl->addObserver(this);
	log->addObserver(this);

	pthread_create(&this->_thread, 0, executedoclone, 0);

	this->readKeyboard();

	int retVal = this->getNextScreen();

	return retVal;
}

/**
 * Reads the state of the CursesView object, determines what work should be
 * performed and calls the corresponding function.
 *
 * This function is called into a thread.
 */
void *executedoclone(void *ptr) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);

	// Temporarily redirect the error output
	int bak, newOutput;
	fflush(stderr);
	bak = dup(2);
	newOutput = open("/dev/null", O_WRONLY);
	dup2(newOutput, 2);
	close(newOutput);

	CursesView *curView = CursesView::getInstance();
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	dcCursesMode mode = curView->getMode();
	dcCursesRole role = curView->getRole();

	try {
		switch(mode) {
		case CURSES_MODE_LOCAL: {
			switch(role) {
			case CURSES_ROLE_READ: {
				dcl->create();
				break;
			}
			case CURSES_ROLE_WRITE: {
				dcl->restore();
				break;
			}
			default: {
				break;
			}
			}

			break;
		}
		case CURSES_MODE_UNICAST: {
			switch(role) {
			case CURSES_ROLE_READ: {
				dcl->send();
				break;
			}
			case CURSES_ROLE_WRITE: {
				dcl->receive();
				break;
			}
			default: {
				break;
			}
			}

			break;
		}
		case CURSES_MODE_LINK: {
			switch(role) {
			case CURSES_ROLE_READ: {
				dcl->chainOrigin();
				break;
			}
			case CURSES_ROLE_WRITE: {
				dcl->chainLink();
				break;
			}
			default: {
				break;
			}
			}

			break;
		}
		default: {
			break;
		}
		}
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
	}

	// Reset stderr
	fflush(stderr);
	dup2(bak, 2);
	close(bak);

	return 0;
}

/**
 * Receives the transfer events and draws the progress bar appropriately
 *
 * \param event
 * 		The event triggered, for this function can be TRANS_TOTAL_SIZE
 * 		or TRANS_TRANSFERRED_BYTES
 * \param numBytes
 * 		The total/transferred amount of bytes
 */
void ProgressScreen::notify(Doclone::dcTransferEvent event, uint64_t numBytes) {
	switch (event) {
	case Doclone::TRANS_TOTAL_SIZE: {
		this->_totalSize = numBytes;
		break;
	}
	case Doclone::TRANS_TRANSFERRED_BYTES: {
		if(this->_totalSize>0) {
			float rate = numBytes/static_cast<float>(this->_totalSize);
			int percent = floor(rate*100);
			if(this->_percent != percent) {
				this->_percent = percent;
				int filledCols=(this->_percent*this->_barWidth)/100;

				if(filledCols>0) {
					WINDOW *tmpWin = derwin(this->_win, 2, filledCols, 15, 2);
					wbkgd(tmpWin, COLOR_PAIR(CursesView::PROGRESS_BAR));
					wrefresh(tmpWin);

					std::stringstream prcStr;
					prcStr << this->_percent << "%%";

					mvwprintw(this->_win, 14, this->_percentTextPosX,
							prcStr.str().c_str());
					wrefresh(this->_win);

					delwin(tmpWin);
				}
			}
		}

		break;
	}
	}
}

/**
 * Receives the operation events and refreshes the action panel
 *
 * \param event
 * 		The event triggered, in this function can be OPER_ADD or
 * 		OPER_MARK_COMPLETED
 * \param type
 * 		The type of operation triggered. See Doclone::dcOperationEvent for
 * 		details
 * \param target
 * 		The target on which the event happened
 */
void ProgressScreen::notify(Doclone::dcOperationEvent event,
		Doclone::dcOperationType type, const std::string &target) {

	Doclone::Operation oper(type, target);

	if(event == Doclone::OPER_MARK_COMPLETED) {
		this->_actionsPanel->markOperationCompleted(oper);
	} else {
		this->_actionsPanel->addOperation(oper);
	}

	this->_actionsPanel->post();
	wrefresh(this->_win);
}

/**
 * Receives the general events and performs the appropriate actions.
 *
 * \param event
 * 		The event triggered, see Doclone::dcEvent for details
 * \param target
 * 		The target on which the event happened
 */
void ProgressScreen::notify(Doclone::dcEvent event, const std::string &target) {
	if(event == Doclone::EVT_CANCEL_EXECUTION) {
		/*
		 * Only show the <Close> button, since the thread already has finished
		 * by some error
		 */
		this->showCloseButton();
	} else if(event == Doclone::EVT_FINISH_EXECUTION){
		// Close the thread
		pthread_join(this->_thread, 0);

		// Set the progress bas at 100%
		Doclone::dcTransferEvent type = Doclone::TRANS_TRANSFERRED_BYTES;
		this->notify(type, this->_totalSize);

		// And then, show the <Close> button
		this->showCloseButton();
	} else if(event == Doclone::EVT_NEW_CONNECION) {
		// TO TRANSLATORS: looks like	New connection from 192.168.1.10
		std::string msg=_("New connection from");
		msg.append(" ");
		msg.append(target);
		this->_events.push_back(msg);

		this->_eventsMenu->setContent(this->_events, this->_events);
		this->_eventsMenu->post();
		wrefresh(this->_win);
	}

	return;
}

/**
 * When an exception has occurred, its message is raised up here. This function
 * just adds it to the events panel
 *
 * \param str
 * 		The exception message
 */
void ProgressScreen::notify(const std::string &str) {
	this->_events.push_back(str);

	this->_eventsMenu->setContent(this->_events, this->_events);
	this->_eventsMenu->post();
	wrefresh(this->_win);
}

/**
 * Reads the state of the widgets and determine what will be the next screen
 *
 * \return The code of the next screen.
 * For this case, 0 (None, this is the last screen)
 */
int ProgressScreen::getNextScreen() {
	pthread_join(this->_thread, 0);

	return ScreenFactory::NONE;
}

/**
 * This class overrides this function because the BaseScreen method goes to the
 * next screen where the ENTER key is pressed, but this screen is the last one.
 * So, it closes the program only if the <Close> button is pressed, and cancels
 * execution when the <Cancel> button is pressed.
 */
void ProgressScreen::readKeyboard() {
	bool readKeyboard = true;

	do {
		int c = getch();

		switch(c) {
		case KEY_ENTER:
		case '\n':
		case '\r': {
			if((*this->_it) == this->_menuClose) {
				readKeyboard = false;
			}

			if((*this->_it) == this->_menuCancel) {
				pthread_kill(this->_thread, SIGINT);
				pthread_join(this->_thread, 0);
			}

			break;
		}
		case '\t': {

			(*this->_it)->blur();

			this->_it++;

			if(this->_it == this->_listWidgets.end()) {
				this->_it = this->_listWidgets.begin();
			}

			(*this->_it)->focus();

			break;
		}
		default: {
			(*this->_it)->doAction(c);

			break;
		}
		}

		update_panels();
		doupdate();
	} while(readKeyboard);
}

/**
 * Shows the <Close> button
 */
void ProgressScreen::showCloseButton() {
	this->_menuCancel->blur();
	this->_menuCancel->unpost();

	this->_it = std::find(this->_listWidgets.begin(), this->_listWidgets.end(),
			this->_menuCancel);

	this->_listWidgets.erase(this->_it);
	this->_listWidgets.push_back(this->_menuClose);

	this->_it = std::find(this->_listWidgets.begin(), this->_listWidgets.end(),
				this->_menuClose);

	this->_menuClose->post();
	this->_menuClose->focus();

	wrefresh(this->_win);
}

ProgressScreen::~ProgressScreen() {
	delete this->_actionsPanel;
	delete this->_eventsMenu;

	wclear(this->_progressBar);
	delwin(this->_progressBar);
}
