//
// Created by Jing SHEN on 19/9/20.
//

#include "screen.hpp"

Screen::Screen(OledDisplay *display, int start_page, int end_page, bool inverted_text = false) {
    _display = display;
    _start_page = start_page;
    _end_page = end_page;
    _invert_text = inverted_text;
    _old_cursor_x = OFFSET_X;
    _old_cursor_y = CHAR_HEIGHT * start_page;
}

void Screen::backspace() {
    int x = _display->getCursorX();
    int y = _display->getCursorY();

    if (x > 0) {
        x -= CHAR_WIDTH;
        _display->setCursor(x, y);
        if (_invert_text) {
            _display->fillRect(x, y, CHAR_WIDTH, CHAR_HEIGHT, WHITE);
        } else {
            _display->fillRect(x, y, CHAR_WIDTH, CHAR_HEIGHT, BLACK);
        }

        _display->display();
    }
}

void Screen::clearScreen() {
    _old_cursor_x = OFFSET_X;
    _old_cursor_y = CHAR_HEIGHT * _start_page;
    int x = _old_cursor_x;
    int y = _old_cursor_y;
    _display->setCursor(x, y);
    if (_invert_text) {
        _display->fillRect(0, y, SCREEN_WIDTH, CHAR_HEIGHT * (_end_page - _start_page ), WHITE);
    } else {
        _display->fillRect(0, y, SCREEN_WIDTH, CHAR_HEIGHT * (_end_page - _start_page ), BLACK);
    }
    _display->display();
}

void Screen::process_char(char c) {

}

void Screen::saveCursor() {
    _old_cursor_x = _display->getCursorX();
    _old_cursor_y = _display->getCursorY();

}

void Screen::restoreCursor() {
    if(_old_cursor_y>=_end_page*CHAR_HEIGHT){
        clearScreen();
    }
    _display->setCursor(_old_cursor_x, _old_cursor_y);
    if (_invert_text) {
        _display->setTextColor(BLACK, WHITE);
    } else {
        _display->setTextColor(WHITE);
    }
}

void Screen::print(char c) {
    restoreCursor();
    _display->print(c);
    process_char(c);
    _display->display();
    saveCursor();
}

void Screen::print(const String &s) {
    restoreCursor();
    _display->print(s);
    for (int i = 0; i < s.length(); i++) {
        process_char(s.charAt(i));
    }
    _display->display();
    saveCursor();
}

void Screen::print(const char str[]) {
    restoreCursor();
    _display->print(str);
    for (int i = 0; i < strlen(str); i++) {
        process_char(str[i]);
    }
    _display->display();
    saveCursor();
}


void Screen::startScrollRight(){
    _display->startscrollright(_start_page,_end_page-1);
}
void Screen::startScrollLeft(){
    _display->startscrollleft(_start_page,_end_page-1);
}
void Screen::stopScroll(){
    _display->stopscroll();

}