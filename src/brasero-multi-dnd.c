/***************************************************************************
 *            multi-dnd.c
 *
 *  Wed Sep 27 17:34:41 2006
 *  Copyright  2006  Rouquier Philippe
 *  <Rouquier Philippe@localhost.localdomain>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <gtk/gtktreestore.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtktreemodelsort.h>
#include <gtk/gtktreemodelfilter.h>
#include <gtk/gtktreednd.h>

#include "eggtreemultidnd.h"

static gboolean
brasero_multi_DND_row_draggable (EggTreeMultiDragSource *drag_source,
				 GList *path_list)
{
	GList *iter;

	for (iter = path_list; iter && iter->data; iter = iter->next) {
		GtkTreePath *path;
		GtkTreeRowReference *reference;

		reference = iter->data;
		path = gtk_tree_row_reference_get_path (reference);
		if (gtk_tree_drag_source_row_draggable (GTK_TREE_DRAG_SOURCE (drag_source), path) == FALSE) {
			gtk_tree_path_free (path);
			return FALSE;
		}
		gtk_tree_path_free (path);
	}

	return TRUE;
}

static gboolean
brasero_multi_DND_drag_data_get (EggTreeMultiDragSource *drag_source, 
				 GList *path_list, 
				 GtkSelectionData *selection_data)
{
	GtkSelectionData *selection_tmp;
	GList *uris_list = NULL;
	gchar **uris;
	GList *iter;
	gint i;

	if (selection_data->target != gdk_atom_intern ("text/uri-list", TRUE))
		return TRUE;

	for (iter = path_list; iter && iter->data; iter = iter->next) {
		gchar **tmp;
		gboolean result;
		GtkTreePath *path;
		GtkTreeRowReference *reference;

		reference = iter->data;
		path = gtk_tree_row_reference_get_path (reference);

		selection_tmp = gtk_selection_data_copy (selection_data);
		result = gtk_tree_drag_source_drag_data_get (GTK_TREE_DRAG_SOURCE (drag_source),
							     path,
							     selection_tmp);
		gtk_tree_path_free (path);

		uris = gtk_selection_data_get_uris (selection_tmp);
		for (tmp = uris; tmp && *tmp; tmp++)
			uris_list = g_list_prepend (uris_list, *tmp);
		g_free (uris);

		gtk_selection_data_free (selection_tmp);

		if (!result) {
			g_list_foreach (uris_list, (GFunc) g_free, NULL);
			g_list_free (uris_list);
			return FALSE;
		}
	}

	uris = g_new0 (gchar*, g_list_length (uris_list) + 1);
	uris_list = g_list_reverse (uris_list);
	for (iter = uris_list, i = 0; iter; i++, iter = iter->next)
		uris [i] = iter->data;

	g_list_free (uris_list);

	gtk_selection_data_set_uris (selection_data, uris);
	g_strfreev (uris);
	return TRUE;
}

static gboolean
brasero_multi_DND_drag_data_delete (EggTreeMultiDragSource *drag_source,
				    GList *path_list)
{
	return TRUE;
}

static void
brasero_multi_DND_drag_source_init (EggTreeMultiDragSourceIface *iface)
{
	iface->row_draggable = brasero_multi_DND_row_draggable;
	iface->drag_data_get = brasero_multi_DND_drag_data_get;
	iface->drag_data_delete = brasero_multi_DND_drag_data_delete;
}

static const GInterfaceInfo multi_DND_drag_source_info = {
	(GInterfaceInitFunc) brasero_multi_DND_drag_source_init,
	NULL,
	NULL
};

void
brasero_enable_multi_DND (void)
{
	GType object_type;

	object_type = gtk_tree_model_filter_get_type ();
	g_type_add_interface_static (object_type,
				     EGG_TYPE_TREE_MULTI_DRAG_SOURCE,
				     &multi_DND_drag_source_info);

	object_type = gtk_tree_model_sort_get_type ();
	g_type_add_interface_static (object_type,
				     EGG_TYPE_TREE_MULTI_DRAG_SOURCE,
				     &multi_DND_drag_source_info);

	object_type = gtk_tree_store_get_type ();
	g_type_add_interface_static (object_type,
				     EGG_TYPE_TREE_MULTI_DRAG_SOURCE,
				     &multi_DND_drag_source_info);

	object_type = gtk_list_store_get_type ();
	g_type_add_interface_static (object_type,
				     EGG_TYPE_TREE_MULTI_DRAG_SOURCE,
				     &multi_DND_drag_source_info);
}
 
