#include "MotionIO.h"

#include "Accelerometers/ADXL345.h"
#include <iostream>

using namespace v8;

Persistent<Function> MotionIO::constructor;

void MotionIO::RegisterModule(Handle<Object> exports)
{
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("MotionIO"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    constructor = Persistent<Function>::New(tpl->GetFunction());

    exports->Set(String::NewSymbol("MotionIO"), constructor);
}

MotionIO::MotionIO(Local<String> address, Local<Function> callback)
: m_address(*v8::String::Utf8Value(address))
, m_callback(Persistent<Function>::New(callback))
, m_eventSource(0)
{
}

MotionIO::~MotionIO()
{
    m_callback.Dispose();
}

void MotionIO::BeginReceivingMotionUpdates()
{
    if (m_eventSource)
        return;

    m_eventSource = new ADXL345(this);
}

void MotionIO::StopReceivingMotionUpdates()
{
    if (!m_eventSource)
        return;

    delete m_eventSource;
    m_eventSource = 0;
}

void MotionIO::InvokeCallback(std::string data)
{
    HandleScope scope;

    const unsigned argc = 1;
    Local<Value> argv[argc] = { Local<Value>::New(String::New(data.c_str())) };
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

void RegisterModule(Handle<Object> exports)
{
    MotionIO::RegisterModule(exports);
}

NODE_MODULE(motion_io, MotionIO::RegisterModule);
