#ifndef ICOMMANDHANDLERBRIDGE_HPP
#define ICOMMANDHANDLERBRIDGE_HPP


class ICommandHandlerBridge {
public:
    virtual bool canDoCommand(int cmd) const = 0;
    virtual void handleCommand(int cmd) = 0;

protected:
    virtual ~ICommandHandlerBridge() { }
};


#endif
