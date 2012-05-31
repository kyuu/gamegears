#ifndef COMMANDHANDLERBRIDGE_HPP
#define COMMANDHANDLERBRIDGE_HPP

#include "ICommandHandlerBridge.hpp"


template<typename commandHandlerT>
class CommandHandlerBridge : public ICommandHandlerBridge {
public:
    explicit CommandHandlerBridge(commandHandlerT* commandHandler) : _commandHandler(commandHandler) { }
    ~CommandHandlerBridge() { }

    // ICommandHandlerBridge implementation
    bool canDoCommand(int cmd) const {
        return _commandHandler->canDoCommand(cmd);
    }

    void handleCommand(int cmd) {
        _commandHandler->handleCommand(cmd);
    }

private:
    commandHandlerT* _commandHandler;
};


#endif
