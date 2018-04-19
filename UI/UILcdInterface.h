#ifndef UI_LCD_INTERFACE_H_
#define UI_LCD_INTERFACE_H_

#include <Print.h>

class UILcdInterface : public Print {
public:
	UILcdInterface() {}
	virtual ~UILcdInterface() {}

	virtual void clear() = 0;
	virtual void setCursor(int, int) = 0;
	virtual void noBacklight() = 0;
	virtual void backlight() = 0;
	virtual size_t write(uint8_t) = 0;
};


// We use generics (`template`) to support any kind of LCD that has the required methods.
template <typename LCD_TYPE>
class UILcdInterfaceImpl : public UILcdInterface {
	LCD_TYPE *lcd;
	
public:

	UILcdInterfaceImpl(LCD_TYPE *lcd) : UILcdInterface(), lcd(lcd) {}
	virtual ~UILcdInterfaceImpl() {}
	
	virtual void clear() override {
		this->lcd->clear();
	}
	virtual void setCursor(int a, int b) override {
		this->lcd->setCursor(a, b);
	}
	virtual void noBacklight() override {
		this->lcd->noBacklight();
	}
	virtual void backlight() override {
		this->lcd->backlight();
	}
	virtual size_t write(uint8_t c) override {
		return this->lcd->write(c);
	}
};

#endif /* UI_LCD_INTERFACE_H_ */

