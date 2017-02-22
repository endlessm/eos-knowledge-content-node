#include "domain.h"
#include "gobject.h"

#include <node.h>
#include <node_buffer.h>
#include <eos-knowledge-content.h>

using namespace v8;

void DomainReadURI(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate ();

    GObject *domain = GObjectFromWrapper (args[0]);
    String::Utf8Value uri (args[1]->ToString ());

    const char *mime = NULL;
    GBytes *bytes = NULL;
    GError *error = NULL;

    if (!eknc_domain_read_uri (EKNC_DOMAIN (domain), *uri, &bytes, &mime, &error)) {
        isolate->ThrowException (Exception::Error (String::NewFromUtf8 (isolate, error->message)));
        return;
    }

    Local<Object> module_obj = Object::New (isolate);
    if (mime)
        module_obj->Set (String::NewFromUtf8 (isolate, "mime_type"), String::NewFromUtf8 (isolate, mime));
    if (bytes) {
        size_t length = 0;
        char * data = (char *)g_bytes_unref_to_data (bytes, &length);
        MaybeLocal<Object> buffer = node::Buffer::New(isolate, data, length);
        module_obj->Set (String::NewFromUtf8 (isolate, "data"), buffer.ToLocalChecked ());
    }
    args.GetReturnValue ().Set (module_obj);
}
