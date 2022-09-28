#pragma once

#include <filesystem>
#include <iterator>
#include <memory>
#include <string>
#include <iostream>
#include <queue>
#include <stack>
#include <iterator>
#include <type_traits>

#include "document.hpp"
#include "utils.hpp"

using std::filesystem::path;
using std::string;

class docgraph {
  private:
    friend class document;

    vector<shared_ptr<document>> docs; 
    enum iter_type {
      DFS, BFS
    };
  public:
    template<bool Const, iter_type TYPE>
    class _iterator {
      public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = shared_ptr<document>;
      using Container = std::conditional_t<TYPE == BFS, std::queue<value_type>, std::stack<value_type>>;
      using reference = std::conditional_t<Const, const value_type&, value_type&>;
      using pointer = std::conditional_t<Const, const value_type*, value_type*>;
      private:
      Container cont;
      shared_ptr<document> cur;

      template<bool isBFS = (TYPE == BFS)>
      std::enable_if_t<isBFS, value_type>
      pop() {
        value_type tmp = cont.front();
        cont.pop();
        return tmp;
      }

      template<bool isBFS = (TYPE == BFS)>
      std::enable_if_t<!isBFS, value_type>
      pop() {
        value_type tmp = cont.top();
        cont.pop();
        return tmp;
      }

      public:
      explicit _iterator(value_type root) {
        if( !root.get() )
          return;
        if( !root->references.empty() ){
          cont.insert(cont.begin(), root->references.begin(), root->references.end());
        }
      }



    };
  public:
    docgraph() = default;

    void scan_dir(path);

    /** Print information on all docs in the graph.
     */
    void printDocs() const {
      for( auto& doc : docs ){
        doc->printInfo();
        std::cout << std::endl;
      }
    }

    // Parse Each Document's References And Connect Them To Each Other
    void parseAndConnect();

    // Standard Iterators Over All Documents
    decltype(docs)::iterator begin() { return docs.begin(); }
    decltype(docs)::iterator end() { return docs.end(); }
    decltype(docs)::const_iterator cbegin() { return docs.cbegin(); }
    decltype(docs)::const_iterator cend() { return docs.cend(); }
    
    decltype(docs)::reverse_iterator rbegin() { return docs.rbegin(); }
    decltype(docs)::reverse_iterator rend() { return docs.rend(); }
    decltype(docs)::const_reverse_iterator crbegin() { return docs.crbegin(); }
    decltype(docs)::const_reverse_iterator crend() { return docs.crend(); }

    // DFS And BFS Iterators
    // TODO


    vector<shared_ptr<document>> getDoc(string, decltype(string::npos)=3); 

    size_t size() const {
      return docs.size();
    }

    bool empty() const {
      return docs.empty();
    }

    const shared_ptr<document> getChild(size_t) const;

};

