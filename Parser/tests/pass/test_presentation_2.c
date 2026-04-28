int globalCount = 3;
bool globalReady = true;

void setup() {
    int localValue = 5;
    bool localFlag = false;

    localValue = globalCount + localValue * 2;
    globalCount = localValue - 1;

    if (globalReady && !localFlag) {
        localValue = globalCount + 4;
    }
}

void loop() {
    int loopValue = globalCount + 1;
    loopValue = loopValue * 2;
}
