//
// Created by Jing SHEN on 1/10/20.
//

#include "base_view.hpp"


void BaseView::setup() {
    display.setup();
    keypad.setup();
    topBar.clearScreen();
    current_mills = millis();
    last_task_mills = current_mills;
    diff_mills = 0;
    keypad.addListener(this);
    topBar.displayText("     Main Menu", topBarPattern);

}

void BaseView::loop() {

    topBar.loop();
    keypad.loop();


}


void BaseView::onMainPressed() {

}

void BaseView::onMainReleased() {

}

void BaseView::onLeftPressed() {

}

void BaseView::onLeftReleased() {

}

void BaseView::onRightPressed() {

}

void BaseView::onRightReleased() {

}

void BaseView::onOkPressed() {

}

void BaseView::onOkReleased() {

}

void BaseView::clearClient() {

    int x = 0;
    int y = CHAR_HEIGHT;
    display.setCursor(x, y);
    display.fillRect(0, y, SCREEN_WIDTH, CHAR_HEIGHT * 7, BLACK);
    display.display();
}
