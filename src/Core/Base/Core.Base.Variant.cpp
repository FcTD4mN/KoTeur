#include "Core.Base.Variant.h"

#include <string>



namespace nCore {
namespace nBase {


cVariant::~cVariant()
{
}


cVariant::cVariant()
{
}


cVariant::cVariant( const cVariant & iRHS )
    //mValueChangeCallback( iRHS.mValueChangeCallback ) Don't copy CB, because std function can relate/use pointers specific to iRHS
{
    // Not much to do here
}


cVariant*
cVariant::MakeVariant( eType iType )
{
    switch( iType )
    {
        case kNumber:
            return  new cNumber( 0.0 );

        case kString:
            return  new cString( "" );
            break;
        case kBoolean:
            return  new cBoolean( false );
            break;

        default:
            printf( "Invalid variant type\n" );
            break;
    }

    return  0;
}


cVariant*
cVariant::MakeVariant( double iValue )
{
    return  new cNumber( iValue );
}


cVariant*
cVariant::MakeVariant( int iValue )
{
    return  new cNumber( double(iValue) );
}


cVariant*
cVariant::MakeVariant( const std::string& iValue )
{
    return  new cString( iValue );
}


cVariant *
cVariant::MakeVariant( bool iBoolean )
{
    return  new cBoolean( iBoolean );
}


// ==============================================================================================================
// ============================================================================================= Access / Getters
// ==============================================================================================================


eType
cVariant::Type() const
{
    return  kInvalid;
}


std::string
cVariant::TypeAsString() const
{
    return  "Invalid";
}


double
cVariant::GetValueNumber() const
{
    return  dynamic_cast< const cNumber* >( this )->Value();
}


const std::string&
cVariant::GetValueString() const
{
    return  dynamic_cast< const cString* >( this )->Value();
}


bool
cVariant::GetValueBool() const
{
    return  dynamic_cast< const cBoolean* >( this )->Value();
}


void
cVariant::SetValueNumber( double iValue )
{
    dynamic_cast< cNumber* >( this )->Value( iValue );
}


void
cVariant::SetValueString( const std::string& iValue )
{
    dynamic_cast< cString* >( this )->Value( iValue );
}


void
cVariant::SetValueBool( bool iValue )
{
    dynamic_cast< cBoolean* >( this )->Value( iValue );
}


// ==============================================================================================================
// =============================================================================================== Input / Output
// ==============================================================================================================


void
cVariant::AddValueChangeCallback( std::function< void( eVariableState ) > iFunction )
{
    mValueChangeCallback.push_back( std::pair< tHash, std::function< void( eVariableState ) > >( std::hash< std::string >{}( "__NONE__" ), iFunction ) );
}


void
cVariant::AddValueChangeCallbackAndAssociateID( std::function< void( eVariableState ) > iFunction, const std::string& iID )
{
    mValueChangeCallback.push_back( std::pair< tHash, std::function< void( eVariableState ) > >( std::hash< std::string >{}( iID ), iFunction ) );
}


void
cVariant::RemoveCallbackByID( const std::string & iID )
{
    tHash hashedID = std::hash< std::string >{}( iID );

    for( int i = 0; i < mValueChangeCallback.size(); ++i )
    {
        auto pair = mValueChangeCallback[ i ];
        if( pair.first == hashedID )
        {
            mValueChangeCallback.erase( mValueChangeCallback.begin() + i );
            break;
        }
    }
}


bool
cVariant::CallbackByIDExist( const std::string & iID )
{
    tHash hashedID = std::hash< std::string >{}( iID );

    for( int i = 0; i < mValueChangeCallback.size(); ++i )
    {
        auto pair = mValueChangeCallback[ i ];
        if( pair.first == hashedID )
            return  true;
    }

    return  false;
}


void
cVariant::SaveXML( tinyxml2::XMLElement* iNode, tinyxml2::XMLDocument* iDocument )
{
    iNode->SetAttribute( "type", Type() );
}


void
cVariant::LoadXML( tinyxml2::XMLElement* iNode )
{
    // Empty
}


cVariant*
cVariant::MakeFromXML( tinyxml2::XMLElement * iNode )
{
    eType type = eType( iNode->IntAttribute( "type", 0 ) );
    cVariant* loadedVar = 0;
    switch( type )
    {
        case eType::kNumber:
            loadedVar = MakeVariant( kNumber );
            loadedVar->LoadXML( iNode );
            break;
        case eType::kString:
            loadedVar = MakeVariant( kString );
            loadedVar->LoadXML( iNode );
            break;
        case eType::kBoolean:
            loadedVar = MakeVariant( kBoolean );
            loadedVar->LoadXML( iNode );
            break;

        default:
            break;
    }

    return  loadedVar;
}



// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================
// ==============================================================================================================



cNumber::~cNumber()
{
}


cNumber::cNumber( double iValue ) :
    tSuperClass(),
    mValue( iValue )
{
}


cNumber::cNumber( const cNumber & iRHS ) :
    tSuperClass( iRHS ),
    mValue( iRHS.mValue )
{
}


// ==============================================================================================================
// ======================================================================================================= Makers
// ==============================================================================================================


cNumber *
cNumber::Clone()
{
    return  new  cNumber( *this );
}


// ==============================================================================================================
// ======================================================================================================= Access
// ==============================================================================================================


double
cNumber::Value() const
{
    return  mValue;
}


void
cNumber::Value( double iValue )
{
    for( auto& pair : mValueChangeCallback )
        pair.second( eVariableState::kBeforeChange );

    mValue = iValue;

    for( auto& pair : mValueChangeCallback )
        pair.second( eVariableState::kAfterChange );
}


eType
cNumber::Type() const
{
    return  kNumber;
}


std::string
cNumber::TypeAsString() const
{
    return  "Number";
}


std::string
cNumber::ToString() const
{
    return  std::to_string( mValue );
}


// ==============================================================================================================
// =============================================================================================== Input / Output
// ==============================================================================================================


void
cNumber::SaveXML( tinyxml2::XMLElement* iNode, tinyxml2::XMLDocument* iDocument )
{
    tSuperClass::SaveXML( iNode, iDocument );
    iNode->SetAttribute( "value", mValue );
}


void
cNumber::LoadXML( tinyxml2::XMLElement* iNode )
{
    tSuperClass::LoadXML( iNode );
    mValue = iNode->DoubleAttribute( "value", 0.0 );
}


cVariant&
cNumber::operator=( const cVariant & iRHS )
{
    if( iRHS.Type() == kNumber )
        Value( iRHS.GetValueNumber() );
    else if( iRHS.Type() == kString )
        Value( std::stoi( iRHS.GetValueString() ) );
    else if( iRHS.Type() == kBoolean )
        Value( iRHS.GetValueBool() );

    return  *this;
}



// ===================================
// ===================================
// ===================================



cString::~cString()
{
}

cString::cString( const std::string & iValue ) :
    tSuperClass(),
    mValue( iValue )
{
}


cString::cString( const cString& iRHS ) :
    tSuperClass( iRHS ),
    mValue( iRHS.mValue )
{
}


// ==============================================================================================================
// ======================================================================================================= Makers
// ==============================================================================================================


cString *
cString::Clone()
{
    return  new  cString( *this );
}


// ==============================================================================================================
// ======================================================================================================= Access
// ==============================================================================================================


const std::string&
cString::Value() const
{
    return  mValue;
}


void
cString::Value( const std::string & iValue )
{
    for( auto& pair : mValueChangeCallback )
        pair.second( eVariableState::kBeforeChange );

    mValue = iValue;

    for( auto& pair : mValueChangeCallback )
        pair.second( eVariableState::kAfterChange );
}


eType
cString::Type() const
{
    return  kString;
}


std::string
cString::TypeAsString() const
{
    return  "String";
}


std::string
cString::ToString() const
{
    return  mValue;
}


// ==============================================================================================================
// =============================================================================================== Input / Output
// ==============================================================================================================


void
cString::SaveXML( tinyxml2::XMLElement* iNode, tinyxml2::XMLDocument* iDocument )
{
    tSuperClass::SaveXML( iNode, iDocument );
    iNode->SetAttribute( "value", mValue.c_str() );
}


void
cString::LoadXML( tinyxml2::XMLElement* iNode )
{
    tSuperClass::LoadXML( iNode );
    mValue = iNode->Attribute( "value" );
}


cVariant&
cString::operator=( const cVariant & iRHS )
{
    if( iRHS.Type() == kNumber )
        Value( std::to_string( iRHS.GetValueNumber() ) );
    else if( iRHS.Type() == kString )
        Value( iRHS.GetValueString() );
    else if( iRHS.Type() == kBoolean )
        Value( std::to_string( iRHS.GetValueBool() ) );

    return  *this;
}



// ===================================
// ===================================
// ===================================



cBoolean::~cBoolean()
{
}

cBoolean::cBoolean( bool iValue ) :
    tSuperClass(),
    mValue( iValue )
{
}


cBoolean::cBoolean( const cBoolean& iRHS ) :
    tSuperClass( iRHS ),
    mValue( iRHS.mValue )
{
}


// ==============================================================================================================
// ======================================================================================================= Makers
// ==============================================================================================================


cBoolean *
cBoolean::Clone()
{
    return  new  cBoolean( *this );
}


// ==============================================================================================================
// ======================================================================================================= Access
// ==============================================================================================================


bool
cBoolean::Value() const
{
    return  mValue;
}


void
cBoolean::Value( bool iValue )
{
    for( auto& pair : mValueChangeCallback )
        pair.second( eVariableState::kBeforeChange );

    mValue = iValue;

    for( auto& pair : mValueChangeCallback )
        pair.second( eVariableState::kAfterChange );
}


eType
cBoolean::Type() const
{
    return  kBoolean;
}


std::string
cBoolean::TypeAsString() const
{
    return  "Boolean";
}


std::string
cBoolean::ToString() const
{
    return  mValue ? "true" : "false";
}


// ==============================================================================================================
// =============================================================================================== Input / Output
// ==============================================================================================================


void
cBoolean::SaveXML( tinyxml2::XMLElement* iNode, tinyxml2::XMLDocument* iDocument )
{
    tSuperClass::SaveXML( iNode, iDocument );
    iNode->SetAttribute( "value", mValue );
}


void
cBoolean::LoadXML( tinyxml2::XMLElement* iNode )
{
    tSuperClass::LoadXML( iNode );
    mValue = iNode->BoolAttribute( "value", false );
}


cVariant&
cBoolean::operator=( const cVariant & iRHS )
{
    if( iRHS.Type() == kNumber )
        Value( iRHS.GetValueNumber() );
    else if( iRHS.Type() == kString )
        Value( false );
    else if( iRHS.Type() == kBoolean )
        Value( iRHS.GetValueBool() );

    return  *this;
}


} // namespace nBase
} // namespace nCore

