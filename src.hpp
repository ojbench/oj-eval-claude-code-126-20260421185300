#pragma once
#include <bits/stdc++.h>
using namespace std;

// Dimensions
inline int row = -1, col = -1;

// Sparse live cell set: key = (long long)r * col + c
static unordered_set<long long> live_cells;
static inline long long keyRC(int r, int c) { return (static_cast<long long>(r) << 20) ^ c; }
// Use mixing instead of multiplication by col since col may change; but rely on encode/decode helpers
static inline long long encode(int r, int c) { return (static_cast<long long>(r) << 20) | static_cast<long long>(c & ((1<<20)-1)); }
static inline int decodeR(long long k) { return static_cast<int>(k >> 20); }
static inline int decodeC(long long k) { return static_cast<int>(k & ((1<<20)-1)); }

inline void Initialize() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  live_cells.clear();
  cin >> col >> row;
  string dummy;
  getline(cin, dummy); // consume endline
  int r = 0, c = 0;
  long long num = 0;
  bool done = false;
  string line;
  while (!done && std::getline(cin, line)) {
    for (char ch : line) {
      if (ch == ' ' || ch == '\t' || ch == '\r') continue;
      if (ch >= '0' && ch <= '9') {
        num = num * 10 + (ch - '0');
        continue;
      }
      auto take_num = [&]() {
        long long cnt = (num == 0 ? 1 : num);
        num = 0;
        return cnt;
      };
      if (ch == 'o') {
        long long cnt = take_num();
        for (long long k = 0; k < cnt; ++k) {
          if (r >= 0 && r < row && c >= 0 && c < col) live_cells.insert(encode(r, c));
          ++c;
        }
      } else if (ch == 'b') {
        long long cnt = take_num();
        c += static_cast<int>(cnt);
      } else if (ch == '$') {
        long long cnt = take_num();
        r += static_cast<int>(cnt);
        c = 0;
      } else if (ch == '!') {
        done = true;
        break;
      } else {
        // ignore other characters
      }
    }
  }
}

inline void Tick() {
  // Neighbor count map
  unordered_map<long long, int> cnt;
  cnt.reserve(live_cells.size() * 8 + 8);
  // Directions
  static const int dr[8] = {-1,-1,-1,0,0,1,1,1};
  static const int dc[8] = {-1,0,1,-1,1,-1,0,1};

  for (auto k : live_cells) {
    int r = decodeR(k);
    int c = decodeC(k);
    for (int i = 0; i < 8; ++i) {
      int nr = r + dr[i];
      int nc = c + dc[i];
      if (nr < 0 || nr >= row || nc < 0 || nc >= col) continue;
      long long nk = encode(nr, nc);
      auto it = cnt.find(nk);
      if (it == cnt.end()) cnt.emplace(nk, 1);
      else if (it->second < 8) ++it->second;
    }
  }

  unordered_set<long long> next_live;
  next_live.reserve(live_cells.size() * 2 + 8);
  for (const auto &p : cnt) {
    long long pos = p.first;
    int n = p.second;
    bool alive = (live_cells.find(pos) != live_cells.end());
    if (n == 3 || (alive && n == 2)) next_live.insert(pos);
  }
  live_cells.swap(next_live);
}

inline void PrintGame() {
  // First line: col row
  cout << col << ' ' << row << '\n';

  if (live_cells.empty()) {
    cout << '!' << '\n';
    return;
  }

  // Build per-row columns map
  vector<vector<int>> rows;
  rows.assign(row, {});
  for (auto k : live_cells) {
    int r = decodeR(k);
    int c = decodeC(k);
    if (r >= 0 && r < row && c >= 0 && c < col) rows[r].push_back(c);
  }
  int last_non_empty = -1;
  for (int r = row - 1; r >= 0; --r) {
    if (!rows[r].empty()) { last_non_empty = r; break; }
  }
  if (last_non_empty < 0) {
    cout << '!' << '\n';
    return;
  }

  for (int r = 0; r <= last_non_empty; ) {
    if (rows[r].empty()) {
      // count consecutive empty rows starting at r
      int e = 0;
      while (r + e <= last_non_empty && rows[r + e].empty()) ++e;
      if (e > 0) {
        if (e > 1) cout << e;
        cout << '$';
        r += e;
      }
      continue;
    }

    auto &vec = rows[r];
    sort(vec.begin(), vec.end());
    vec.erase(unique(vec.begin(), vec.end()), vec.end());

    int cur = 0;
    for (size_t i = 0; i < vec.size(); ) {
      int start = vec[i];
      int end = start;
      size_t j = i + 1;
      while (j < vec.size() && vec[j] == end + 1) { end = vec[j]; ++j; }
      int dead_before = start - cur;
      if (dead_before > 1) cout << dead_before;
      if (dead_before >= 1) cout << 'b';
      int alive_len = end - start + 1;
      if (alive_len > 1) cout << alive_len;
      cout << 'o';
      cur = end + 1;
      i = j;
    }
    // Do not emit trailing dead cells in the row

    // Determine upcoming empty rows to compress separators
    if (r < last_non_empty) {
      int e = 0;
      int rr = r + 1;
      while (rr <= last_non_empty && rows[rr].empty()) { ++e; ++rr; }
      int total_sep = 1 + e; // one for next row + empties between
      if (total_sep > 1) cout << total_sep;
      cout << '$';
      r = rr; // skip empties and move to next non-empty
    } else {
      ++r; // last row printed
    }
  }

  cout << '!' << '\n';
}

inline int GetLiveCell() {
  return static_cast<int>(live_cells.size());
}
