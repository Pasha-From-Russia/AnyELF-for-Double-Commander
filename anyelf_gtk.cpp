/*
Copyright (C) 2020 by Pasha-From-Russia

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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "anyelf_gtk.h"
#include <elfio/elfio_version.hpp>
#include <fcntl.h>
#include <stdarg.h>

std::string g_text;
GtkWidget *g_view;
GtkWidget *g_scroll;
std::string g_searchText;
bool g_newSearch;
int g_previousCursorPos = 0;

static const gchar *g_fontname = "monospace 12";

//---------------------------------------------------------------------------
gint showDialog(
		HWND t_parent,
		GtkMessageType t_msgType,
		GtkButtonsType t_buttons,
		const char* t_printmask,
		...
		)
{
	char buf[512];
	va_list argList;
	va_start(argList, t_printmask);
	vsnprintf(buf, sizeof(buf), t_printmask, argList);
	va_end(argList);

	GtkWidget *dialog = gtk_message_dialog_new(
					GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(t_parent))),
					GTK_DIALOG_MODAL,
					t_msgType,
					t_buttons,
					"%s",
					(const gchar*)buf);
	gint ret = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return ret;
}

//---------------------------------------------------------------------------
gboolean key_press(
		GtkWidget *t_window,
		GdkEventKey* t_event,
		GtkTextBuffer *t_buffer)
{
	if (t_event->type != GDK_KEY_PRESS) {
		return FALSE;
	}

	bool ctrlPressed = (t_event->state & GDK_CONTROL_MASK) != 0;

	GtkScrollType scrollType = GTK_SCROLL_NONE;

	switch (t_event->keyval) {
		case GDK_KEY_Home:
			if(ctrlPressed) {
				scrollType = GTK_SCROLL_START;
			}
			break;
		case GDK_KEY_End:
			if(ctrlPressed) {
				scrollType = GTK_SCROLL_END;
			}
			break;
		case GDK_KEY_Up:
			scrollType = GTK_SCROLL_STEP_BACKWARD;
			break;
		case GDK_KEY_Down:
			scrollType = GTK_SCROLL_STEP_FORWARD;
			break;
		case GDK_KEY_Page_Up:
			if(ctrlPressed) {
				scrollType = GTK_SCROLL_START;
			} else {
				scrollType = GTK_SCROLL_PAGE_BACKWARD;
			}
			break;
		case GDK_KEY_Page_Down:
			if(ctrlPressed) {
				scrollType = GTK_SCROLL_END;
			} else {
				scrollType = GTK_SCROLL_PAGE_FORWARD;
			}
			break;
		default:
			return FALSE;
	}

	if(scrollType == GTK_SCROLL_NONE) {
		return FALSE;
	}

	gboolean sig_value;
	g_signal_emit_by_name(
				G_OBJECT(g_scroll),
				"scroll-child",
				scrollType,
				FALSE,
				&sig_value);
	return TRUE;
}

//---------------------------------------------------------------------------
enum {
	SEARCH_OK,
	SEARCH_NOT_FOUND
};

enum SEARCH_DIRECTION {
	SR_FORWARD,
	SR_BACKWARD
};

gboolean my_gtk_text_iter_search(
							GtkTextIter *t_iter,
							const gchar *t_searchText,
							bool t_caseSensitive,
							GtkTextIter *t_mstart,
							GtkTextIter *t_mend,
							SEARCH_DIRECTION t_direction)
{
	if(t_direction != SR_FORWARD && t_direction != SR_BACKWARD) {
		t_direction = SR_FORWARD;
	}

	size_t textLen = strlen(t_searchText);
	if(textLen == 0) {
		return FALSE;
	}

	char *textDup = strdup(t_searchText);
	if(!textDup) {
		return FALSE; //strdup failed
	}

	if(!t_caseSensitive) {
		for(size_t i = 0; i < textLen; i++) {
			textDup[i] = tolower(textDup[i]);
		}
	}

	gunichar c;
	bool ok = false;
	size_t uLoop;
	int iLoop;
	if(t_direction == SR_BACKWARD) {
		*t_mend = *t_iter;
	}

	while(true) {
		if(t_direction == SR_BACKWARD) {
			if(!gtk_text_iter_backward_char(t_iter)) {
				break;
			}
		}

		c = gtk_text_iter_get_char(t_iter);
		if(!t_caseSensitive) {
			c = g_unichar_tolower(c);
		}

		if(t_direction == SR_FORWARD) {

			/* search forward */

			if((gunichar)textDup[0] == c) {
				*t_mstart = *t_iter;

				for(uLoop = 0; uLoop < textLen; uLoop++) {
					c = gtk_text_iter_get_char(t_iter);
					if(!t_caseSensitive) {
						c = g_unichar_tolower(c);
					}

					if((gunichar)textDup[uLoop] != c ||
						!gtk_text_iter_forward_char(t_iter)) {
							break; //break for loop
					}
				}

				if(uLoop == textLen) {
					//found
					*t_mend = *t_iter;
					ok = true;
					break; //break while loop
				}
			}
			if(!gtk_text_iter_forward_char(t_iter)) {
				break; //break while loop
			}

			/* end search forward */

		} else {

			/* search backward */

			iLoop = (int)textLen - 1;

			if((gunichar)textDup[iLoop] == c) {
				while(iLoop--) {
					if(!gtk_text_iter_backward_char(t_iter)) {
						break;
					}
					c = gtk_text_iter_get_char(t_iter);
					if(!t_caseSensitive) {
						c = g_unichar_tolower(c);
					}

					if((gunichar)textDup[iLoop] != c) {
						gtk_text_iter_forward_char(t_iter); //go back
						break;
					}
				}
			}
			if(iLoop < 0) {
				*t_mstart = *t_iter;
				ok = true;
				break; //break while loop
			}

			*t_mend = *t_iter;
			/* end search backward */
		}
	}
	if(textDup) {
		free(textDup);
		textDup = NULL;
	}

	return ok ? TRUE : FALSE;
}

