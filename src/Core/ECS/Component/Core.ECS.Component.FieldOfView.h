#pragma once


#include "Core.ECS.Core.Component.h"


#include <tinyxml2.h>


namespace nCore {
namespace nECS {
namespace nComponent {


class cFieldOfView :
    public ::nCore::nECS::nCore::cComponentGeneric
{
public:
    typedef  ::nCore::nECS::nCore::cComponentGeneric  tSuperClass;

public:
    virtual  ~cFieldOfView();
    cFieldOfView();
    cFieldOfView( double iAngle, double iDistance );
    cFieldOfView( const cFieldOfView& iRHS );

public:
    // Copy
    virtual  ::nCore::nECS::nCore::cComponent* Clone() const override;

public:
    // Access / Set
    double Angle();
    double Distance();

    void Angle( double iAngle );
    void Distance( double iDistance );
};


} // namespace nComponent
} // namespace nECS
} // namespace nCore

