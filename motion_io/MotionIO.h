#ifndef MOTIONIO_H
#define MOTIONIO_H

#include <node.h>
#include <string>
#include "ObservableIO.h"

using namespace v8;

class MotionIO : public node::ObjectWrap, IOEventObserver {
public:
    static void RegisterModule(Handle<Object>, Handle<Object>);

    virtual void IOSourceDidUpdate(IOEventSource&, std::string s) { InvokeCallback(s); }

private:
    explicit MotionIO(Local<String>, Local<Function>);
    ~MotionIO();

    void BeginReceivingMotionUpdates();
    void StopReceivingMotionUpdates();

    void InvokeCallback(std::string);

    static Handle<Value> New(const Arguments&);
    static Persistent<Function> constructor;

    std::string m_address;
    Persistent<Function> m_callback;
    IOEventSource* m_eventSource;
};

#endif // MOTIONIO_H