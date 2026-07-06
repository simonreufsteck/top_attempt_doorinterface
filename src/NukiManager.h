#pragma once

#include <NukiBle.h>
#include <NukiLock.h>
#include <BleScanner.h>

class NukiManager : public Nuki::SmartlockEventHandler {
public:
    NukiManager();
    void begin();
    void loop();

    bool isPaired();
    bool isPairing();
    void startPairing();
    void cancelPairing();

    String getLockStateStr();
    int getBatteryPct();
    bool isBatteryCritical();
    int getRssi();

    bool unlock();
    bool lock();

    void notify(Nuki::EventType eventType) override;

private:
    BleScanner::Scanner _scanner;
    NukiLock::NukiLock _nukiLock;

    bool _pairingRequested = false;
    unsigned long _pairingStart = 0;
    static const unsigned long _pairingTimeout = 600000;

    bool _stateUpdateNeeded = false;
    NukiLock::KeyTurnerState _lastState;
    bool _hasState = false;
};
