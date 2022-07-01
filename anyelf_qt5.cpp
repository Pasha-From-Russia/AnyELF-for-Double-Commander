/*
Copyright (C) 2022 by Pasha-From-Russia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "anyelf.h"
#include <elfio/elfio_version.hpp>
#include <QString>
#include <QTextEdit>
#include <QFont>
#include <QTextCursor>
#include <QtCore/QtCore>
#include <QMessageBox>
#include <QTextStream>


static const QString g_fontname      = "monospace";
static const int     g_fontsize      = 11;
static const QString ANYELF_VERSION  = "1.0";

/* API */
//---------------------------------------------------------------------------
int DCPCALL ListSearchText(
        HWND  t_listWin,
        char *t_searchString,
        int   t_searchParameter)
{
    QTextEdit *view = static_cast<QTextEdit*>(t_listWin);
    if (!view || !t_searchString) {
        return LISTPLUGIN_ERROR;
    }
    bool backwardSearch = false;

    QTextDocument::FindFlags flags;
    if (t_searchParameter & lcs_matchcase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (t_searchParameter & lcs_backwards) {
        flags |= QTextDocument::FindBackward;
        backwardSearch = true;
    }

    QTextCursor cursor = view->document()->find(t_searchString, view->textCursor(), flags);
    if (cursor.isNull()) {
        QString text;
        QTextStream(&text) \
                << "\"" \
                <<  t_searchString \
                << "\"" \
                " not found!\nTry to search from the " \
                << (backwardSearch ? "end" : "beginning") << "?";
        int ret = QMessageBox::question(view, "AnyElf", text, QMessageBox::Yes | QMessageBox::No);
        if (ret != QMessageBox::Yes) {
            return LISTPLUGIN_OK;
        }
        cursor = view->textCursor();
        cursor.movePosition(backwardSearch ? QTextCursor::End : QTextCursor::Start);
        cursor = view->document()->find(t_searchString, cursor, flags);
    }
    if (!cursor.isNull()) {
        view->setTextCursor(cursor);
    } else {
        QString text;
        QTextStream(&text) << "\"" <<  t_searchString << "\" not found!";
        QMessageBox::information(view, "AnyElf", text);
    }

    return LISTPLUGIN_OK;
}

//---------------------------------------------------------------------------
HWND DCPCALL ListLoad(
        HWND  t_parentWin,
        char *t_fileToLoad,
        int   t_showFlags)
{
    if (!isElfFile(t_fileToLoad)) {
        return NULL;
    }

    QString text = elfdump(t_fileToLoad).c_str();
    if (text.isEmpty()) {
        return NULL;
    }

    QString tail;
    QTextStream(&tail) \
            << "\n\nELFIO " << ELFIO_VERSION << " by Serge Lamikhov-Center\n" \
            << "AnyELF-Qt5 plugin v" << ANYELF_VERSION \
            << " for DoubleCommander by Pasha-From-Russia";

    QWidget *parent = static_cast<QWidget*>(t_parentWin);
    QTextEdit *view = new QTextEdit(parent);
    view->setPlainText(text + tail);
    view->setReadOnly(true);
    view->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    QFont font(g_fontname, g_fontsize);
    view->document()->setDefaultFont(font);
    view->moveCursor(QTextCursor::Start);
    view->show();
    return view;
}

//---------------------------------------------------------------------------
int DCPCALL ListSendCommand(
        HWND t_listWin,
        int  t_command,
        int  t_parameter)
{
    QTextEdit *view = static_cast<QTextEdit*>(t_listWin);
    if (!view) {
        return LISTPLUGIN_ERROR;
    }
    switch (t_command) {
        case lc_selectall:
            view->selectAll();
            break;
        case lc_copy:
            view->copy();
            break;
        default:
            return LISTPLUGIN_ERROR;
    }
    return LISTPLUGIN_ERROR;
}

//---------------------------------------------------------------------------
void DCPCALL ListCloseWindow(
        HWND t_listWin)
{
    QTextEdit *view = static_cast<QTextEdit*>(t_listWin);
    if (!view) {
        return;
    }
    delete view;
}
