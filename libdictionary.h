#ifndef LIBDICTIONARY_H_INCLUDED
#define LIBDICTIONARY_H_INCLUDED

struct DictionaryEntry {
  char **entries;
  int length;
};

struct Dictionary {
  struct DictionaryEntry **entries;
  int length;
};

struct DictionaryIterator {
  struct Dictionary *dictionary;
  int *entries;
  int hasReachedLast;
};

int dictionaryIterator_nextEntry(struct DictionaryIterator **dictionaryIterator,
                                 char **nextElement);
void dictionaryIterator_allocate(struct DictionaryIterator **dictionaryIterator,
                                 struct Dictionary **dictionary);

void dictionary_newSegment(struct Dictionary **dictionary);
void dictionary_addWord(struct Dictionary **dictionary, char *word);

void dictionary_allocate(struct Dictionary **dictionary);
void dictionary_fromFile(char *filename, struct Dictionary **dictionary);
int dictionary_getSize(struct Dictionary **dictionary);
void dictionary_free(struct Dictionary **dictionary);
void dictionaryIterator_free(struct DictionaryIterator **dictionaryIterator);

#endif