//
// Created by Jing SHEN on 19/9/20.
//

#include "screen.hpp"

Screen::Screen(OledDisplay* display, int start_page, int end_page,bool inverted_text=false ) {
    _display=display;
    _start_page=start_page;
    _end_page=end_page;
    _invert_text=inverted_text;
}

void Screen::backspace() {
    int x = _display->getCursorX();
    int y = _display->getCursorY();

    if(x>0){
        x-=CHAR_WIDTH;
        _display->setCursor(x,y);
        if(_invert_text) {
            _display->fillRect(x, y, CHAR_WIDTH, CHAR_HEIGHT, WHITE);
        }else{
            _display->fillRect(x, y, CHAR_WIDTH, CHAR_HEIGHT, BLACK);
        }

        _display->display();
    }
}

void Screen::process_char(char c) {

}