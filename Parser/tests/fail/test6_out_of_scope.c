void setup() {
    int x = 3;

    if (x < 5) {
        x = x + 1;
        int y = 10; // 'y' is declared in the inner scope of the 'if' block
    } else {
        x = x - 1;
    }
    y = 20;
}


void loop() {
    bool flag = false;

    if (!flag) {
        flag = true;
    } else {
        flag = false;
    }
}