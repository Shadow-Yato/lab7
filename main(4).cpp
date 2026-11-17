/*********************************
 * Автор:   Михайлов Даниил      *
 * Задание: Структуры и файлы    *
 * Вариант: 15                   *
 *********************************/
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Album {
  string artist;
  string title;
  int year;
  double price;
};

vector<string> splitFields(const string& line) {
  vector<string> fields;
  size_t start = 0;
  size_t separator;
  bool isLast = false;

  while (!isLast) {
    separator = line.find(';', start);
    if (separator == string::npos) {
      fields.push_back(line.substr(start));
      isLast = true;
    } else {
      fields.push_back(line.substr(start, separator - start));
      start = separator + 1;
    }
  }

  return fields;
}

bool readInt(const string& text, int& value) {
  istringstream parser(text);
  char extra;

  if (!(parser >> value)) {
    return false;
  }

  return !(parser >> extra);
}

bool readDouble(const string& text, double& value) {
  istringstream parser(text);
  char extra;

  if (!(parser >> value) || !isfinite(value)) {
    return false;
  }

  return !(parser >> extra);
}

bool readAlbums(const string& path, vector<Album>& albums) {
  int maxAlbumCount = 100;
  int minYear = 1900;
  int maxYear = 2026;
  int fieldCount = 4;
  ifstream input(path);
  string line;
  vector<string> fields;
  Album current;
  int lineNumber = 0;
  bool isValid;

  if (!input) {
    cout << "ERROR open\n";
    return false;
  }

  while (getline(input, line)) {
    ++lineNumber;
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1);
    }

    fields = splitFields(line);
    isValid = static_cast<int>(fields.size()) == fieldCount &&
              !fields[0].empty() && !fields[1].empty() &&
              readInt(fields[2], current.year) &&
              readDouble(fields[3], current.price) &&
              current.year >= minYear && current.year <= maxYear &&
              current.price >= 0.0 &&
              static_cast<int>(albums.size()) < maxAlbumCount;
    if (!isValid) {
      cout << "ERROR line " << lineNumber << "\n";
      return false;
    }

    current.artist = fields[0];
    current.title = fields[1];
    albums.push_back(current);
  }

  if (input.bad()) {
    cout << "ERROR read\n";
    return false;
  }

  return true;
}

bool findNewestAlbum(const vector<Album>& albums, const string& artist,
                     size_t& resultIndex) {
  bool isFound = false;

  for (size_t albumIndex = 0; albumIndex < albums.size(); ++albumIndex) {
    if (albums[albumIndex].artist == artist &&
        (!isFound || albums[albumIndex].year > albums[resultIndex].year)) {
      resultIndex = albumIndex;
      isFound = true;
    }
  }

  return isFound;
}

bool findMostExpensiveAlbum(const vector<Album>& albums, size_t& resultIndex) {
  bool isFound = false;

  for (size_t albumIndex = 0; albumIndex < albums.size(); ++albumIndex) {
    if (!isFound || albums[albumIndex].price > albums[resultIndex].price) {
      resultIndex = albumIndex;
      isFound = true;
    }
  }

  return isFound;
}

string buildReport(const vector<Album>& albums, const string& artist) {
  ostringstream report;
  size_t newestIndex = 0;
  size_t expensiveIndex = 0;
  bool isNewestFound;
  bool isExpensiveFound;

  isNewestFound = findNewestAlbum(albums, artist, newestIndex);
  isExpensiveFound = findMostExpensiveAlbum(albums, expensiveIndex);

  report << "Albums of " << artist << ":\n";
  if (!isNewestFound) {
    report << "EMPTY\n";
  }
  for (size_t albumIndex = 0; albumIndex < albums.size(); ++albumIndex) {
    if (albums[albumIndex].artist == artist) {
      report << albums[albumIndex].title << ", "
             << albums[albumIndex].year << ", "
             << albums[albumIndex].price << "\n";
    }
  }

  if (isNewestFound) {
    report << "Newest album: " << albums[newestIndex].title
           << " (" << albums[newestIndex].year << ")\n";
  } else {
    report << "Newest album: NONE\n";
  }

  if (isExpensiveFound) {
    report << "Most expensive album: " << albums[expensiveIndex].artist
           << " - " << albums[expensiveIndex].title
           << ", " << albums[expensiveIndex].price;
  } else {
    report << "Most expensive album: NONE";
  }

  return report.str();
}

bool writeReport(const string& path, const string& text) {
  ofstream output(path);

  if (!output) {
    cout << "ERROR output\n";
    return false;
  }

  output << text;
  output.flush();
  if (!output) {
    cout << "ERROR write\n";
    return false;
  }

  return true;
}

int main() {
  string inputPath;
  string outputPath;
  string artist;
  vector<Album> albums;
  string reportText;

  cout << "Enter input file path: ";
  getline(cin, inputPath);
  cout << "Enter output file path: ";
  getline(cin, outputPath);
  cout << "Enter artist: ";
  getline(cin, artist);

  if (!readAlbums(inputPath, albums)) {
    return 1;
  }

  reportText = buildReport(albums, artist);

  if (!writeReport(outputPath, reportText)) {
    return 1;
  }
  cout << reportText;

  return 0;
}
