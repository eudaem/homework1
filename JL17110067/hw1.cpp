#include <iostream>
#include <fstream>
#include <ctype.h>
#include <string>
#include <list>
#include <assert.h>
#include <dirent.h>
#include <cstring>
#include <limits.h>
#define EOF_CHAR INT_MAX


using namespace std;
typedef unsigned long long u64;
const size_t buffer_size = 1024 * 128;
const size_t hash_size = 1024 * 1024;


/*
 * hash table
 */
template<typename K>
struct hash_map {
    list <pair<K, size_t> > table[hash_size];

    /// increase the number of a key with default count 0
    void increase(const K &key) {
      size_t h = key.hash % hash_size;
      for (typename list<pair<K, size_t> >::iterator it = table[h].begin(); it != table[h].end(); it++) {
        if (it->first == key) {
          it->second++;
          return;
        }
      }
      table[h].push_back(make_pair(key, 1));
    }

    /// increase the number of a key, and update that key without changing hash code
    void increase_update(const K &key) {
      size_t h = key.hash % hash_size;
      for (typename list<pair<K, size_t> >::iterator it = table[h].begin(); it != table[h].end(); it++) {
        if (it->first == key) {
          if (key.compare(it->first)) {
              // update
              it->first = key;
          }
          it->second++;
          return;
        }
      }
      table[h].push_back(make_pair(key, 1));
    }

    /// find a key in hash table
    pair<K, size_t> find(const K &key) {
      size_t h = key.hash % hash_size;
      for (typename list<pair<K, size_t> >::iterator it = table[h].begin(); it != table[h].end(); it++) {
        if (it->first == key) {
          return *it;
        }
      }
      return make_pair(K(), -1);
    }
};

/*
 * string_plus
 * add refinment string and hash code to a string
 */
struct string_plus {
    string str;
    string cmp_str;
    u64 hash;

    string_plus() {}

    string_plus(const char *s) {
      hash = 0;
      str = s;
      size_t size = str.size();
      cmp_str.reserve(size);
      
      // shrink
      while (isdigit(s[size - 1])) size--;

      // hash
      for (size_t i = 0; i < size; i++) {
        char ch = (s[i] >= 'a' && s[i] <= 'z') ? s[i] - 'a' + 'A' : s[i];
        cmp_str.push_back(ch);
        hash = (hash * 147 + ch);
      }
    }

    inline bool compare(const string_plus &s) const {
      return str < s.str;
    }

    inline bool operator==(const string_plus &x) const {
      if(hash!=x.hash) return false;
      return cmp_str == x.cmp_str;
    }
};

struct string_pair : public pair<string_plus, string_plus> {
    u64 hash;

    string_pair() { hash = 0; }

    string_pair(const pair<string_plus, string_plus> &p) {
      first = p.first;
      second = p.second;
      hash = p.first.hash * p.second.hash;
    }

};

struct stats_ {
    u64 characters;
    u64 words;
    u64 lines;

    hash_map<string_plus> words_count_map;
    hash_map<string_pair> phrase_count_map;

    stats_() {
      characters = words = lines = 0;
    }

} stats;

struct word_consumer_ {

    string_plus last_ref;

    // consume a word
    void consume(const char *s) {
      string_plus ref(s);
      stats.words++;
      
      // word
      stats.words_count_map.increase_update(ref);
      
      // phrase
      if (!last_ref.str.empty())
        stats.phrase_count_map.increase(make_pair(last_ref, ref));
      
      last_ref = ref;
    }

} word_consumer;

struct char_consumer_ {

    enum {
        S_WHITE, S_ALPHA, S_NUMBER
    } state;

    char word_buf[buffer_size];
    int word_len;
    int prev;

    // judge if a word is valid
    inline bool is_valid_word() {
      if (word_len < 4) return false;
      for (size_t i = 0; i < 4; i++)
        if (!isalpha(word_buf[i]))
          return false;
      return true;
    }

