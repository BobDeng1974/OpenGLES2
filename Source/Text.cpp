#include "Text.h"
#include "GameConstants.h"
#include "ResourceManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;

using namespace ci;

Text::Text( Font* font, std::string text ) :
	mFont( font ),
	letterSpacing( 0 ),
	lineSpacing( 0 ),
	width( 0 ),
	height( 0 ),
	scale( 1.0f ),
	truncateWithDots( false ),
	padding( 0 )
{
	mColor = Vec4f::one();
	mMesh = ResourceManager::get()->getMesh( "models/quad_plane.obj" );
	setText( text );
}

Text::~Text()
{
	// This will delete the elements in the list:
	clearCharacters();
}

void Text::clearCharacters()
{
	// Clean up pointers and clear
	for( const auto c : mCharacters ) {
		delete c;
	}
	mCharacters.clear();
}

void Text::setText( const std::string& text )
{
	// Start with a fresh crop of new characters
	clearCharacters();
	
	mPenPosition = Vec2i( 0, -mFont->defaultLineHeight * 0.6f );
	
	for( int i = 0; i < text.length(); i++ ) {
		// Inser the first character
		insertCharacter( text[i] );
		
		// Check end of line according to 'width' property
		bool lineWidthExceeded = width > 0 && mPenPosition.x * scale > width - padding * 2;
		if ( lineWidthExceeded ) {
			
			// Check max lines according to 'height' property
			bool lineHeightExceeded = height > 0 && ( mPenPosition.y + mFont->defaultLineHeight ) * scale > height - padding * 2;
			if ( lineHeightExceeded ) {
				if ( truncateWithDots ) {
					// Go back to the most recent space
					revertToLastSpace( text, i );
					// Add the dots
					insertCharacter( '.' );
					insertCharacter( '.' );
					insertCharacter( '.' );
				}
				
				// Out of space, so stop adding characters
				return;
			}
			else {
				// Go back to the most recent space
				revertToLastSpace( text, i );
				// Insert new line
				insertCharacter( '\n' );
				// And add the next character
				insertCharacter( text[i] );
			}
		}
	}
}

void Text::removeLastCharacter()
{
	// Reset the pen position to where it was before the character was added
	mPenPosition.x -= mCharacters.back()->xadvance + letterSpacing;
	
	// Removal it and clean up, since the Character pointer is copied from the font
	delete mCharacters.back();
	mCharacters.pop_back();
}

void Text::insertCharacter( const char c )
{
	// Check for special characters
	if ( c == '\n' ) {
		// New line by offseting mPenPosition
		mPenPosition.x = 0;
		mPenPosition.y += mFont->defaultLineHeight + lineSpacing;
	}
	
	else if ( Font::Character* character = mFont->getCharacterCopy( c ) ) {
		// Adjust character display properties according to Text settings
		character->paddingOffset = Vec2i( padding, padding );
		character->scale = scale;
		character->setPosition( mPenPosition.x, mPenPosition.y );
		character->visible = true;
		
		// Add it to the list for rendering
		mCharacters.push_back( character );
		
		// Update current position of where new characters will be added
		mPenPosition.x += character->xadvance + letterSpacing;
	}
}

int Text::getCharacterWidth( const char c )
{
	// Returns the width of the character stored in the font
	// Note: Does not use 'getCharacterCopy' method here
	if ( Font::Character* character = mFont->getCharacter( c ) ) {
		return character->size.x;
	}
	return 0;
}

void Text::revertToLastSpace( const std::string& text, int& i )
{
	// Iterate back until we hit a space, removing characters and resetting i from the loop
	while( ( text[i] != ' ' && text[i] != '-' ) && i >= 0 ) {
		removeLastCharacter();
		i--;
	}
	// If the current character is now a space, remove it as well and iterate forward so
	// that the space doesn't get added again.
	if ( text[i] == ' ' ) {
		removeLastCharacter();
		i++;
	}
}