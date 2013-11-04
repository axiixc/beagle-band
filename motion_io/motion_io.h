#ifndef MOTION_IO_H
#define MOTION_IO_H

#include <node.h>
#include <string>

using namespace v8;

class MotionIO : public node::ObjectWrap {
public:
    static void RegisterModule(Handle<Object>, Handle<Object>);

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
};

#endif // MOTION_IO_H