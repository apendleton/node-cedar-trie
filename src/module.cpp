#include "cedar_binding/binding.hpp"
#include <nan.h>

static void init(v8::Local<v8::Object> target) {
    // expose CedarTrie class
    cedar_trie::CedarSet::Init(target);
}

NODE_MODULE(module, init) // NOLINT