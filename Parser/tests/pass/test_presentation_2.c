void setup() {
    int outerCount = 3;
    bool outerReady = true;

    int localValue = 5;
    bool localFlag = false;

    localValue = outerCount + localValue * 2;
    outerCount = localValue - 1;

    if (outerReady && !localFlag) {
        int innerValue = outerCount + 4;
        localValue = innerValue + outerCount;
    }
}

void loop() {
    int loopValue = 1;

    loopValue = loopValue * 2;
}
