#pragma once

#include <cstddef>
#include <filesystem>
#include <iterator>
#include <fstream>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <iostream>
#include <queue>
#include <stack>
#include <iterator>
#include <type_traits>
#include <set>
#include <map>

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
      using reference = std::conditional_t<Const, const document&, document&>;
      using pointer = std::conditional_t<Const, const value_type, value_type>;
      private:
      std::set<value_type> visited;
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

      explicit _iterator(value_type root) : cur(root) {
        visited = std::set<value_type>();
        cont = Container();
        if( !root.get() )
          return;
        
        visited.insert(root);

        for( auto ref : root->references ){
          if( visited.find(ref.getDoc()) == visited.end() ){
            cont.push(ref.getDoc());
          }
        }
        //if( !root->references.empty() ){
        //  cont.insert(cont.begin(), root->references.begin(), root->references.end());
        //}
      }

      _iterator() = delete;
      _iterator(const _iterator&) = default;
      _iterator(const _iterator&& rhs) : visited(std::move(rhs.visited)), cont(std::move(rhs.cont)), cur(std::move(rhs.cur)) {}
      _iterator& operator=(const _iterator&) = default;
      _iterator& operator=(const _iterator&& rhs) {
        visited = std::move(rhs.visited);
        cont = std::move(rhs.cont);
        cur = std::move(rhs.cur);
        return *this;
      }

      template<bool wasConst, class = std::enable_if<Const && ! wasConst>>
      _iterator(const _iterator<wasConst, TYPE> &rhs) : cont(rhs.cont), cur(rhs.cur) {}

      template<bool wasConst, class = std::enable_if<Const && ! wasConst>>
      _iterator(const _iterator<wasConst, TYPE> &&rhs) : cont(std::move(rhs.cont)), cur(std::move(rhs.cur)) {}

      template<bool wasConst, class =  std::enable_if<Const && !wasConst>>
      _iterator& operator=(const _iterator<wasConst, TYPE>& rhs) {
        visited = rhs.visited;
        cont = rhs.cont;
        cur = rhs.cur;
        return *this;
      }
      
      template<bool wasConst, class =  std::enable_if<Const && !wasConst>>
      _iterator& operator=(const _iterator<wasConst, TYPE>&& rhs) {
        visited = std::move(rhs.visited);
        cont = std::move(rhs.cont);
        cur = std::move(rhs.cur);
        return *this;
      }

      friend class _iterator<!Const, TYPE>;

      pointer operator->() { return cur; }
      reference operator*() { return *cur; }

      bool operator==(const _iterator &other) const { return cur.get() == other.cur.get(); }
      bool operator!=(const _iterator &other) const { return cur.get() != other.cur.get(); }

      _iterator& operator++() {
        if( cont.empty() ) { cur = nullptr; return *this; }

        cur = pop(); 

        for( auto ref : cur->references ) {
          if( visited.find(ref.getDoc()) == visited.end() ){
            cont.push(ref.getDoc());
            visited.insert(ref.getDoc());
          }
        }
        return *this;
      }

      _iterator operator++(int) {
        _iterator result(*this);
        if( cont.empty() ) { cur = nullptr; return result; }
        
        cur = pop();

        for( auto ref : cur->references )
          if( visited.find(ref.getDoc()) == visited.end() ) {
            cont.push(ref);
            visited.insert(ref.getDoc());
          }
        return result;
      }


    };
  public:
    using DFSiterator = _iterator<false, DFS>;
    using constDFSiterator = _iterator<true, DFS>;
    using BFSiterator = _iterator<false, BFS>;
    using constBFSiterator = _iterator<true, BFS>;


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
    DFSiterator dfsbegin(size_t idx) { return DFSiterator(docs.at(idx)); }
    DFSiterator dfsend() { return DFSiterator(nullptr); }
    constDFSiterator cdfsbegin(size_t idx) const { return constDFSiterator(docs.at(idx)); }
    constDFSiterator cdfsend() const { return constDFSiterator(nullptr); }

    BFSiterator bfsbegin(size_t idx) { return BFSiterator(docs.at(idx)); }
    BFSiterator bfsend() { return BFSiterator(nullptr); }
    constBFSiterator cbfsbegin(size_t idx) const { return constBFSiterator(docs.at(idx)); }
    constBFSiterator cbfsend() const { return constBFSiterator(nullptr); }
    
    vector<shared_ptr<document>> getDoc(string, decltype(string::npos)=3); 

    size_t size() const {
      return docs.size();
    }

    bool empty() const {
      return docs.empty();
    }

    const shared_ptr<document> getChild(size_t) const;

    const vector<uint8_t> serialize() const;
    
    friend std::istream& operator>>(std::istream&,docgraph&);
    friend std::ostream& operator<<(std::ostream&,docgraph&);
};