    // consume a char
    inline void consume(int ch) {
      
      // count lines
      if(ch!=EOF_CHAR) {
        if( (prev=='\n') || (prev==EOF_CHAR) ) {
          stats.lines++;
        }
      } 

      // record
      prev = ch;

      // count chars
      if (ch >= 32 && ch <= 126) {
        stats.characters++;
      } else {
        ch = 0;
      }

      assert(word_len <= buffer_size);

      // dfa
      if (isalpha(ch)) {
        switch (state) {
          case S_WHITE:
            // start a new word
            word_len = 1;
            word_buf[0] = ch;
            state = S_ALPHA;
            break;
          case S_ALPHA:
            word_buf[word_len++] = ch;
            break;
          default:
            break;
        }
      } else if (isdigit(ch)) {
        switch (state) {
          case S_ALPHA:
            word_buf[word_len++] = ch;
            state = S_ALPHA;
            break;
          default:
            state = S_NUMBER;
            break;
        }
      } else {
        if (is_valid_word()) {
          word_buf[word_len++] = 0;
          word_consumer.consume(word_buf);
          word_len = 0;
        }
        state = S_WHITE;
      }
    }
} char_consumer;


template<typename T, int N>
struct top {
    pair<T, size_t> res[N];

    // find the top N items in hash table
    top(hash_map<T>& col) {
      for (size_t i = 0; i < N; i++) res[i].second = 0;

      // for each item
      for (size_t i = 0; i < hash_size; i++) {
        list <pair<T, size_t> > &slot = col.table[i];
        for (typename list<pair<T, size_t> >::iterator it = slot.begin(); it != slot.end(); it++) {
          // update the top N records
          size_t pos = N;
          while (pos >= 1 && it->second > res[pos-1].second) pos--;
          for (size_t i = N - 1; i > pos; i--) {
              res[i] = res[i - 1];
          }
          // important condition to avoid index out of bound 
          if (pos <= N - 1) res[pos] = *it;
        }
      }
    }

};

/// buffer of read
char read_buf[buffer_size];

void readfile(string filename) {

#ifdef DEBUG
  cout << filename << "\n";
#endif

  // important clear-up
  word_consumer.last_ref = "";
  char_consumer.prev = EOF_CHAR;

  ifstream fin(filename.c_str());
  while (!fin.eof()) {
    fin.read(read_buf, buffer_size);
    long long n = fin.gcount();
    for (long long i = 0; i < n; i++) char_consumer.consume(read_buf[i]);
  }
  char_consumer.consume(EOF_CHAR);
  fin.close();
}

/*
 * write back the result
 */
void write_result() {
#ifdef DEBUG
  cout<<"write\n";
#endif
  FILE *fout;
  fout = fopen("result.txt", "w+");
  assert(fout != NULL);
  fprintf(fout, "char_number :%llu\n", stats.characters);
  fprintf(fout, "line_number :%llu\n", stats.lines);
  fprintf(fout, "word_number :%llu\n", stats.words);

  // get the top 10  
  top<string_plus, 10> top_words(stats.words_count_map);
  top<string_pair, 10> top_phrase(stats.phrase_count_map);
  
  fprintf(fout, "\nthe top ten frequency of word :\n");
  for (size_t i = 0; i < 10; i++) {
    string_plus s1 = top_words.res[i].first;
    if (!s1.str.empty())
      fprintf(fout, "%s\t%d\n", s1.str.c_str(), top_words.res[i].second);
  }

  // phrase
  fprintf(fout, "\n\nthe top ten frequency of phrase :\n");
  for (size_t i = 0; i < 10; i++) {
    string_pair p = top_phrase.res[i].first;
    if (p.first.str.empty()) continue;
    pair<string_plus, int> ref1 = stats.words_count_map.find(p.first);
    pair<string_plus, int> ref2 = stats.words_count_map.find(p.second);
    assert(ref1.second != -1 && ref2.second != -1);
    fprintf(fout, "%s %s\t%d\n", ref1.first.str.c_str(), ref2.first.str.c_str(), top_phrase.res[i].second);
  }

  fclose(fout);
}

/*
 * search the directory recursively
 */
void search(string file) {
  DIR *dir = opendir(file.c_str());
  dirent *info;

  if (dir) {
    string dir_path = file + "/*.*";
    while ((info = readdir(dir)) != NULL) {
      // ignore '.' and '..'
      if ((info->d_name[0] == '.') &&
          ((info->d_name[1] == 0) || (info->d_name[1] == '.' && info->d_name[2] == 0)))
        continue;
      search(string().assign(file).append("/").append(info->d_name));
    }
    closedir(dir);
  } else {
    readfile(file);
  }
}

int main(int argc, const char *argv[]) {
  if (argc <= 1) {
    printf("fatal error: no such file or directory\n");
    return -1;
  }
  string arg = argv[1];
  search(arg);
  write_result();
  return 0;
}