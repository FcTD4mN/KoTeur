#pragma once


#include "Core.ECS.Core.Component.h"


#include <SFML/Graphics.hpp>
#include <tinyxml2.h>


namespace nCore {
namespace nECS {
namespace nComponent {


class cDirection :
    public ::nCore::nECS::nCore::cComponentGeneric
{
public:
    typedef  ::nCore::nECS::nCore::cComponentGeneric  tSuperClass;

public:
    virtual  ~cDirection();
    cDirection();
    cDirection( float iX, float iY );
    cDirection( const sf::Vector2f& iVector );
    cDirection( const cDirection& iRHS );

public:
    // Copy
    virtual  ::nCore::nECS::nCore::cComponent* Clone() const override;

public:
    // Access / Set
    double X();
    double Y();

    void X( double iX );
    void Y( double iY );

    sf::Vector2f AsVector2F();
    void SetUsingVector( const sf::Vector2f& iVector );
};


} // namespace nComponent
} // namespace nECS
} // namespace nCore

