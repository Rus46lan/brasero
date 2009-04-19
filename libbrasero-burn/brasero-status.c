/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Libbrasero-burn
 * Copyright (C) Philippe Rouquier 2005-2009 <bonfire-app@wanadoo.fr>
 *
 * Libbrasero-burn is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The Libbrasero-burn authors hereby grant permission for non-GPL compatible
 * GStreamer plugins to be used and distributed together with GStreamer
 * and Libbrasero-burn. This permission is above and beyond the permissions granted
 * by the GPL license by which Libbrasero-burn is covered. If you modify this code
 * you may extend this exception to your version of the code, but you are not
 * obligated to do so. If you do not wish to do so, delete this exception
 * statement from your version.
 * 
 * Libbrasero-burn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include "brasero-status.h"

struct _BraseroStatus {
	BraseroBurnResult res;

	GError *error;
	gdouble progress;
	gchar *current_action;
};

BraseroStatus *
brasero_status_new (void)
{
	return g_new0 (BraseroStatus, 1);
}

void
brasero_status_free (BraseroStatus *status)
{
	if (status->error)
		g_error_free (status->error);

	if (status->current_action)
		g_free (status->current_action);

	g_free (status);
}

BraseroBurnResult
brasero_status_get_result (BraseroStatus *status)
{
	g_return_val_if_fail (status != NULL, BRASERO_BURN_ERR);
	return status->res;
}

gdouble
brasero_status_get_progress (BraseroStatus *status)
{
	g_return_val_if_fail (status != NULL, -1.0);
	if (status->res == BRASERO_BURN_OK)
		return 1.0;

	if (status->res != BRASERO_BURN_NOT_READY)
		return -1.0;

	return status->progress;
}

GError *
brasero_status_get_error (BraseroStatus *status)
{
	g_return_val_if_fail (status != NULL, NULL);
	if (status->res != BRASERO_BURN_ERR)
		return NULL;

	return g_error_copy (status->error);
}

gchar *
brasero_status_get_current_action (BraseroStatus *status)
{
	gchar *string;

	g_return_val_if_fail (status != NULL, NULL);
	if (status->res != BRASERO_BURN_NOT_READY)
		return NULL;

	string = g_strdup (status->current_action);
	return string;

}

void
brasero_status_set_completed (BraseroStatus *status)
{
	g_return_if_fail (status != NULL);
	status->res = BRASERO_BURN_OK;
	status->progress = 1.0;
}

void
brasero_status_set_not_ready (BraseroStatus *status,
			      gdouble progress,
			      const gchar *current_action)
{
	g_return_if_fail (status != NULL);
	status->res = BRASERO_BURN_NOT_READY;
	status->progress = progress;

	if (status->current_action)
		g_free (status->current_action);
	status->current_action = g_strdup (current_action);
}

void
brasero_status_set_error (BraseroStatus *status,
			  GError *error)
{
	g_return_if_fail (status != NULL);

	status->res = BRASERO_BURN_ERR;
	status->progress = -1.0;

	if (status->error)
		g_error_free (status->error);
	status->error = error;
}

