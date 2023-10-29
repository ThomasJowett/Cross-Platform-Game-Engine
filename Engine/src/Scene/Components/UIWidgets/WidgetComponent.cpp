#include "WidgetComponent.h"

void WidgetComponent::SetAnchorLeft(float left)
{
	anchorLeft = left;

	if (anchorRight < left) {
		anchorRight = left;
		marginRight = (position.x + size.x) - (anchorRight * s_referenceWidth);
	}

	marginLeft = position.x - (anchorLeft * s_referenceWidth);
}

void WidgetComponent::SetAnchorRight(float right)
{
	anchorRight = right;

	if (anchorLeft > right) {
		anchorLeft = right;
		marginLeft = position.x - (anchorLeft * s_referenceWidth);
	}

	marginRight = (position.x + size.x) - (anchorRight * s_referenceWidth);
}

void WidgetComponent::SetAnchorTop(float top)
{
	anchorTop = top;
	if (anchorBottom < top) {
		anchorBottom = top;
		marginBottom = position.y + size.y - (anchorBottom * s_referenceHeight);
	}

	marginTop = position.x - (anchorTop * s_referenceHeight);
}

void WidgetComponent::SetAnchorBottom(float bottom)
{
	anchorBottom = bottom;

	if (anchorTop > bottom) {
		anchorTop = bottom;
		marginTop = (anchorTop * s_referenceHeight) + position.x;
	}

	marginBottom = position.y + size.y - (anchorBottom * s_referenceHeight);
}

void WidgetComponent::SetMarginLeft(float left)
{
	marginLeft = left;

	position.x = (anchorLeft * s_referenceWidth) + marginLeft;
	if (!fixedWidth)
		size.x = ((anchorRight *s_referenceWidth) + marginRight) - ((anchorLeft * s_referenceWidth) + marginLeft);
	else
		marginRight = marginLeft + size.x;
}

void WidgetComponent::SetMarginTop(float top)
{
	marginTop = top;
	position.y = (anchorTop * s_referenceHeight) + marginTop;
	if (!fixedHeight)
		size.y = ((anchorBottom * s_referenceHeight) + marginBottom) - ((anchorTop * s_referenceWidth) + marginTop);
	else
		marginBottom = marginTop + size.y;
}

void WidgetComponent::SetMarginRight(float right)
{
	marginRight = right;
	if (!fixedWidth)
		size.x = ((anchorRight * s_referenceWidth) + marginRight) - ((anchorLeft * s_referenceWidth) + marginLeft);
	else {
		position.x = (anchorRight * s_referenceWidth) + marginRight - size.x;
		marginLeft = marginRight - size.x;
	}
}

void WidgetComponent::SetMarginBottom(float bottom)
{
	marginBottom = bottom;
	if (!fixedHeight) {
		size.y = ((anchorBottom * s_referenceHeight) + marginBottom) - ((anchorTop * s_referenceWidth) + marginTop);
	}
	else {
		position.y = (anchorTop * s_referenceHeight) + marginTop - size.y;
		marginTop = marginBottom - size.y;
	}
}

void WidgetComponent::SetPositionX(float x)
{
	position.x = x;

	marginLeft = position.x - (anchorLeft * s_referenceWidth);
	marginRight = marginLeft + size.x;

}

void WidgetComponent::SetPositionY(float y)
{
	position.y = y;

	marginTop = position.y - (anchorTop * s_referenceHeight);
	marginBottom = marginTop + size.y;
}

void WidgetComponent::SetSizeX(float sizeX)
{
	size.x = sizeX;

	marginRight = marginLeft + size.x;
}

void WidgetComponent::SetSizeY(float sizeY)
{
	size.y = sizeY;
	
	marginBottom = marginTop + size.x;
}
