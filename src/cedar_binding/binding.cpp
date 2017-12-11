#include "binding.hpp"
#include <iostream>

namespace cedar_trie {

using value_type = signed char;
using map_type = cedar::da<value_type, -1, -2, true>;

// Triggered from Javascript world when calling "new CedarSet(name)"
NAN_METHOD(CedarSet::New) {
    if (info.IsConstructCall()) {
        try {
            auto* const self = new CedarSet();
            self->Wrap(info.This()); // Connects C++ object to Javascript object (this)
        } catch (const std::exception& ex) {
            return Nan::ThrowTypeError(ex.what());
        }

        info.GetReturnValue().Set(info.This());
    } else {
        return Nan::ThrowTypeError(
            "Cannot call constructor as function, you need to use 'new' keyword");
    }
}

NAN_METHOD(CedarSet::add) {
    auto* self = Nan::ObjectWrap::Unwrap<CedarSet>(info.Holder());

    if (info.Length() >= 1) {
        if (info[0]->IsString()) {
            Nan::Utf8String utf8_value(info[0]);
            int len = utf8_value.length();

            self->trie.update(*utf8_value, len, 0);
        } else {
            return Nan::ThrowTypeError("Only strings can be added");
        }
    } else {
        return Nan::ThrowTypeError("An argument is required");
    }
}

NAN_METHOD(CedarSet::delete_) {
    auto* self = Nan::ObjectWrap::Unwrap<CedarSet>(info.Holder());

    if (info.Length() >= 1) {
        if (info[0]->IsString()) {
            Nan::Utf8String utf8_value(info[0]);
            int len = utf8_value.length();

            int found = self->trie.erase(*utf8_value, len, 0);
            std::cout << found << std::endl;
            info.GetReturnValue().Set(Nan::New<v8::Boolean>(found));
        } else {
            return Nan::ThrowTypeError("Only strings can be deleted");
        }
    } else {
        return Nan::ThrowTypeError("An argument is required");
    }
}

NAN_METHOD(CedarSet::has) {
    auto* self = Nan::ObjectWrap::Unwrap<CedarSet>(info.Holder());

    if (info.Length() >= 1) {
        if (info[0]->IsString()) {
            Nan::Utf8String utf8_value(info[0]);
            int len = utf8_value.length();

            int found = self->trie.exactMatchSearch<value_type>(*utf8_value, len) != -1;
            std::cout << found << std::endl;
            info.GetReturnValue().Set(Nan::New<v8::Boolean>(found));
        } else {
            return Nan::ThrowTypeError("Only strings can be checked");
        }
    } else {
        return Nan::ThrowTypeError("An argument is required");
    }
}

NAN_METHOD(CedarSet::vomit) {
    auto* self = Nan::ObjectWrap::Unwrap<CedarSet>(info.Holder());

    size_t from (0), num (0), pos (0), p (0);
    char key[1024] = {0};
    union { int i; value_type x; } b;
    for (b.i = self->trie.begin (from, p); b.i != map_type::CEDAR_NO_PATH; b.i = self->trie.next (from, p)) {
        self->trie.suffix (key, p, from);
        std::cout << key << std::endl;
    }
}

// This is a Singleton, which is a general programming design concept for
// creating an instance once within a process.
Nan::Persistent<v8::Function>& CedarSet::create_once() {
    static Nan::Persistent<v8::Function> init;
    return init;
}

void CedarSet::Init(v8::Local<v8::Object> target) {
    Nan::HandleScope scope;

    // Officially create the CedarSet
    auto fnTp = Nan::New<v8::FunctionTemplate>(
        CedarSet::New); // Passing the CedarSet::New method above
    fnTp->InstanceTemplate()->SetInternalFieldCount(
        1);                     // It's 1 when holding the ObjectWrap itself and nothing else
    fnTp->SetClassName(Nan::New("CedarSet").ToLocalChecked()); // Passing the Javascript string object above


    SetPrototypeMethod(fnTp, "add", add);
    SetPrototypeMethod(fnTp, "delete", delete_);
    SetPrototypeMethod(fnTp, "has", has);
    SetPrototypeMethod(fnTp, "vomit", vomit);

    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
    create_once().Reset(fn);

    Nan::Set(target, Nan::New("CedarSet").ToLocalChecked(), fn);
}

NAN_METHOD(CedarSetIterator::New) {
    auto* const self = new CedarSetIterator(Nan::ObjectWrap::Unwrap<CedarSet>(info[0]));
    self->Wrap(info.This());
}

Nan::Persistent<v8::Function>& CedarSetIterator::create_once() {
    static Nan::Persistent<v8::Function> init;
    return init;
}

void CedarSetIterator::Init(v8::Local<v8::Object> target) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("CedarSetIterator").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    //SetPrototypeMethod(tpl, "next", Next);

    create_once().Reset(Nan::GetFunction(tpl).ToLocalChecked());
}

CedarSetIterator::CedarSetIterator(CedarSet* cs_) {
    cs = cs_;
    persistentRef.Reset(cs_->handle());
}
CedarSetIterator::~CedarSetIterator() {
    persistentRef.Reset();
}

} // namespace object_sync