//---------------------------------------------------------------------------
int find(
		const char* t_searchString,
		int t_searchParameter,
		bool t_fromEnd = false)
{
	GtkTextIter iter;
	GtkTextIter mstart, mend;

	static GtkTextIter prev_mstart, prev_mend;

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(g_view));

	GtkTextMark *last_pos = gtk_text_buffer_get_mark(
								buffer,
								"last_pos");

	if(last_pos == NULL) {
		if(t_fromEnd) {
			gtk_text_buffer_get_end_iter(buffer, &iter);
		} else {
			gtk_text_buffer_get_start_iter(buffer, &iter);
		}
	} else {
		gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);
	}

	gboolean found;

	bool caseSensitive = (t_searchParameter & lcs_matchcase) != 0;

	if(t_searchParameter & lcs_backwards) {
		found = my_gtk_text_iter_search(
				&iter,
				t_searchString,
				caseSensitive,
				&mend,
				&mstart,
				SR_BACKWARD);
	} else {
		found = my_gtk_text_iter_search(
				&iter,
				t_searchString,
				caseSensitive,
				&mstart,
				&mend,
				SR_FORWARD);
	}

	bool shouldSelect = false;
	int ret = SEARCH_NOT_FOUND;
	if(found) {
		shouldSelect = true;
		ret = SEARCH_OK;
	} else {
		if(!g_newSearch) {
			mstart = prev_mstart;
			mend = prev_mend;
			shouldSelect = true;
		}
	}

	if(shouldSelect) {
		gtk_text_buffer_place_cursor(
					buffer,
					&mstart);
		gtk_text_buffer_select_range(buffer, &mstart, &mend);
		GtkTextMark* mark = gtk_text_buffer_create_mark(
					buffer,
					"last_pos",
					&mend,
					FALSE);
		gtk_text_view_scroll_to_mark(
					GTK_TEXT_VIEW(g_view),
					mark,
					0.0,
					true,
					0.0,
					0.17);

		prev_mstart = mstart;
		prev_mend = mend;
		g_newSearch = false;
		g_previousCursorPos = gtk_text_iter_get_offset(&mstart);
	}
	return ret;
}

