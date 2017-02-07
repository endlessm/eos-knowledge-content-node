#include "engine.h"
#include "gobject.h"

#include <eos-knowledge-content.h>

using namespace v8;

struct CallbackData {
    Persistent<Promise::Resolver> resolver;
};

static void EngineGetObjectCallback(GObject *source,
                                    GAsyncResult *result,
                                    gpointer user_data)
{
    Isolate *isolate = Isolate::GetCurrent ();
    HandleScope scope(isolate);
    CallbackData *data = (CallbackData *)user_data;

    GError *error = NULL;
    EkncContentObjectModel *model;

    Local<Promise::Resolver> resolver = Local<Promise::Resolver>::New(isolate, data->resolver);
    if ((model = eknc_engine_get_object_finish (EKNC_ENGINE (source), result, &error)))
        resolver->Resolve(WrapperFromGObject (isolate, G_OBJECT (model)));
    else
        resolver->Reject(Exception::TypeError(String::NewFromUtf8(isolate, error->message)));

    /* XXX: Needed for the promises to resolve. It seems you should run microtasks
     * manually when you work with promises from C/C++ */
    isolate->RunMicrotasks();
    data->resolver.Reset();
    delete data;
}

void EngineGetObject(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate ();
    GObject *engine = GObjectFromWrapper (args.This ());
    String::Utf8Value id (args[0]->ToString ());
    String::Utf8Value app_id (args[1]->ToString ());

    Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate);
    Local<Promise> promise = resolver->GetPromise();
    CallbackData *data = new CallbackData();
    data->resolver.Reset(isolate, resolver);
    eknc_engine_get_object_for_app (EKNC_ENGINE (engine), *id, *app_id, NULL, (GAsyncReadyCallback)EngineGetObjectCallback, data);
    args.GetReturnValue ().Set (promise);
}

static void EngineQueryCallback(GObject *source,
                                GAsyncResult *result,
                                gpointer user_data)
{
    Isolate *isolate = Isolate::GetCurrent ();
    HandleScope scope(isolate);
    CallbackData *data = (CallbackData *)user_data;

    GError *error = NULL;
    EkncQueryResults *results;

    Local<Promise::Resolver> resolver = Local<Promise::Resolver>::New(isolate, data->resolver);
    if ((results = eknc_engine_query_finish (EKNC_ENGINE (source), result, &error)))
        resolver->Resolve(WrapperFromGObject (isolate, G_OBJECT (results)));
    else
        resolver->Reject(Exception::TypeError(String::NewFromUtf8(isolate, error->message)));

    /* XXX: Needed for the promises to resolve. It seems you should run microtasks
     * manually when you work with promises from C/C++ */
    isolate->RunMicrotasks();
    data->resolver.Reset();
    delete data;
}

void EngineQuery(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate ();
    GObject *engine = GObjectFromWrapper (args.This ());
    GObject *query = GObjectFromWrapper (args[0]);

    Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate);
    Local<Promise> promise = resolver->GetPromise();
    CallbackData *data = new CallbackData();
    data->resolver.Reset(isolate, resolver);
    eknc_engine_query (EKNC_ENGINE (engine), EKNC_QUERY_OBJECT (query), NULL, (GAsyncReadyCallback)EngineQueryCallback, data);
    args.GetReturnValue ().Set (promise);
}
