#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <vector>
#include <iostream>

using std::shared_ptr;
using std::string;
using std::vector;
using std::filesystem::path;


class document {
  vector<shared_ptr<document>> references;
  path file;
  public:
    document(path, vector<shared_ptr<document>> = {});

    document(const document&) = default;
    document(document&&) = default;
    document& operator=(const document&) = default;
    document& operator=(document&&) = default;


    string docname() const {
      return file.filename();
    }

    void printInfo() const;

    bool addReference(document& doc);
    bool addReference(shared_ptr<document> doc);

    bool operator==(const document& other) const {
      return other.docname() == docname();
    }

};