/* API */
//---------------------------------------------------------------------------
int DCPCALL ListSearchText(
				HWND t_listWin,
				char* t_searchString,
				int t_searchParameter)
{
	if(!g_view || !t_searchString) {
		return LISTPLUGIN_ERROR;
	}

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(g_view));
	GtkTextMark *last_pos = gtk_text_buffer_get_mark(
								buffer,
								"last_pos");

	GtkTextMark *insertPointMark = gtk_text_buffer_get_insert(buffer);
	GtkTextIter insertPointIter;
	gtk_text_buffer_get_iter_at_mark(buffer, &insertPointIter, insertPointMark);
	gint currentCursorPos = gtk_text_iter_get_offset(&insertPointIter);

	std::string newSearchText = t_searchString;
	if(g_searchText != newSearchText) {
		if(last_pos) {
			gtk_text_buffer_delete_mark(buffer, last_pos);
		}
		last_pos = NULL;
		g_searchText = newSearchText;
		g_newSearch = true;
	} else if(currentCursorPos != g_previousCursorPos) {
		if(last_pos) {
			gtk_text_buffer_delete_mark(buffer, last_pos);
		}
	} else {
		g_newSearch = false;
	}

	bool firstRun = last_pos == NULL;
	bool backwardSearch = t_searchParameter & lcs_backwards;

	if(find(t_searchString, t_searchParameter) == SEARCH_OK) {
		return LISTPLUGIN_OK;
	} else {
		if(firstRun) {
			showDialog(
				t_listWin,
				GTK_MESSAGE_INFO,
				GTK_BUTTONS_OK,
				"\"%s\" not found!",
				t_searchString
				);
			return LISTPLUGIN_ERROR;
		}
		gint dlgRet = showDialog(
				t_listWin,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				"\"%s\" not found!\nTry to search from the %s?",
				t_searchString,
				backwardSearch ? "end" : "beginning"
				);

		if(dlgRet == GTK_RESPONSE_YES) {
			if(last_pos) {
				gtk_text_buffer_delete_mark(buffer, last_pos);
			}
			if(find(t_searchString, t_searchParameter, backwardSearch) == SEARCH_OK) {
				return LISTPLUGIN_OK;
			}
		}
	}
	return LISTPLUGIN_ERROR;
}

//---------------------------------------------------------------------------
HWND DCPCALL ListLoad(
				HWND t_parentWin,
				char* t_fileToLoad,
				int t_showFlags)
{
	g_searchText = "";
	g_previousCursorPos = 0;

	int f = open(t_fileToLoad, O_RDONLY);
	if(f == -1) {
		return NULL;
	}
	uint32_t header = 0;
	int8_t size = sizeof(header);
	read(f, &header, size);
	close(f);
	if(header != 0x464C457F) {
		//not 0x7F E L F signature
		return NULL;
	}

	g_text = elfdump(t_fileToLoad);
	if (g_text.empty()) {
		return NULL;
	}

	char info[512];

	snprintf(info, sizeof(info),
			 "\n\nELFIO %s by Serge Lamikhov-Center\n"
			 "AnyELF-GTK plugin v%d.%d for DoubleCommander by Pasha-From-Russia",
			ELFIO_VERSION,
			ANYELF_VERSION_HI,
			ANYELF_VERSION_LOW);

	g_text += info;

	GtkTextBuffer *buffer;

	GtkWidget *gFix = gtk_vbox_new(FALSE, 5);

	gtk_container_add(
				GTK_CONTAINER(GTK_WIDGET(t_parentWin)),
				gFix);
	g_scroll = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_add(
				GTK_CONTAINER(gFix),
				g_scroll);

	g_view = gtk_text_view_new();

	gtk_widget_add_events(g_view, GDK_BUTTON_PRESS_MASK);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_view));

	gtk_text_buffer_set_text(buffer, g_text.c_str(), -1);

	PangoFontDescription *font = pango_font_description_from_string(g_fontname);

	gtk_widget_modify_font(
				g_view,
				font);

	gtk_text_view_set_editable(GTK_TEXT_VIEW(g_view), FALSE);

	gtk_container_add(
				GTK_CONTAINER(g_scroll),
				g_view);

	gtk_scrolled_window_set_policy(
				GTK_SCROLLED_WINDOW(g_scroll),
				GTK_POLICY_AUTOMATIC,
				GTK_POLICY_AUTOMATIC);

	g_signal_connect(
			G_OBJECT(g_view),
			"key-press-event",
			G_CALLBACK(key_press),
			buffer);

	gtk_widget_show_all(gFix);

	return gFix;
}

//---------------------------------------------------------------------------
int DCPCALL ListSendCommand(
				HWND t_listWin,
				int t_command,
				int t_parameter)
{
	GtkTextIter p;

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_view));

	switch(t_command) {
		case lc_selectall:
			gtk_text_buffer_get_start_iter(buffer, &p);
			gtk_text_buffer_place_cursor(buffer, &p);
			gtk_text_buffer_get_end_iter(buffer, &p);
			gtk_text_buffer_move_mark_by_name(buffer, "selection_bound", &p);
			break;
		case lc_copy:
			gtk_text_buffer_copy_clipboard(
						buffer,
						gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
			break;
		default:
			return LISTPLUGIN_ERROR;
	}
	return LISTPLUGIN_OK;
}

//---------------------------------------------------------------------------
void DCPCALL ListGetDetectString(char* t_detectString, int t_maxlen)
{
	snprintf(t_detectString, t_maxlen, "%s", "EXT=\"*\"");
}

//---------------------------------------------------------------------------
void DCPCALL ListCloseWindow(HWND t_listWin)
{
	gtk_widget_destroy(GTK_WIDGET(t_listWin));
}
