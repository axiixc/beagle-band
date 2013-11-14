#ifndef OBSERVABLEIO_H
#define OBSERVABLEIO_H

#include <string>

class IOEventSource;

class IOEventObserver {
public:
    virtual void IOSourceDidUpdate(IOEventSource&, std::string);
};

class IOEventSource {
public:
    IOEventSource(IOEventObserver* observer)
    : m_observer(observer)
    {
    }

protected:
    inline void PostCallback(std::string s) { if (m_observer) m_observer->IOSourceDidUpdate(*this, s); }

private:
    IOEventObserver* m_observer;
};

#endif // OBSERVABLEIO_H