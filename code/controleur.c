#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "modele.h"
#include "vue.h"
#include "controleur.h"
#include "list.h"





Controleur_t *creer_controleur(Vue_t *v, Modele_t *m){

	Controleur_t *c = malloc(sizeof(Controleur_t));
	if(c == NULL)
		return NULL;
	c->m = m;
	c->v = v;
	c->spinButtonNLib = gtk_hscale_new_with_range(0.0, 10.0, 1.0);
    gtk_scale_set_draw_value(GTK_SCALE(c->spinButtonNLib), TRUE);
   

	c->entryExeName = gtk_entry_new_with_max_length(20);
	c->entryLibName = empty_list();
	c->entryMainName = gtk_entry_new_with_max_length(20);
	c->checkButtonGtkMode = gtk_check_button_new_with_label("Gtk+2");
	c->checkButtonLibSeparateMode = gtk_check_button_new_with_label("Separates libs");
	c->checkButtonLibCommuneMode = gtk_check_button_new_with_label("Commune lib");
	c->checkButtonCheckFileMode = gtk_check_button_new_with_label("Check files");
	c->checkButtonOpenWithApp = gtk_check_button_new_with_label("Open with app");
	c->entryOpenApp = gtk_entry_new_with_max_length(20);
	c->buttonConfirm = gtk_button_new_with_label("[Make]");
	c->checkButtonCompressed = gtk_check_button_new_with_label("Compress project");
	c->checkButtonCustomCflagsMode = gtk_check_button_new_with_label("Custom CFLAGS");
	c->entryCflags = gtk_entry_new_with_max_length(MAXC_CFLAGS);

	return c;
}

void destroy_controleur(Controleur_t *c){

	c->entryLibName = remove_list(c->entryLibName);
	free(c);
}

extern void fill_box(GtkWidget *box, int Nwg, ...);

void spin_add_library_entry(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;

	const int nLib = (int) gtk_range_get_value(GTK_RANGE(widget));

	while(nLib < length_list(c->entryLibName)){
		c->entryLibName = remove_first_element(c->entryLibName);
		spin_sub_library_label(c->v);
		spin_sub_library_hbox(c->v);
	}
	while(nLib > length_list(c->entryLibName)){
		c->entryLibName = add_first(c->entryLibName, gtk_entry_new_with_max_length(20));
		spin_add_library_label(c->v);
		spin_add_library_hbox(c->v);
		GtkWidget *tmpBox = (GtkWidget *) get_element(c->v->boxLib, 0);
		GtkWidget *tmpLabel = (GtkWidget *) get_element(c->v->labelEntryNameLib, 0);
		GtkWidget *tmpEntry = (GtkWidget *) get_element(c->entryLibName, 0);
		fill_box(tmpBox, 2, tmpLabel, tmpEntry);
		box_in_window_and_show(c->v);
	}
	if(c->m->openAppMode)
		gtk_widget_show(c->entryOpenApp);
	else
		gtk_widget_hide(c->entryOpenApp);

	if(c->m->customCflagsMode)
		gtk_widget_show(c->entryCflags);
	else
		gtk_widget_hide(c->entryCflags);
	

}



void destroy_fenetre(GtkWidget *widget, gpointer pData){

	gtk_main_quit();
}


static int check_entry_space(char *entryText){

	if(!strlen(entryText))
		return 0;

	for(int i = 0; i < strlen(entryText); i++)
		if(entryText[i] != ' ')
			return 1;

	return 0;
}

static int check_entry_empty(Controleur_t *c){

	if(!check_entry_space(gtk_entry_get_text(GTK_ENTRY(c->entryExeName))))
		return 1;
	if(!check_entry_space(gtk_entry_get_text(GTK_ENTRY(c->entryMainName))))
		return 1;
	if(!check_entry_space(gtk_entry_get_text(GTK_ENTRY(c->entryOpenApp))) && c->m->openAppMode)
		return 1;
	if(!check_entry_space(gtk_entry_get_text(GTK_ENTRY(c->entryCflags))) && c->m->customCflagsMode)
		return 1;
	for(int i = 0; i < length_list(c->entryLibName); i++)
		if(!check_entry_space(gtk_entry_get_text(GTK_ENTRY((GtkWidget *)get_element(c->entryLibName, i)))))
			return 1;
	return 0;
}

void make_makefile(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	if(check_entry_empty(c)){
		gtk_label_set_text(GTK_LABEL(c->v->labelWarning), "[empty entry]");
		return;
	}

	if(c->m->openAppMode)
		strcpy(c->m->app, (char *) gtk_entry_get_text(GTK_ENTRY(c->entryOpenApp)));
	if(c->m->customCflagsMode)
		strcpy(c->m->cflags, (char *) gtk_entry_get_text(GTK_ENTRY(c->entryCflags)));
	
	if(!run(c->m, c->entryExeName, c->entryMainName, c->entryLibName, c->entryOpenApp)){
		gtk_label_set_text(GTK_LABEL(c->v->labelWarning), "[file not found]");
		return;
	}

	gtk_label_set_text(GTK_LABEL(c->v->labelWarning), "[makefile created]");
}

void gtk_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	active_mode(c->m, 1);
}

void libs_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	
	active_mode(c->m, 2);
	if(c->m->libSepMode)
		gtk_widget_set_sensitive(c->checkButtonLibCommuneMode, FALSE);
	else
		gtk_widget_set_sensitive(c->checkButtonLibCommuneMode, TRUE);
}

void libc_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	active_mode(c->m, 3);
	if(c->m->libComMode)
		gtk_widget_set_sensitive(c->checkButtonLibSeparateMode, FALSE);
	else
		gtk_widget_set_sensitive(c->checkButtonLibSeparateMode, TRUE);
}

void checkf_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	active_mode(c->m, 4);
}

void open_app_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	active_mode(c->m, 5);
	if(c->m->openAppMode)
		gtk_widget_show(c->entryOpenApp);
	else
		gtk_widget_hide(c->entryOpenApp);
}

void compressed_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	active_mode(c->m, 6);
}

void custom_cflags_mode(GtkWidget *widget, gpointer pData){

	Controleur_t *c = (Controleur_t *) pData;
	active_mode(c->m, 7);
	if(c->m->customCflagsMode)
		gtk_widget_show(c->entryCflags);
	else
		gtk_widget_hide(c->entryCflags);
}




