// $Id$
//
// Copyright (C) 2008, 2009  Rafael Ostertag
//
// This file is part of YAPET.
//
// YAPET is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// YAPET is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// YAPET.  If not, see <http://www.gnu.org/licenses/>.
//

#ifdef HAVE_CONFIG
# include <config.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#include <structs.h>
#include <partdec.h>
#include <crypt.h>

#include "../intl.h"
#include "colors.h"
#include "messagebox.h"
#include "dialogbox.h"
#include "passwordrecord.h"

void
PasswordRecord::createWindow() throw (YAPET::UI::UIException) {
    if (window != NULL)
        throw YAPET::UI::UIException (_ ("May you consider deleting the window before reallocating") );

    window = newwin (getHeight(), getWidth(), getStartY(), getStartX() );

    if (window == NULL)
        throw YAPET::UI::UIException (_ ("Error creating password entry") );

    name = new YAPET::UI::InputWidget (getStartX() + 1,
                                       getStartY() + 2,
                                       getWidth() - 2,
                                       YAPET::NAME_SIZE);
    host = new YAPET::UI::InputWidget (getStartX() + 1,
                                       getStartY() + 4,
                                       getWidth() - 2,
                                       YAPET::HOST_SIZE);
    username = new YAPET::UI::InputWidget (getStartX() + 1,
                                           getStartY() + 6,
                                           getWidth() - 2,
                                           YAPET::USERNAME_SIZE);
    password = new YAPET::UI::InputWidget (getStartX() + 1,
                                           getStartY() + 8,
                                           getWidth() - 2,
                                           YAPET::PASSWORD_SIZE);
    comment = new YAPET::UI::InputWidget (getStartX() + 1,
                                          getStartY() + 10,
                                          getWidth() - 2,
                                          YAPET::COMMENT_SIZE);
    okbutton = new YAPET::UI::Button (_ ("OK"),
                                      getStartX() + 1,
                                      getStartY() + 12);
    cancelbutton = new YAPET::UI::Button (_ ("Cancel"),
                                          getStartX() + okbutton->getLength() + 2,
                                          getStartY() + 12);
#ifdef ENABLE_PWGEN
    pwgenbutton = new YAPET::UI::Button (_ ("Generate Password"),
                                         getStartX() + okbutton->getLength() + cancelbutton->getLength() + 3,
                                         getStartY() + 12);
#endif
    refresh();
}

/**
 * Ask the user whether or not he want to cancel, but only if fields have been changed.
 *
 */
bool
PasswordRecord::sureToCancel() throw (YAPET::UI::UIException) {
    bool suretoexit = true;

    if (entryChanged() ) {
        YAPET::UI::DialogBox* question = NULL;

        try {
            question = new YAPET::UI::DialogBox (_ ("Q U E S T I O N"),
                                                 _ ("Entries modified. Really cancel?") );
            question->run();
            YAPET::UI::ANSWER a = question->getAnswer();
            suretoexit = a == YAPET::UI::ANSWER_OK ? true : false;
            delete question;
        } catch (YAPET::UI::UIException&) {
            if (question != NULL)
                delete question;
        }

        refresh();
    }

    if (suretoexit) {
        encentry = NULL;
        return true;
    } else {
        return false;
    }
}

PasswordRecord::PasswordRecord (YAPET::Key& k, YAPET::PartDec* pe)
throw (YAPET::UI::UIException) : window (NULL),
        name (NULL),
        host (NULL),
        username (NULL),
        password (NULL),
        comment (NULL),
        okbutton (NULL),
        cancelbutton (NULL),
#ifdef ENABLE_PWGEN
        pwgenbutton (NULL),
