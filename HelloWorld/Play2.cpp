#include "Play2.h"

void Play2::DrawFontTextWithScale(const char* fontId, std::string text, Play::Point2D pos, Play::Align justify, float scale)
{
	// find the total width of the text
	int font = Play::Graphics::GetSpriteId(fontId);
	int totalWidth{ 0 };
	for (char c : text)
	{
		totalWidth += Play::Graphics::GetFontCharWidth(font, c) * scale;
	}

	// set the horizontal position based on the alignment
	switch (justify)
	{
	case Play::Align::CENTRE:
		pos.x -= totalWidth / 2;
		break;
	case Play::Align::RIGHT:
		pos.x -= totalWidth;
		break;
	default:
		break;
	}

	pos.x += Play::Graphics::GetSpriteOrigin(font).x * scale; // take into account sprite origin
	pos.x -= 3; // HACK - to make the text look centred on the buttons
	DrawStringWithScale(font, TRANSFORM_SPACE(pos), text, scale);
}

void Play2::DrawStringWithScale(int fontId, Play::Point2f pos, std::string text, float scale)
{
	// draw each character in the correct position by keeping track of the total width of the text
	int width = 0;
	for (char c : text)
	{
		Play::Graphics::DrawRotated(fontId, { pos.x + width, pos.y }, c - 32, 0, scale);
		width += Play::Graphics::GetFontCharWidth(fontId, c) * scale;
	}
}

// uses the collision radius to check if the object is leaving the display area
bool Play2::IsHitboxLeavingDisplayArea(Play::GameObject& obj)
{
	int spriteID = obj.spriteId;
	Play::Vector2f hitbox = { obj.radius, obj.radius };
	Play::Point2f pos = TRANSFORM_SPACE(obj.pos);

	// check left side
	if (pos.x - hitbox.width < 0 && obj.velocity.x < 0)
	{
		return true;
	}

	// check right side
	if (pos.x + hitbox.width > Play::Window::GetWidth() && obj.velocity.x > 0)
	{
		return true;
	}

	// check bottom
	if (pos.y - hitbox.height < 0 && obj.velocity.y < 0)
	{
		return true;
	}

	// check top
	if (pos.y + hitbox.height > Play::Window::GetHeight() && obj.velocity.y > 0)
	{
		return true;
	}

	return false;
}

bool Play2::RevertPositionIfNecessary(Play::GameObject& object)
{
	// set the object's position back to the old position if it has been updated to an invalid position.
	Play2 play2 = Play2();
	if (play2.IsHitboxLeavingDisplayArea(object)
		|| HasBlockingCollision(object))
	{
		object.pos = object.oldPos;
		return true;
	}

	return false;
}
bool Play2::HasBlockingCollision(Play::GameObject& object)
{
	// check if the object is colliding with the environment (e.g. fences)
	std::vector<int> environmentIds = Play::CollectGameObjectIDsByType(TYPE_ENVIRONMENT);
	for (int environmentId : environmentIds)
	{
		Play::GameObject& obj_environment = Play::GetGameObject(environmentId);
		if (Play::IsColliding(object, obj_environment))
		{
			return true;
		}
	}
	return false;
}

void Play2::InitDisplaySize()
{
	// Get a handle to the user's desktop window
	const HWND hDesktop = GetDesktopWindow();

	// the top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates for the screen size
	RECT desktop;
	GetWindowRect(hDesktop, &desktop);
	int x = desktop.right;
	int y = desktop.bottom;

	// make the game bigger if the screen resolution is large enough, leaving extra room so game isn't completely maximised.
	int extraRoom = 10;
	int displayScale = std::min((x - extraRoom) / DISPLAY_WIDTH, (y - extraRoom) / DISPLAY_HEIGHT);
	if (displayScale < 1)
	{
		displayScale = 1;
	}
	DISPLAY_SCALE = displayScale;
}