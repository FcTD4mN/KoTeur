#pragma once

#include "Core.ECS.Core.System.h"

#include <SFML/Graphics.hpp>


namespace nCore {
namespace nECS {
namespace nSystem {


class cBehaviourTreeSystem :
    public ::nCore::nECS::nCore::cSystem
{
public:
    typedef  cSystem  tSuperClass;

public:
    virtual  ~cBehaviourTreeSystem();
    cBehaviourTreeSystem();
    cBehaviourTreeSystem( const cBehaviourTreeSystem& iSys );

    // Overrides
public:
    virtual  cSystem* Clone() const override;

public:
    // Update/Draw
    virtual  void  Update( unsigned int iDeltaTime ) override;

public:
    // Entity
    virtual  void  IncomingEntity( ::nCore::nECS::nCore::cEntity* iEntity ) override;

};


} // namespace nSystem
} // namespace nECS
} // namespace nCore

