#include "Core.ECS.Core.Entity.h"


#include "Core.ECS.Core.Component.h"
#include "Core.ECS.Core.World.h"
#include "Core.ECS.Core.System.h"

#include "Core.Render.Layer.h"

#include "Core.Registries.ComponentRegistry.h"


namespace nCore {
namespace nECS {
namespace nCore {


static unsigned int sgEntityCount = 0;


// -------------------------------------------------------------------------------------
// ------------------------------------------------------------ Construction/Destruction
// -------------------------------------------------------------------------------------


cEntity::~cEntity()
{
    for( auto it = mComponents.begin(); it != mComponents.end(); ++it )
        delete  it->value;
}


cEntity::cEntity() :
    mWorld( 0 ),
    mContainerLayer( 0 ),
    mID( "idontknow" + std::to_string( sgEntityCount ) ),
    mComponents(),
    mTags(),
    mObserverSystems(),
    mLoaded( false ),
    mDead( false ),
    mIDForHandles( 1 )
{
    mComponents.reserve( 16 );
    mTags.reserve( 64 );
    ++sgEntityCount;
}


cEntity::cEntity( const cEntity& iEntity ) :
    mWorld( iEntity.mWorld ),
    mContainerLayer( iEntity.mContainerLayer ),
    mID( iEntity.mID + "c" + std::to_string( sgEntityCount ) ),
    mComponents(),                                  // This will need to get copied
    mTags( iEntity.mTags ),                         // This can be copied, as there is no pointer
    mObserverSystems( iEntity.mObserverSystems ),   // Same systems will observe this entity
    mLoaded( iEntity.mLoaded ),
    mDead( iEntity.mDead ),
    mIDForHandles( iEntity.mIDForHandles + 1 )
{
    mComponents.reserve( iEntity.mComponents.size() );
    mTags.reserve( iEntity.mTags.size() );
    for( auto it = iEntity.mComponents.begin(); it != iEntity.mComponents.end(); ++it )
    {
        sPair pair;
        pair.key = it->key;
        pair.value = it->value->Clone();
        mComponents.push_back( pair );
    }
    ++sgEntityCount;

    // We add the clone to the world and layer of the original
    if( mWorld )
        mWorld->AddEntityAndPutInLayer( this, mContainerLayer );
}


// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------- Copy
// -------------------------------------------------------------------------------------


cEntity*
cEntity::Clone() const
{
    return  new cEntity( *this );
}


void
cEntity::LeaveAllSystems()
{
    for( int i = 0; i < mObserverSystems.size(); ++i )
        mObserverSystems[ i ]->EntityLost( this );

    mObserverSystems.clear();
}


// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------- Components
// -------------------------------------------------------------------------------------


void
cEntity::AddComponent( ::nCore::nECS::nCore::cComponent * iComponent )
{
    sPair pair;
    pair.key = iComponent->Name();
    pair.value = iComponent;
    mComponents.push_back( pair );

    if( mLoaded )
    {
        LeaveAllSystems();
        mWorld->UpdateWorldWithEntity( this );
    }

    IncIDForHandles();
}


void
cEntity::SetComponent( cComponent * iComponent )
{
    for( int i = 0; i < mComponents.size(); ++i )
    {
        if( mComponents[ i ].key == iComponent->Name() )
        {
            delete  mComponents[ i ].value;
            mComponents[ i ].value = iComponent;
        }
    }

    //IncIDForHandles(); // Should we invalid handle in this case ? or just if component count changed ?
}


void
cEntity::RemoveComponent( cComponent * iComponent )
{
    for( int i = 0; i < mComponents.size(); ++i )
    {
        if( mComponents[ i ].key == iComponent->Name() && mComponents[ i ].value == iComponent )
            mComponents.erase( mComponents.begin() + i );
    }

    if( mLoaded )
    {
        LeaveAllSystems();
        mWorld->UpdateWorldWithEntity( this );
    }

    delete  iComponent;

    IncIDForHandles();
}


void
cEntity::RemoveComponentByName( const std::string & iComponentName )
{
    ::nCore::nECS::nCore::cComponent* component = 0;
    for( int i = 0; i < mComponents.size(); ++i )
    {
        if( mComponents[i].key == iComponentName )
        {
            component = mComponents[i].value;
            mComponents.erase( mComponents.begin() + i );
        }
    }

    if( mLoaded )
    {
        LeaveAllSystems();
        mWorld->UpdateWorldWithEntity( this );
    }

    delete  component;

    IncIDForHandles();
}


cComponent*
cEntity::GetComponentByName( const std::string & iComponentName )
{
    for( int i = 0; i < mComponents.size(); ++i )
    {
        if( mComponents[ i ].key == iComponentName )
            return  mComponents[ i ].value;
    }

    return  0;
}


cComponent*
cEntity::GetComponentAtIndex( int iIndex )
{
    return  mComponents[ iIndex ].value;
}


void
cEntity::RemoveComponentAtIndex( int iIndex )
{
    ::nCore::nECS::nCore::cComponent* component = mComponents[ iIndex ].value;

    if( mLoaded )
    {
        LeaveAllSystems();
        mComponents.erase( mComponents.begin() + iIndex );
        mWorld->UpdateWorldWithEntity( this );
    }
    else // Yes erase is done in both cases, but first case require it to be
    {
        mComponents.erase( mComponents.begin() + iIndex );
    }

    delete  component;

    IncIDForHandles();
}


unsigned int
cEntity::GetComponentCount() const
{
    return  (unsigned int)( mComponents.size() );
}


// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------- Tags
// -------------------------------------------------------------------------------------


void
cEntity::AddTag( const std::string & iTag )
{
    mTags.push_back( iTag );
    IncIDForHandles();
}


void
cEntity::RemoveTag( const std::string & iTag )
{
    for( int i = 0; i < mTags.size(); ++i )
    {
        if( mTags[ i ] == iTag )
            mTags.erase( mTags.begin() + i );
    }

    IncIDForHandles();
}


bool
cEntity::HasTag( const std::string & iTag )
{
    for( int i = 0; i < mTags.size(); ++i )
    {
        if( mTags[ i ] == iTag )
            return  true;
    }

    return  false;
}


const std::string&
cEntity::GetTagAtIndex( int iIndex )
{
    return  mTags[ iIndex ];
}


void
cEntity::SetTagAtIndex( int iIndex, const std::string& iTag )
{
    mTags[ iIndex ] = iTag;
}


unsigned int
cEntity::GetTagCount() const
{
    return  (unsigned int)( mTags.size() );
}


// -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------ Access
// -------------------------------------------------------------------------------------



::nCore::nRender::cLayer*
cEntity::Layer()
{
    return  mContainerLayer;
}


const ::nCore::nRender::cLayer*
cEntity::Layer() const
{
    return  mContainerLayer;
}


bool
cEntity::IsDead() const
{
    return  mDead;
}


void
cEntity::SetLoaded()
{
    mLoaded = true;
}


void
cEntity::SetWorld( cWorld * iWorld )
{
    mWorld = iWorld;
}


const std::string&
cEntity::ID() const
{
    return  mID;
}


bool
cEntity::SetID( const std::string & iID )
{
    if( mWorld && !mWorld->IsIDUnique( iID ) )
        return  false;

    mID = iID;
    return  true;
}


void
cEntity::Destroy()
{
    // To prevent unneeded multiple destructions
    if( mDead )
        return;

    LeaveAllSystems();
    mContainerLayer->RemoveEntity( this );

    mWorld->DestroyEntity( this );

    mDead = true;
    IncIDForHandles();
}


void
cEntity::AddSystemObserver( cSystem * iSystem )
{
    mObserverSystems.push_back( iSystem );
}


cEntityHandle
cEntity::GetHandle()
{
    return  cEntityHandle( this );
}


unsigned int
cEntity::GetIDForHandle() const
{
    return  mIDForHandles;
}


void
cEntity::DrawUsingObserverSystems( sf::RenderTarget * iRenderTarget )
{
    for( auto sys : mObserverSystems )
    {
        sys->DrawEntity( iRenderTarget, this );
    }
}


void
cEntity::IncIDForHandles()
{
    mIDForHandles = ( mIDForHandles + 1 ) % 99999;
}


// -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------ Input/Output
// -------------------------------------------------------------------------------------


void
cEntity::SaveXML( tinyxml2::XMLElement * iNode, tinyxml2::XMLDocument* iDocument ) const
{
    iNode->SetAttribute( "id", mID.c_str() );

    tinyxml2::XMLElement* components = iDocument->NewElement( "components" );

    for( auto it = mComponents.begin(); it != mComponents.end(); ++it )
    {
        // Thing is wherever you do a mComponents[ "something" ], it creates a key value pair : {"something", NULL}
        if( it->value )
        {
            tinyxml2::XMLElement* component = iDocument->NewElement( "component" );
            it->value->SaveXML( component, iDocument );
            components->LinkEndChild( component );
        }
    }

    iNode->LinkEndChild( components );

    tinyxml2::XMLElement* tags = iDocument->NewElement( "tags" );

    for( auto it = mTags.begin(); it != mTags.end(); ++it )
    {
        tinyxml2::XMLElement* tag = iDocument->NewElement( "tag" );
        tag->SetAttribute( "name", it->c_str() );
        tags->LinkEndChild( tag );
    }

    iNode->LinkEndChild( tags );
}


void
cEntity::LoadXML( tinyxml2::XMLElement * iNode )
{
    mID = iNode->Attribute( "id" );
    tinyxml2::XMLElement* components = iNode->FirstChildElement( "components" );

    for( tinyxml2::XMLElement* component = components->FirstChildElement( "component" ); component; component = component->NextSiblingElement() )
    {
        cComponent* comp = ::nCore::nRegistries::cComponentRegistry::Instance()->CreateComponentFromName( component->Attribute( "name" ) );
        if( comp )
        {
            comp->LoadXML( component );
            AddComponent( comp );
        }
        else
        {
            cComponentGeneric* genComp = new cComponentGeneric( component->Attribute( "name" ) );
            genComp->LoadXML( component );
            AddComponent( genComp );
        }
    }

    tinyxml2::XMLElement* tags = iNode->FirstChildElement( "tags" );

    for( tinyxml2::XMLElement* tag = tags->FirstChildElement( "tag" ); tag; tag = tag->NextSiblingElement() )
    {
        AddTag( tag->Attribute( "name" ) );
    }
}



// =================================================
// ===================== HANDLE ====================
// =================================================



cEntityHandle::~cEntityHandle()
{
}


cEntityHandle::cEntityHandle( cEntity* iEntity ) :
    mEntity( iEntity ),
    mHandleID( iEntity->GetIDForHandle() )
{
}


cEntity *
cEntityHandle::GetEntity()
{
    if( !IsHandleValid() && !SyncHandle() )
        return  0;

    return  mEntity;
}


bool
cEntityHandle::IsHandleValid()
{
    return  mEntity->GetIDForHandle() == mHandleID;
}


bool
cEntityHandle::SyncHandle()
{
    if( mEntity->IsDead() )
    {
        return  false;
    }
    else
    {
        mHandleID = mEntity->GetIDForHandle();
        return  true;
    }
}


} // namespace nCore
} // namespace nECS
} // namespace nCore
