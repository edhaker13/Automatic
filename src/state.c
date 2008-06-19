/*
 * Copyright (C) 2008 Frank Aurich (1100101+automatic@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>

#include "automatic.h"
#include "output.h"
#include "list.h"


#define MAX_LINE_LEN	300




int save_state(NODE *head) {
	FILE *fp;
	NODE *current = head;
	char tmp[MAX_LINE_LEN + 1];
	const char *state_file = am_get_statefile();

	if(state_file) {
		dbg_printf(P_MSG, "Saving state (%d downloaded torrents) to disk", listCount(head));
		if((fp = fopen(state_file, "wb")) == NULL) {
			dbg_printf(P_ERROR, "[save_state] Error: Unable to open '%s' for writing: %s", state_file, strerror(errno));
			return -1;
		}
		while (current != NULL) {
			sprintf(tmp, "%s\n", current->item.url);
			if(!fwrite(tmp, strlen(tmp), 1, fp)){
				dbg_printf(P_ERROR, "[save_state] Error: Unable to write to '%s': %s", state_file, strerror(errno));
				fclose(fp);
				return -1;
			}
			current = current->pNext;
		}
		fclose(fp);
	}
	return 0;
}

int load_state(NODE **head) {
	FILE *fp;
	char line[MAX_LINE_LEN], str[MAX_LINE_LEN];
	int len;
	rss_item item;
	const char* state_file = am_get_statefile();

	item = newRSSItem();

	if((fp = fopen(state_file, "rb")) == NULL) {
		dbg_printf(P_ERROR, "[load_state] Error: Unable to open '%s' for reading: %s", state_file, strerror(errno));
		return -1;
	}
	while (fgets(line, MAX_LINE_LEN, fp)) {
		len = strlen(line);
		if(len > 20) {  /* arbitrary threshold for the length of an URL */
			strncpy(str, line, len);
			str[len-1] = '\0';
			item.url = str;
			add_to_bucket(item, head, 0);
		}
	}
	fclose(fp);
	dbg_printf(P_MSG, "[load_state] Restored %d old entries", listCount(*head));
	print_list(*head);
	return 0;
}