#endif
        key (&k),
        encentry (pe),
        s_name (""),
        s_host (""),
        s_username (""),
        s_password (""),
        s_comment (""),
        namechanged (false),
        hostchanged (false),
        usernamechanged (false),
        passwordchanged (false),
        commentchanged (false) {
    if (encentry != NULL) {
        YAPET::Record<YAPET::PasswordRecord>* dec_rec = NULL;

        try {
            YAPET::Crypt crypt (*key);
            dec_rec = crypt.decrypt<YAPET::PasswordRecord> (encentry->getEncRecord() );
            YAPET::PasswordRecord* ptr_rec = *dec_rec;
            s_name = (char*) ptr_rec->name;
            s_host = (char*) ptr_rec->host;
            s_username = (char*) ptr_rec->username;
            s_password = (char*) ptr_rec->password;
            s_comment = (char*) ptr_rec->comment;
            delete dec_rec;
        } catch (YAPET::YAPETException& ex) {
            if (dec_rec != NULL)
                delete dec_rec;

            YAPET::UI::MessageBox* msgbox = NULL;

            try {
                msgbox = new YAPET::UI::MessageBox (_ ("E R R O R"), ex.what() );
                msgbox->run();
                delete msgbox;
            } catch (YAPET::UI::UIException&) {
                if (msgbox != NULL)
                    delete msgbox;

                // What should I do else, looks pretty screwed up??
            }
        }
    }

    createWindow();
}

PasswordRecord::~PasswordRecord() {
    wclear (window);
    delwin (window);
    delete name;
    delete host;
    delete username;
    delete password;
    delete comment;
    delete okbutton;
    delete cancelbutton;
#ifdef ENABLE_PWGEN
    delete pwgenbutton;
#endif
}

