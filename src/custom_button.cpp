#include "custom_button.hpp"

CustomButton::CustomButton(Vector2 position, float scale, Texture2D texture, const char* text, int fontSize, Color textColor) {
	this->position = position;
	this->scale = scale;
	this->texture = texture;
	this->text = text;
	this->fontSize = fontSize;
	this->textColor = textColor;
}

void CustomButton::setPosition(Vector2 position) {
	this->position = position;
}

void CustomButton::setScale(float scale) {
	this->scale = scale;
}

void CustomButton::setTexture(Texture2D texture) {
	this->texture = texture;
}

void CustomButton::setText(const char* text) {
	this->text = text;
}

void CustomButton::setFontSize(int fontSize) {
	this->fontSize = fontSize;
}

void CustomButton::setTextColor(Color textColor) {
	this->textColor = textColor;
}

bool CustomButton::isHovered() {
	Vector2 mouse_location = GetMousePosition();
	if (mouse_location.x > position.x && mouse_location.x < position.x + texture.width * scale &&
		mouse_location.y > position.y && mouse_location.y < position.y + texture.height * scale) {
		return true;
	}
	return false;
}

bool CustomButton::isClicked() {
	if (isHovered()) {
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			return true;
		}
	}
	return false;
}

void CustomButton::Draw() {
	DrawTextureEx(texture, position, 0.0f, scale, WHITE);
	DrawText(text, position.x + ((texture.width * scale) / 2) - MeasureText(text, fontSize) / 2, (position.y + texture.height * scale / 2) - fontSize / 2, fontSize, textColor);
}