#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libdictionary.h"

#define max(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
  })

int dictionary_getSize(struct Dictionary **dictionary) {
  int count = 1;
  for (int i = 0; i < (*dictionary)->length; i++) {
    count *= max(1, (*dictionary)->entries[i]->length);
  }
  return count;
}

void dictionaryIterator_increment(
    struct DictionaryIterator **dictionaryIterator) {
  for (int last = (*dictionaryIterator)->dictionary->length - 1; last >= 0;
       last--) {
    (*dictionaryIterator)->entries[last] += 1;
    if ((*dictionaryIterator)->entries[last] <
        (*dictionaryIterator)->dictionary->entries[last]->length) {
      return;
    }
    (*dictionaryIterator)->entries[last] = 0;
  }

  (*dictionaryIterator)->hasReachedLast = 1;
}

int dictionaryIterator_nextEntry(struct DictionaryIterator **dictionaryIterator,
                                 char **nextElement) {
  int requiredStringLength = 1;
  for (int i = 0; i < (*dictionaryIterator)->dictionary->length; i++) {
    if ((*dictionaryIterator)->dictionary->entries[i]->length == 0) {
      continue;
    }

    requiredStringLength +=
        strlen((*dictionaryIterator)
                   ->dictionary->entries[i]
                   ->entries[(*dictionaryIterator)->entries[i]]);
  }

  if (*nextElement != NULL) {
    free(*nextElement);
  }

  *nextElement = malloc(requiredStringLength * sizeof(char));
  (*nextElement)[0] = 0;
  for (int i = 0; i < (*dictionaryIterator)->dictionary->length; i++) {
    if ((*dictionaryIterator)->dictionary->entries[i]->length == 0) {
      continue;
    }

    strcat(*nextElement, (*dictionaryIterator)
                             ->dictionary->entries[i]
                             ->entries[(*dictionaryIterator)->entries[i]]);
  }
  int returnValue = (*dictionaryIterator)->hasReachedLast;
  dictionaryIterator_increment(dictionaryIterator);
  return !returnValue;
}

void dictionary_fromFile(char *filename, struct Dictionary **dictionary) {
  char *line = NULL;
  size_t len = 0, read;

  FILE *toBeLoadedFrom = fopen(filename, "r");

  if (toBeLoadedFrom == NULL) return;

  while ((read = getline(&line, &len, toBeLoadedFrom)) != -1) {
    if (line[read - 1] == '\n') {
      line[read - 1] = '\0';
    }

    if (strstr(line, "[NEXT]")) {
      dictionary_newSegment(dictionary);
    } else if (strstr(line, "[END]")) {
      break;
    } else {
      dictionary_addWord(dictionary, line);
    }
  }

  fclose(toBeLoadedFrom);
  if (line) free(line);
}

void dictionaryIterator_allocate(struct DictionaryIterator **dictionaryIterator,
                                 struct Dictionary **dictionary) {
  *dictionaryIterator = malloc(sizeof(struct DictionaryIterator));
  (*dictionaryIterator)->dictionary = *dictionary;
  (*dictionaryIterator)->hasReachedLast = 0;
  (*dictionaryIterator)->entries = calloc((*dictionary)->length, sizeof(int));
}

void dictionary_allocate(struct Dictionary **dictionary) {
  *dictionary = malloc(sizeof(struct Dictionary));
  (*dictionary)->length = 0;
}

void dictionary_allocateEntry(struct DictionaryEntry **dictionaryEntry) {
  *dictionaryEntry = malloc(sizeof(struct DictionaryEntry));
  (*dictionaryEntry)->length = 0;
}

char **dictionaryEntry_append(struct DictionaryEntry **dictionaryEntry) {
  if ((*dictionaryEntry)->length == 0) {
    (*dictionaryEntry)->entries = malloc(sizeof(char *));
  } else {
    (*dictionaryEntry)->entries =
        realloc((*dictionaryEntry)->entries,
                ((*dictionaryEntry)->length + 1) * sizeof(char *));
  }
  (*dictionaryEntry)->length += 1;

  return &((*dictionaryEntry)->entries[(*dictionaryEntry)->length - 1]);
}

struct DictionaryEntry **dictionary_append(struct Dictionary **dictionary) {
  if ((*dictionary)->length == 0) {
    (*dictionary)->entries = malloc(sizeof(struct DictionaryEntry *));
  } else {
    (*dictionary)->entries =
        realloc((*dictionary)->entries,
                ((*dictionary)->length + 1) * sizeof(struct DictionaryEntry *));
  }
  (*dictionary)->length += 1;

  return &((*dictionary)->entries[(*dictionary)->length - 1]);
}

void dictionary_newSegment(struct Dictionary **dictionary) {
  struct DictionaryEntry **dictionaryEntry = dictionary_append(dictionary);
  dictionary_allocateEntry(dictionaryEntry);
}

void dictionary_addWord(struct Dictionary **dictionary, char *word) {
  if ((*dictionary)->length == 0) {
    dictionary_newSegment(dictionary);
  }

  struct DictionaryEntry *dictionaryEntry =
      (*dictionary)->entries[(*dictionary)->length - 1];

  char **wordPosition = dictionaryEntry_append(&dictionaryEntry);
  *wordPosition = strdup(word);
}

void dictionaryEntry_free(struct DictionaryEntry **dictionaryEntry) {
  if ((*dictionaryEntry)->length > 0) {
    for (int i = 0; i < (*dictionaryEntry)->length; i++) {
      free((*dictionaryEntry)->entries[i]);
    }
    free((*dictionaryEntry)->entries);
  }
  free(*dictionaryEntry);
}

void dictionary_free(struct Dictionary **dictionary) {
  if ((*dictionary)->length > 0) {
    for (int i = 0; i < (*dictionary)->length; i++) {
      dictionaryEntry_free(&((*dictionary)->entries[i]));
    }

    free((*dictionary)->entries);
  }
  free(*dictionary);
}

void dictionaryIterator_free(struct DictionaryIterator **dictionaryIterator) {
  free((*dictionaryIterator)->entries);
  free(*dictionaryIterator);
}
