#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <sys/types.h>
#include <vector>
#include <iostream>

using std::shared_ptr;
using std::string;
using std::vector;
using std::filesystem::path;

/**
 * @brief List of all subsystems in REGS
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 */
enum class SUBSYSTEMS {
  SYSTEMS,
  GOES,
  QFH,
  YAGI,
  ADSB,
  ATC
};

inline string to_string(SUBSYSTEMS sys) {
  switch(sys) {
    case SUBSYSTEMS::SYSTEMS:
      return "Systems";
    case SUBSYSTEMS::GOES:
      return "GOES";
    case SUBSYSTEMS::QFH:
      return "QFH";
    case SUBSYSTEMS::YAGI:
      return "Yagi";
    case SUBSYSTEMS::ADSB:
      return "ADS-B";
    case SUBSYSTEMS::ATC:
      return "ATC";
    default:
      return "INVALID SUBSYSTEM";
  }
}

/**
 * @brief Type of documents which can be parsed
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 */
enum DOCTYPE { 
  WORD_XML ///< .docx documents
};

class document {
  vector<shared_ptr<document>> references;
  path file;
  SUBSYSTEMS subsys;
  unsigned revision;
  string document_name;
  public:
    document(path, vector<shared_ptr<document>> = {});

    document(const document&) = default;
    document(document&&) = default;
    document& operator=(const document&) = default;
    document& operator=(document&&) = default;

    bool getFileNameInfo();

    string docname() const {
      return document_name;
    }

    string filename() const {
      return file.filename();
    }

    template<DOCTYPE T>
    vector<string> parseReferences() const;

    void printInfo() const;

    bool addReference(document& doc);
    bool addReference(shared_ptr<document> doc);

    bool operator==(const document& other) const {
      return other.docname() == docname() && other.revision == revision;
    }

};

