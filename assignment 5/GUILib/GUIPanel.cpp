#include "GUIPanel.h"
#include "GUIControl.h"
#include <InterfaceEvents.h>
#include <EventSystem.h>

GUIPanel::GUIPanel()
{
	EventSystem* pEventSystem = EventSystem::getInstance();
	pEventSystem->addListener(Event::MOUSE_MOVE_EVENT, this);
	pEventSystem->addListener(Event::MOUSE_BUTTON_EVENT, this);
	pEventSystem->addListener(Event::KEYBOARD_EVENT, this);
}

void GUIPanel::clear()
{
	for (unsigned int i = 0; i < mpControls.size(); i++)
	{
		delete mpControls[i];
	}
	mpControls.clear();
}

void GUIPanel::addControl(GUIControl* pControl)
{
	mpControls.push_back(pControl);
}

void GUIPanel::draw(GraphicsBuffer* dest)
{
	for (unsigned int i = 0; i < mpControls.size(); i++)
	{
		mpControls[i]->draw(dest);
	}
}

void GUIPanel::press(const ControlID& id)
{
	for (unsigned int i = 0; i < mpControls.size(); i++)
	{
		if (mpControls[i]->getID() == id)
		{
			mpControls[i]->setPressed();
		}
	}
}

void GUIPanel::release(const ControlID& id)
{
	for (unsigned int i = 0; i < mpControls.size(); i++)
	{
		if (mpControls[i]->getID() == id)
		{
			mpControls[i]->setReleased();
		}
	}
}

void GUIPanel::hover(const ControlID& id)
{
	for (unsigned int i = 0; i < mpControls.size(); i++)
	{
		if (mpControls[i]->getID() == id)
		{
			mpControls[i]->setHover();
		}
	}
}

void GUIPanel::handleEvent(const Event& theEvent)
{
	//const InterfaceEvent& iEvent = static_cast<const InterfaceEvent&>(theEvent);
	//needs to be a listener
	//heres all the controls, detect whatever (like hover)
	//call the callback from here

	//TODO: implement me!!!
	if (theEvent.getType() == Event::MOUSE_BUTTON_EVENT)
	{
		const MouseButtonEvent& iEvent = static_cast<const MouseButtonEvent&>(theEvent);
		for (unsigned int i = 0; i < mpControls.size(); i++) //pressed or released?
		{
			Vector2D loc = mpControls[i]->getLoc();
			Vector2D size = mpControls[i]->getSize();
			if (loc.getX() - (size.getX()) < iEvent.getLocation().getX() && iEvent.getLocation().getX() < loc.getX() + (size.getX()) && 
				(loc.getY() - (size.getY()) < iEvent.getLocation().getY() && iEvent.getLocation().getY() < loc.getY() + (size.getY())))
			{
				if (iEvent.getPressedState() == InputSystem::PRESSED)
				{
					mpControls[i]->onClick("pressed");
				}
				else if (iEvent.getPressedState() == InputSystem::RELEASED)
				{
					mpControls[i]->onClick("released");
				}
			}
		}
	}
	else if (theEvent.getType() == Event::MOUSE_MOVE_EVENT) //hover?
	{
		const MouseMoveEvent& iEvent = static_cast<const MouseMoveEvent&>(theEvent);
		for (unsigned int i = 0; i < mpControls.size(); i++)
		{
			Vector2D loc = mpControls[i]->getLoc();
			Vector2D size = mpControls[i]->getSize();
			if (loc.getX() - (size.getX()) < iEvent.getLocation().getX() && iEvent.getLocation().getX() < loc.getX() + (size.getX()) && 
				(loc.getY() - (size.getY()) < iEvent.getLocation().getY() && iEvent.getLocation().getY() < loc.getY() + (size.getY())))
			{
				mpControls[i]->onClick("hover");
			}
		}
	}
	else if (theEvent.getType() == Event::KEYBOARD_EVENT)
	{

	}
}
