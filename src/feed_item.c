/* $Id$
 * $Name$
 * $ProjectName$
 */

/**
 * @file feed_item.c
 *
 * RSS Item-specific functions.
 */

/*
 * Copyright (C) 2008 Frank Aurich (1100101+automatic@gmail.com)
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
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


#include "feed_item.h"
#include "filters.h"
#include "output.h"
#include "regex.h"
#include "utils.h"

#ifdef MEMWATCH
    #include "memwatch.h"
#endif


/** \brief Check if the provided rss item is a match for any of the given filters
 *
 * \param[in]  filters List of regular expressions to check against a given feed item
 * \param[in]  string  The string to be checked by the regular expression.
 * \param[out] folder  The particular download folder should this filter be a match .
 * \return 1 if a filter matched, 0 otherwise.
 *
 */
uint8_t isMatch(const am_filters filters, const char * string, const char * feedID, char **folder) {
  am_filters current_regex = NULL;
  am_filter filter;

  assert(folder != NULL);

  current_regex = filters;
  while (current_regex != NULL && current_regex->data != NULL) {
    filter = (am_filter) current_regex->data;

    /* The performRegexReplace method is only called in two cases:
    ** A) The given filter has no ID. We assume this filter matches items from all feeds.
    ** B) Both filter and feed have matching IDs.
    **
    ** In all other cases, the item is ignored.
    */
    if( !(filter->feedID && *filter->feedID) || (feedID && *feedID && strcasecmp(feedID, filter->feedID) == 0)) {
      *folder = performRegexReplace(string, filter->pattern, filter->folder);
      if(folder != NULL) {
        return 1;
      }
    }

    current_regex = current_regex->next;
  }

  return 0;
}

/** \brief Create a new RSS feed item
 *
 * \return New feed item.
 */
feed_item newFeedItem(void) {
    feed_item i = (feed_item)am_malloc(sizeof(struct feed_item));
    if(i != NULL) {
      i->name     = NULL;
      i->url      = NULL;
      i->category = NULL;
      i->guid     = NULL;
    }

    return i;
}

/** \brief Free all allocated memory associated with the given feed item
 *
 * \param[in] data Pointer to a feed item
 */
void freeFeedItem(void *data) {
  feed_item item = (feed_item)data;
  if(item != NULL) {
    if(item->name != NULL) {
      am_free(item->name);
      item->name = NULL;
    }

    if(item->url != NULL) {
      am_free(item->url);
      item->url = NULL;
    }

    if(item->category != NULL) {
      am_free(item->category);
      item->category = NULL;
    }

    if(item->guid != NULL) {
      am_free(item->guid);
      item->guid = NULL;
    }

    am_free(item);
    item = NULL;
  }
}