void
PasswordRecord::run() throw (YAPET::UI::UIException) {
    while (true) {
        int ch = 0;
#ifdef HAVE_WRESIZE

        while ( (ch = name->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = name->focus();
#endif // HAVE_WRESIZE
        s_name = name->getText();
        namechanged = name->isTextChanged();

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

#ifdef HAVE_WRESIZE

        while ( (ch = host->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = host->focus();
#endif // HAVE_WRESIZE
        s_host = host->getText();
        hostchanged = host->isTextChanged();

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

#ifdef HAVE_WRESIZE

        while ( (ch = username->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = username->focus();
#endif // HAVE_WRESIZE
        s_username = username->getText();
        usernamechanged = username->isTextChanged();

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

#ifdef HAVE_WRESIZE

        while ( (ch = password->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = password->focus();
#endif // HAVE_WRESIZE
        s_password = password->getText();
        passwordchanged = password->isTextChanged();

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

#ifdef HAVE_WRESIZE

        while ( (ch = comment->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = comment->focus();
#endif // HAVE_WRESIZE
        s_comment = comment->getText();
        commentchanged = comment->isTextChanged();

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

#ifdef HAVE_WRESIZE

        while ( (ch = okbutton->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = okbutton->focus();
#endif // HAVE_WRESIZE

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

        if (ch == '\n') {
            if (!entryChanged() ) {
                encentry = NULL;
                return;
            }

            YAPET::Record<YAPET::PasswordRecord> unenc_rec;
            YAPET::PasswordRecord* ptr_rec = unenc_rec;
            strncpy ( (char*) ptr_rec->name, s_name.c_str(), YAPET::NAME_SIZE);
            strncpy ( (char*) ptr_rec->host, s_host.c_str(), YAPET::HOST_SIZE);
            strncpy ( (char*) ptr_rec->username, s_username.c_str(), YAPET::USERNAME_SIZE);
            strncpy ( (char*) ptr_rec->password, s_password.c_str(), YAPET::PASSWORD_SIZE);
            strncpy ( (char*) ptr_rec->comment, s_comment.c_str(), YAPET::COMMENT_SIZE);
            YAPET::BDBuffer* enc_rec = NULL;

            try {
                encentry = new YAPET::PartDec (unenc_rec, *key);
            } catch (YAPET::YAPETException& ex) {
                if (enc_rec != NULL)
                    delete enc_rec;

                encentry = NULL;
                YAPET::UI::MessageBox* msgbox = NULL;

                try {
                    msgbox = new YAPET::UI::MessageBox (_ ("E R R O R"), ex.what() );
                    msgbox->run();
                    delete msgbox;
                } catch (YAPET::UI::UIException&) {
                    if (msgbox != NULL)
                        delete msgbox;

                    // What should I do else, looks pretty screwed up??
                }
            }

            return;
        }

#ifdef HAVE_WRESIZE

        while ( (ch = cancelbutton->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#else // HAVE_WRESIZE
        ch = cancelbutton->focus();
#endif // HAVE_WRESIZE

        if ( (ch == '\n' || ch == KEY_ESC) &&
                sureToCancel() ) {
            return;
        }

#ifdef ENABLE_PWGEN
#  ifdef HAVE_WRESIZE

        while ( (ch = pwgenbutton->focus() ) == KEY_RESIZE)
            YAPET::UI::BaseWindow::resizeAll();

#  else // HAVE_WRESIZE
        ch = pwgenbutton->focus();
#  endif // HAVE_WRESIZE

        if (ch == '\n' ) {
            PWGenDialog* tmp = NULL;

            try {
                tmp = new PWGenDialog ();
                tmp->run();

                if (!tmp->isCanceled() ) {
                    s_password = tmp->getPassword();
                    passwordchanged = true;
                    password->setText (s_password);
                    password->setTextChanged (true);
                }

                delete tmp;
            } catch (std::exception& ex2) {
                if (tmp != NULL)
                    delete tmp;
            }

            YAPET::UI::BaseWindow::refreshAll();
        }

        if (ch == KEY_ESC && sureToCancel() ) {
            return;
        }

#endif // ENABLE_PWGEN
    } // while (true)
}

void
PasswordRecord::resize() throw (YAPET::UI::UIException) {
    int retval = delwin (window);

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error deleting password entry window") );

    delete name;
    delete host;
    delete username;
    delete password;
    delete comment;
    delete okbutton;
    delete cancelbutton;
#ifdef ENABLE_PWGEN
    delete pwgenbutton;
#endif
    window = NULL;
    name = NULL;
    username = NULL;
    password = NULL;
    comment = NULL;
    okbutton = NULL;
    cancelbutton = NULL;
#ifdef ENABLE_PWGEN
    pwgenbutton = NULL;
#endif
    createWindow();
}

void
PasswordRecord::refresh() throw (YAPET::UI::UIException) {
    YAPET::UI::Colors::setcolor (window, YAPET::UI::MESSAGEBOX);
    int retval = wclear (window);

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error clearing window") );

    retval = box (window, 0, 0);

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error adding box") );

    retval = mymvwaddstr (window, 0, 2, _ ("P A S S W O R D  R E C O R D") );

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error setting label") );

    retval = mymvwaddstr (window, 1, 1, _ ("Name") );

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error adding label") );

    retval = mymvwaddstr (window, 3, 1, _ ("Host") );

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error adding label") );

    retval = mymvwaddstr (window, 5, 1, _ ("Username") );

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error adding label") );

    retval = mymvwaddstr (window, 7, 1, _ ("Password") );

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error adding label") );

    retval = mymvwaddstr (window, 9, 1, _ ("Comment") );

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error adding label") );

    retval = wrefresh (window);

    if (retval == ERR)
        throw YAPET::UI::UIException (_ ("Error refreshing window") );

    name->setText (s_name);
    name->setTextChanged (namechanged);
    name->refresh();
    host->setText (s_host);
    host->setTextChanged (hostchanged);
    host->refresh();
    username->setText (s_username);
    username->setTextChanged (usernamechanged);
    username->refresh();
    password->setText (s_password);
    password->setTextChanged (passwordchanged);
    password->refresh();
    comment->setText (s_comment);
    comment->setTextChanged (commentchanged);
    comment->refresh();
    okbutton->refresh();
    cancelbutton->refresh();
#ifdef ENABLE_PWGEN
    pwgenbutton->refresh();
#endif
}

bool
PasswordRecord::entryChanged() const {
    return name->isTextChanged() ||
           host->isTextChanged() ||
           username->isTextChanged() ||
           password->isTextChanged() ||
           comment->isTextChanged();
}
