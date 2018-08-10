#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>

#include "libabikeccak.h"
#include "libdictionary.h"

struct FunctionHash {
  char *hash;
  char *identifier;
  UT_hash_handle hh;
};

int insert_hashmap(char *entry, struct FunctionHash **hashTable) {
  struct FunctionHash *fh =
      (struct FunctionHash *)malloc(sizeof(struct FunctionHash));
  calculate_contract_abi_hash(entry, &(fh->hash));
  fh->identifier = strdup(entry);
  HASH_ADD_KEYPTR(hh, *hashTable, fh->hash, strlen(fh->hash), fh);
  return 0;
}

int test_against_hashmap(char *entry, struct FunctionHash **hashTable) {
  struct FunctionHash *fh;
  char *hash;
  int returnValue = 0;

  calculate_contract_abi_hash(entry, &hash);
  HASH_FIND_STR(*hashTable, hash, fh);
  if (fh) {
    printf("hash(\"%s\") = hash(\"%s\") = %s\n", entry, fh->identifier, hash);
    returnValue = 1;
  }
  free(hash);
  return returnValue;
}

void iterate_dictionary(char *dictionaryFile, struct FunctionHash **hashTable,
                        char *prefix, char *suffix,
                        int (*functionPtr)(char *,
                                           struct FunctionHash **hashTable)) {
  struct Dictionary *dictionary = NULL;
  dictionary_allocate(&dictionary);

  if (prefix != NULL) {
    dictionary_addWord(&dictionary, prefix);
    dictionary_newSegment(&dictionary);
  }

  dictionary_fromFile(dictionaryFile, &dictionary);

  if (prefix != NULL) {
    dictionary_newSegment(&dictionary);
    dictionary_addWord(&dictionary, suffix);
  }

  struct DictionaryIterator *dictionaryIterator = NULL;
  dictionaryIterator_allocate(&dictionaryIterator, &dictionary);

  char *generated = NULL;
  while (dictionaryIterator_nextEntry(&dictionaryIterator, &generated)) {
    (*functionPtr)(generated, hashTable);
  }

  if (generated) {
    free(generated);
  }
  dictionaryIterator_free(&dictionaryIterator);
  dictionary_free(&dictionary);
}

int bruteforce_mode_dictionary(char *fromDictionaryFile, char *toDictionaryFile,
                               char *fromPrefix, char *fromSuffix,
                               char *toPrefix, char *toSuffix) {
  struct FunctionHash *s, *tmp, *hashTable = NULL;

  iterate_dictionary(fromDictionaryFile, &hashTable, fromPrefix, fromSuffix,
                     &insert_hashmap);
  iterate_dictionary(toDictionaryFile, &hashTable, toPrefix, toSuffix,
                     &test_against_hashmap);

  HASH_ITER(hh, hashTable, s, tmp) {
    HASH_DEL(hashTable, s);
    free(s->hash);
    free(s->identifier);
    free(s);
  }

  return EXIT_SUCCESS;
}