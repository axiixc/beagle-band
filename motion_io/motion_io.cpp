#include <node.h>
#include <string>
#include "motion_io.h"

using namespace v8;

Persistent<Function> MotionIO::constructor;

void MotionIO::RegisterModule(Handle<Object> exports, Handle<Object> module)
{
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("MotionIO"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    constructor = Persistent<Function>::New(tpl->GetFunction());

    module->Set(String::NewSymbol("exports"), constructor);
}

MotionIO::MotionIO(Local<String> address, Local<Function> callback)
: m_address(*v8::String::Utf8Value(address))
, m_callback(Persistent<Function>::New(callback))
{
}

MotionIO::~MotionIO() {}

void MotionIO::BeginReceivingMotionUpdates()
{
    InvokeCallback(m_address);
}

void MotionIO::StopReceivingMotionUpdates()
{
}

void MotionIO::InvokeCallback(std::string data)
{
    HandleScope scope;

    const unsigned argc = 1;
    Local<Value> argv[argc] = { String::New(data.c_str()) };
    m_callback->Call(Context::GetCurrent()->Global(), argc, argv);
}

Handle<Value> MotionIO::New(const Arguments& args)
{
    HandleScope scope;

    if (!args.IsConstructCall())
    {
        const int argc = 2;
        Local<Value> argv[argc] = { args[0], args[1] };

        return scope.Close(constructor->NewInstance(argc, argv));
    }

    if (args[0]->IsUndefined())
    {
         return ThrowException(Exception::TypeError(
            String::New("Address cannot be undefined")));
    }

    if (!args[1]->IsFunction())
    {
        return ThrowException(Exception::TypeError(
            String::New("Callback function requires as second argument")));
    }

    Local<String> address = args[0]->ToString();
    Local<Function> callback = Local<Function>::Cast(args[1]);
    MotionIO* obj = new MotionIO(address, callback);
    obj->Wrap(args.This());
    obj->BeginReceivingMotionUpdates();

    return args.This();
}

void RegisterModule(Handle<Object> exports, Handle<Object> module)
{
    MotionIO::RegisterModule(exports, module);
}

NODE_MODULE(motion_io, MotionIO::RegisterModule);
