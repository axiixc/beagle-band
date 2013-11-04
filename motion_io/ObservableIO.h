#ifndef OBSERVABLEIO_H
#define OBSERVABLEIO_H

class IOEventSource;

class IOEventObserver {
public:
    virtual void IOSourceDidUpdate(IOEventSource&, std::string);
};

class IOEventSource {
public:
    IOEventSource(IOEventObserver& callback)
    : m_observer(callback)
    {
    }

protected:
    inline void PostCallback(std::string s) { m_observer.IOSourceDidUpdate(*this, s); }

private:
    IOEventObserver& m_observer;
};

#endif // OBSERVABLEIO_H