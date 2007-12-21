#include "alarm-applet.h"
#include "ui.h"

void
display_error_dialog (const gchar *message, const gchar *secondary_text, GtkWindow *parent)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (parent,
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
									message);
	
	if (secondary_text != NULL) {
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
												  secondary_text);
	}
	
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void
update_label (AlarmApplet *applet)
{
	gchar *tmp;
	struct tm *tm;
	guint hour, min, sec, now;
	
	if (applet->started) {
		if (applet->label_type == LABEL_TYPE_REMAIN) {
			now = time (NULL);
			sec = applet->alarm_time - now;
			
			min = sec / 60;
			sec -= min * 60;
			
			hour = min / 60;
			min -= hour * 60;
			
			tmp = g_strdup_printf(_("%02d:%02d:%02d"), hour, min, sec);
			
		} else {
			tm = localtime (&(applet->alarm_time));
			tmp = g_strdup_printf(_("%02d:%02d:%02d"), tm->tm_hour, tm->tm_min, tm->tm_sec);
		}
		
		g_object_set(applet->label, "label", tmp, NULL);
		g_free(tmp);
	} else if (applet->alarm_triggered) {
		g_object_set(applet->label, "label", _("Alarm!"), NULL);
	} else {
		g_object_set(applet->label, "label", _("No alarm"), NULL);
	}
}

gboolean
is_separator (GtkTreeModel *model, GtkTreeIter *iter, gpointer sep_index)
{
	GtkTreePath *path;
	gboolean result;

	path = gtk_tree_model_get_path (model, iter);
	result = gtk_tree_path_get_indices (path)[0] == GPOINTER_TO_INT (sep_index);
	gtk_tree_path_free (path);

	return result;
}

/*
 * Shamelessly stolen from gnome-da-capplet.c
 */
void
fill_combo_box (GtkComboBox *combo_box, GList *list, const gchar *custom_label)
{
	GList *l;
	GtkTreeModel *model;
	GtkCellRenderer *renderer;
	GtkTreeIter iter;
	GdkPixbuf *pixbuf = NULL;
	GtkIconTheme *theme;
	AlarmListEntry *entry;
	
	g_debug ("fill_combo_box... %d", g_list_length (list));

//	if (theme == NULL) {
	theme = gtk_icon_theme_get_default ();
//	}

	gtk_combo_box_set_row_separator_func (combo_box, is_separator,
					  GINT_TO_POINTER (g_list_length (list)), NULL);

	model = GTK_TREE_MODEL (gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING));
	gtk_combo_box_set_model (combo_box, model);

	renderer = gtk_cell_renderer_pixbuf_new ();

	/* not all cells have a pixbuf, this prevents the combo box to shrink */
	gtk_cell_renderer_set_fixed_size (renderer, -1, 22);
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box), renderer,
					"pixbuf", PIXBUF_COL,
					NULL);

	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box), renderer,
					"text", TEXT_COL,
					NULL);

	for (l = list; l != NULL; l = g_list_next (l)) {
		entry = (AlarmListEntry *) l->data;
		
		pixbuf = gtk_icon_theme_load_icon (theme, entry->icon, 22, 0, NULL);
		
		gtk_list_store_append (GTK_LIST_STORE (model), &iter);
		gtk_list_store_set (GTK_LIST_STORE (model), &iter,
					PIXBUF_COL, pixbuf,
					TEXT_COL, entry->name,
					-1);
		
		if (pixbuf)
			g_object_unref (pixbuf);
	}

	gtk_list_store_append (GTK_LIST_STORE (model), &iter);
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, -1);
	gtk_list_store_append (GTK_LIST_STORE (model), &iter);
	gtk_list_store_set (GTK_LIST_STORE (model), &iter,
			PIXBUF_COL, NULL,
			TEXT_COL, custom_label,
			-1);
}

void
set_alarm_dialog_response_cb (GtkDialog *dialog,
							  gint rid,
							  AlarmApplet *applet)
{
	guint hour, minute, second;
	gchar *message;
	
	g_debug ("Set-Alarm Response: %s", (rid == GTK_RESPONSE_OK) ? "OK" : "Cancel");
	
	if (rid == GTK_RESPONSE_OK) {
		// Store info & start timer
		hour    = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (applet->hour));
		minute  = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (applet->minute));
		second  = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (applet->second));
		
		g_object_get (applet->message, "text", &message, NULL);
		
		alarm_gconf_set_started (applet, TRUE);
		alarm_gconf_set_alarm (applet, hour, minute, second);
		alarm_gconf_set_message (applet, message);
		
		g_free (message);
	}
	
	gtk_widget_destroy (GTK_WIDGET (dialog));
	applet->set_alarm_dialog = NULL;
}

void
set_alarm_dialog_populate (AlarmApplet *applet)
{
	if (applet->set_alarm_dialog == NULL)
		return;
	
	struct tm *tm;
	
	/* Fill fields */
	if (applet->alarm_time != 0) {
		tm = localtime (&(applet->alarm_time));
		g_object_set (applet->hour, "value", (gdouble)tm->tm_hour, NULL);
		g_object_set (applet->minute, "value", (gdouble)tm->tm_min, NULL);
		g_object_set (applet->second, "value", (gdouble)tm->tm_sec, NULL);
	}
	
	if (applet->alarm_message != NULL) {
		g_object_set (applet->message, "text", applet->alarm_message, NULL);
	}
}

void
display_set_alarm_dialog (AlarmApplet *applet)
{
	if (applet->set_alarm_dialog != NULL) {
		// Dialog already open.
		gtk_window_present (GTK_WINDOW (applet->set_alarm_dialog));
		return;
	}
	
	GladeXML *ui = glade_xml_new(ALARM_UI_XML, "set-alarm", NULL);
	GtkWidget *ok_button;
	
	/* Fetch widgets */
	applet->set_alarm_dialog = GTK_DIALOG (glade_xml_get_widget (ui, "set-alarm"));
	applet->hour  	= glade_xml_get_widget (ui, "hour");
	applet->minute	= glade_xml_get_widget (ui, "minute");
	applet->second	= glade_xml_get_widget (ui, "second");
	applet->message = glade_xml_get_widget (ui, "message");
	ok_button = glade_xml_get_widget (ui, "ok-button");
	
	set_alarm_dialog_populate (applet);
	
	/* Set response and connect signal handlers */
	/* TODO: Fix, this isn't working */
	gtk_widget_grab_default (ok_button);
	//gtk_dialog_set_default_response (applet->set_alarm_dialog, GTK_RESPONSE_OK);
	
	g_signal_connect (applet->set_alarm_dialog, "response", 
					  G_CALLBACK (set_alarm_dialog_response_cb), applet);
}