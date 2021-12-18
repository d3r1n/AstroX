#ifndef BUTTON_INCLUDED

#define BUTTON_INCLUDED
#include "raylib.h"

class CustomButton {
	public:

		// Position and the size of the button
		Vector2 position; 	// position of the button (x,y)
		float scale;		// scale of the button

		// Texture and Text inside it
		Texture2D texture; 	// texture of the button
		const char* text; 	// text of the button (placed in the middle of the button)
		int fontSize; 		// font size of the text
		Color textColor; 	// color of the text

		// Constructors
		CustomButton() = default;
		CustomButton(Vector2 position, float scale, Texture2D texture, const char* text, int fontSize, Color textColor);
		
		// Setters
		void setPosition(Vector2 position);
		void setScale(float scale);
		void setTexture(Texture2D texture);
		void setText(const char* text);
		void setFontSize(int fontSize);
		void setTextColor(Color textColor);

		// State getters
		bool isHovered(); 	// check if the mouse is hovering over the button
		bool isClicked(); 	// check if the button is clicked


		// Drawing the button
		void Draw(); // draw the button

};

#endif