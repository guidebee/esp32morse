//
// Created by Jing SHEN on 19/9/20.
//

#include "screen.hpp"

Screen::Screen(OledDisplay *display, int start_page, int end_page, bool inverted_text = false, bool has_cursor) {
    _display = display;
    _start_page = start_page;
    _end_page = end_page;
    _invert_text = inverted_text;
    _old_cursor_x = OFFSET_X;
    _old_cursor_y = CHAR_HEIGHT * start_page;
    _has_cursor = has_cursor;
}

void Screen::backspace() {
    int x = _display->getCursorX();
    int y = _display->getCursorY();

    if (x > 0 && (y>=_start_page*CHAR_HEIGHT && y<_end_page*CHAR_HEIGHT)) {
        x -= CHAR_WIDTH;
        _display->setCursor(x, y);
        _old_cursor_y = y;
        _old_cursor_x = x - CHAR_WIDTH;
        if (_invert_text) {
            _display->fillRect(x, y, CHAR_WIDTH * 4, CHAR_HEIGHT, WHITE);
        } else {
            _display->fillRect(x, y, CHAR_WIDTH * 4, CHAR_HEIGHT, BLACK);
        }
        drawCursor();

        _display->display();
        saveCursor();
    }
}

void Screen::clearScreen() {
    _old_cursor_x = OFFSET_X;
    _old_cursor_y = CHAR_HEIGHT * _start_page;
    int x = _old_cursor_x;
    int y = _old_cursor_y;
    _display->setCursor(x, y);
    if (_invert_text) {
        _display->fillRect(0, y, SCREEN_WIDTH, CHAR_HEIGHT * (_end_page - _start_page), WHITE);
    } else {
        _display->fillRect(0, y, SCREEN_WIDTH, CHAR_HEIGHT * (_end_page - _start_page), BLACK);
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
    if (_old_cursor_y >= _end_page * CHAR_HEIGHT) {
        clearScreen();
    }
    _display->setCursor(_old_cursor_x, _old_cursor_y);
    if (_invert_text) {
        _display->setTextColor(BLACK, WHITE);
    } else {
        _display->setTextColor(WHITE);
    }
}

void Screen::drawCursor() {
    if (_has_cursor) {
        if (_invert_text) {
            _display->drawFastHLine(_old_cursor_x + CHAR_WIDTH, _old_cursor_y + CHAR_HEIGHT - 2, CHAR_WIDTH, BLACK);
        } else {
            _display->drawFastHLine(_old_cursor_x + CHAR_WIDTH, _old_cursor_y + CHAR_HEIGHT - 2, CHAR_WIDTH, WHITE);
        }
    };
}


void Screen::clearScreenIfNeeded() {
    int y = _display->getCursorY();
    int x = _display->getCursorX();
    if (x < OFFSET_X) {
        _old_cursor_x = OFFSET_X;
        _display->setCursor(_old_cursor_x, _old_cursor_y);
    }
    if (y >= (_end_page - 1)*CHAR_HEIGHT) {

        if (x > CHAR_WIDTH * NUMBER_CHARS_ONE_LINE) {
            clearScreen();
        }
    }

}

void Screen::print(char c) {
    restoreCursor();
    clearScreenIfNeeded();
    _display->print(c);
    if (c != '\n') {
        drawCursor();
    } else {
        int x = _old_cursor_x;
        int y = _old_cursor_y;
        if (_invert_text) {
            _display->fillRect(x, y, CHAR_WIDTH * 4, CHAR_HEIGHT, WHITE);
        } else {
            _display->fillRect(x, y, CHAR_WIDTH * 4, CHAR_HEIGHT, BLACK);
        }
    }
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


void Screen::startScrollRight() {
    _display->startscrollright(_start_page, _end_page - 1);
}

void Screen::startScrollLeft() {
    _display->startscrollleft(_start_page, _end_page - 1);
}

void Screen::stopScroll() {
    _display->stopscroll();

}

void DynamicScreen::displayText(std::string text, std::list<int> pattern) {
    _displayMessage=text;
    _stored_pattern=std::list<int>(pattern);
    _periods.clear();
    for(int& p:_stored_pattern){
        _periods.push_back(p);
    }

}

void DynamicScreen::loop() {
    auto current_mills = millis();
    _remaining_period -= static_cast<int>(current_mills - _last_time);
    if (_remaining_period < 10) {
        if (!_periods.empty()) {
            _show=!_show;
            auto period = _periods.front();
            _periods.pop_front();
            if(_show){
                print(_displayMessage.c_str());
            }else{
                clearScreen();
            }
            _remaining_period = period;
        } else {
            for(int& p:_stored_pattern){
                _periods.push_back(p);
            }
        }
    }
    _last_time = current_mills;
    if (_remaining_period < 0) _remaining_period = 0;
}