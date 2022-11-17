#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <filesystem>
#include <sys/types.h>
#include <vector>
#include <algorithm>
#include <iostream>


using std::shared_ptr;
using std::string;
using std::vector;
using std::filesystem::path;

// Declare graph here. 
class docgraph;

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
  WORD_XML, ///< .docx documents
  INVALID,  /// All unparseable documents
};

// This Combination Should Be Unique Within A Graph 
struct DOCID {
  uint8_t SUBSYS;
  uint8_t REVISION;
  std::string NAME;

  DOCID() = default;
  DOCID(uint8_t s, uint8_t r, std::string n) :SUBSYS(s), REVISION(r), NAME(n) {}
  DOCID(const DOCID&) = default;

  DOCID& operator=(const DOCID&) = default;

  bool operator==(const DOCID&) const = default;
  bool operator<(const DOCID& rhs) const {
    return SUBSYS < rhs.SUBSYS || SUBSYS < rhs.SUBSYS || NAME != rhs.NAME; 
  }

  friend std::istream& operator>>(std::istream&, DOCID&);
  friend std::ostream& operator<<(std::ostream&, DOCID);
};

class document;

class reference {
  private:
    shared_ptr<document> doc;
    string ref_string;
    DOCID id; // Id of document, in place of it 

    friend std::istream& operator>>(std::istream&, docgraph&);
    friend std::ostream& operator<<(std::ostream&, docgraph&);
  public:
    reference() = default;

    reference(shared_ptr<document> d, string s, DOCID i)
      : doc(d), ref_string(s), id(i) {}

    reference(shared_ptr<document> d)
      : doc(d), ref_string("") {}

    bool operator==(reference& other) {
      return doc == other.doc && id == other.id;
    }

    shared_ptr<document> getDoc() const {
      return doc;
    }

    string const& getRef() const {
      return ref_string;
    }

    DOCID getId() const {
      return id;
    }
    
    friend std::istream& operator>>(std::istream& is, reference& c);
    friend std::ostream& operator<<(std::ostream& os, reference& c);
};

class document {
  vector<reference> references;
  vector<string> unfound_references;
  vector<string> parsed_references;
  path file;
  SUBSYSTEMS subsys;
  unsigned revision;
  string document_name;

  friend class docgraph;
  friend std::istream& operator>>(std::istream&, docgraph&);
  friend std::ostream& operator<<(std::ostream&, docgraph&);

  // Documents Found While Parsing From A Bytestream
  vector<reference> __stream_documents;
  public:
    
    // Used for DFS/BFS algorithms
    bool visited;
    
    explicit document() = default;
    document(path, vector<reference> = {});

    document(const document&) = default;
    document(document&&) = default;
    document& operator=(const document&) = default;
    document& operator=(document&&) = default;

    bool getFileNameInfo();
    DOCID getDocID() const {
      return DOCID((uint8_t)subsys, (uint8_t)revision, document_name);
    }

    string docname() const {
      return document_name;
    }

    string filename() const {
      return file.filename();
    }

    template<DOCTYPE T>
    vector<string> parseReferences() const;

    const vector<string>& getParsedReferences() const {
      return parsed_references;
    }

    void parseReferences();

    void printInfo() const;

    bool addReference(shared_ptr<document> doc, string refString);
    void addReference(string ref){
      std::transform(ref.begin(), ref.end(), ref.begin(), ::tolower);
      unfound_references.push_back(ref);
    }

    bool hasReference(shared_ptr<document>) const;
    bool hasReference(const document*) const;

    bool hasUnfoundReference(string) const;

    bool operator==(const document& other) const {
      return other.docname() == docname() && other.revision == revision;
    }
  
    friend std::istream& operator>>(std::istream& is, document& c);
    friend std::ostream& operator<<(std::ostream& os, document& c);
};


