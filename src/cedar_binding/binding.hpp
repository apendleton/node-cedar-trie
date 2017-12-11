#pragma once
#include <nan.h>

#define USE_EXACT_FIT 1
#include "../cedar/cedarpp.h"

namespace cedar_trie {

using value_type = signed char;
using map_type = cedar::da<value_type, -1, -2, true>;

/**
     * CedarTrie class
     * This is in a header file so we can access it across other .cpp files if necessary
     * Also, this class adheres to the rule of Zero because we define no custom destructor or copy constructor
     */
class CedarSet : public Nan::ObjectWrap {

  public:
    // initializer
    static void Init(v8::Local<v8::Object> target);

    // methods required for the V8 constructor (?)
    static NAN_METHOD(New);
    static Nan::Persistent<v8::Function>& create_once();

    static NAN_METHOD(add);
    static NAN_METHOD(delete_);
    static NAN_METHOD(has);
    static NAN_METHOD(vomit);

  protected:
    cedar::da<signed char, -1, -2, true> trie;

  private:
    explicit CedarSet() = default;
    // member variable
    // specific to each instance of the class
};

class CedarSetIterator : public Nan::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> target);

    static NAN_METHOD(New);
    static Nan::Persistent<v8::Function>& create_once();

    //static NAN_METHOD(Next);

    CedarSetIterator(CedarSet* cs_);

    // non copyable/movable
    CedarSetIterator(CedarSetIterator const&) = delete;
    CedarSetIterator& operator=(CedarSetIterator const&) = delete;
    CedarSetIterator(CedarSetIterator&&) = delete;
    CedarSetIterator& operator=(CedarSetIterator&&) = delete;

  private:
    ~CedarSetIterator();
    CedarSet* cs;
    Nan::Persistent<v8::Object> persistentRef;
    size_t from (0), num (0), pos (0), p (0);
    char key[1024] ({0});
    union { int i; value_type x; } b;
};

} // namespace object_sync