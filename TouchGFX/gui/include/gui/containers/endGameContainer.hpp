#ifndef ENDGAMECONTAINER_HPP
#define ENDGAMECONTAINER_HPP

#include <gui_generated/containers/endGameContainerBase.hpp>

class endGameContainer : public endGameContainerBase
{
public:
    endGameContainer();
    virtual ~endGameContainer() {}

    virtual void initialize();
protected:
};

#endif // ENDGAMECONTAINER_HPP
