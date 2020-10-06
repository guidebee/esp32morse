//
// Created by Jing SHEN on 19/9/20.
//

#ifndef ESP32MORSE_SCREEN_HPP
#define ESP32MORSE_SCREEN_HPP

#include "oled_display.hpp"
#include <list>

#define MAX_LINE_NUMBER 50
#define NUMBER_CHARS_ONE_LINE 20
#define MAX_VIEW_POINT_LINE 8
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define OFFSET_X 4

class Screen {
private:
    OledDisplay *_display;
    int _start_page;
    int _end_page;
    int _row_no = 0;
    int _column_no = 0;
    bool _invert_text = false;
    char _buffer[MAX_LINE_NUMBER][NUMBER_CHARS_ONE_LINE];
    char _view_port[MAX_VIEW_POINT_LINE][NUMBER_CHARS_ONE_LINE];
    char _line[NUMBER_CHARS_ONE_LINE];
    int _old_cursor_x;
    int _old_cursor_y;
    bool _has_cursor = false;
    int _offset_x = OFFSET_X;

public:
    explicit Screen(OledDisplay *display, int start_page, int end_page,
                    bool inverted_text, bool has_cursor = false, int offset_x = OFFSET_X);

    void print(char c);

    void print(const String &s);

    void print(const char str[]);

    bool backspace();

    void clearScreen();

    void startScrollRight();

    void startScrollLeft();

    void stopScroll();

private:
    void process_char(char c);

    void saveCursor();

    void restoreCursor();

    void drawCursor();

    void clearScreenIfNeeded();
};


class ExtraDisplayListener {
public:
    virtual void drawExtra() = 0;
};


class DynamicScreen : public Screen {
private:
    std::string _displayMessage;
    unsigned long _last_time;
    int _remaining_period;
    std::list<int> _periods = std::list<int>();
    std::list<int> _stored_pattern;
    bool _show = false;
    bool _reload = true;
    ExtraDisplayListener *_displayListener = nullptr;

public:
    explicit DynamicScreen(OledDisplay *display, int start_page, int end_page,
                           bool inverted_text) : Screen(display, start_page, end_page, inverted_text) {
        _last_time = millis();
        _remaining_period = 0;
    }

    void loop();

    void displayText(std::string text, std::list<int> pattern, bool reload = true);

    void addDrawListener(ExtraDisplayListener *displayListener);

};

#endif //ESP32MORSE_SCREEN_HPP
