#include "climate.h"

#include "binary.h"
#include "config.h"
#include "debug.h"


Climate::Climate(Clock* clock, GPIO* gpio) : clock_(clock),
        rear_defrost_(REAR_DEFROST_PIN, REAR_DEFROST_TRIGGER_MS, clock, gpio) {
    // Init operational state.
    state_ = STATE_OFF;
    mode_ = MODE_OFF;
    dual_ = false;

    // Init state storage.
    state_init_ = 0;
    state_changed_ = false;
    state_last_broadcast_ = 0;
    initFrame(&state_frame_, CLIMATE_STATE_FRAME_ID, 8);

    // Init control storage.
    control_init_ = false;
    control_changed_ = true;
    control_last_broadcast_ = 0;
    initFrame(&control_frame_540_, 0x540, 8);
    control_frame_540_.data[0] = 0x80;
    initFrame(&control_frame_541_, 0x541, 8);
    control_frame_541_.data[0] = 0x80;
    memset(control_state_, 0, 8);
}

void Climate::receive(const Broadcast& broadcast) {
    uint32_t control_hb = control_init_ ? CLIMATE_CONTROL_FRAME_HB : CLIMATE_CONTROL_INIT_HB;
    rear_defrost_.update();

    if (!control_init_ && clock_->millis() >= CLIMATE_CONTROL_INIT_EXPIRE) {
        control_frame_540_.data[0] = 0x60;
        control_frame_540_.data[1] = 0x40;
        control_frame_540_.data[6] = 0x04;
        control_frame_541_.data[0] = 0x00;
        control_init_ = true;
    }

    if (control_changed_ ||
            clock_->millis() - control_last_broadcast_ >= control_hb) {
        control_changed_ = false;
        control_last_broadcast_ = clock_->millis();
        broadcast(control_frame_540_);
        broadcast(control_frame_541_);
    }

    if (state_init_ == 0x03 && (state_changed_ ||
            clock_->millis() - state_last_broadcast_ >= CLIMATE_STATE_FRAME_HB)) {
        state_changed_ = false;
        state_last_broadcast_ = clock_->millis();
        broadcast(state_frame_);
    }
}

void Climate::send(const Frame& frame) {
    switch (frame.id) {
        case 0x54A:
            handle54A(frame);
            break;
        case 0x54B:
            handle54B(frame);
            break;
        case 0x625:
            handle625(frame);
            break;
        case CLIMATE_CONTROL_FRAME_ID:
            handleControl(frame);
            break;
    }
}

bool Climate::filter(uint32_t id) const {
    return id == 0x54A || id == 0x54B || id == 0x625 || id == CLIMATE_CONTROL_FRAME_ID;
}

void Climate::handle54A(const Frame& frame) {
    if (frame.len != 8) {
        return;
    }
    state_init_ |= 0x01;
    setDriverTemp(frame.data[4]);
    setPassengerTemp(frame.data[5]);
    setOutsideTemp(frame.data[7]);
}

void Climate::handle54B(const Frame& frame) {
    if (frame.len != 8) {
        return;
    }
    state_init_ |= 0x02;
    bool ac = getBit(frame.data, 0, 3);
    bool recirculate = getBit(frame.data, 3, 4);
    uint8_t fan_speed = (frame.data[2] + 1) / 2;

    dual_ = getBit(frame.data, 3, 7);
    mode_ = (Mode)frame.data[1];

    if (mode_ == MODE_WINDSHIELD) {
        state_ = STATE_DEFROST;
    } else if (getBit(frame.data, 0, 7)) {
        state_ = STATE_OFF;
    } else if (getBit(frame.data, 0, 0)) {
        state_ = STATE_AUTO;
    } else if (fan_speed == 0) {
        state_ = STATE_HALF_MANUAL;
    } else {
        state_ = STATE_MANUAL;
    }

    switch (state_) {
        case STATE_OFF: 
            setActive(false);
            setAuto(false);
            setAc(false);
            setDual(false);
            setRecirculate(false);
            setFrontDefrost(false);
            setFanSpeed(0);
            setDriverTemp(0);
            setPassengerTemp(0);
            setMode(MODE_OFF);
            break;
        case STATE_AUTO:
            setActive(true);
            setAuto(true);
            setAc(ac);
            setDual(dual_);
            setRecirculate(recirculate);
            setFrontDefrost(false);
            setFanSpeed(fan_speed);
            setMode(mode_);
            break;
        case STATE_MANUAL:
        case STATE_HALF_MANUAL:
            setActive(true);
            setAuto(false);
            setAc(ac);
            setDual(dual_);
            setRecirculate(recirculate);
            setFrontDefrost(false);
            setFanSpeed(fan_speed);
            setMode(mode_);
            break;
        case STATE_DEFROST:
            setActive(true);
            setAuto(false);
            setAc(ac);
            setDual(false);
            setRecirculate(false);
            setFrontDefrost(true);
            setFanSpeed(fan_speed);
            setMode(MODE_WINDSHIELD);
            break;
    }
}

void Climate::handle625(const Frame& frame) {
    if (frame.len == 0) {
        return;
    }
    setRearDefrost(getBit(frame.data, 0, 0));
}

void Climate::handleControl(const Frame& frame) {
    // check if any bits have flipped
    if (xorBits(control_state_, frame.data, 0, 0)) {
        triggerOff();
    }
    if (xorBits(control_state_, frame.data, 0, 1)) {
        triggerAuto();
    }
    if (xorBits(control_state_, frame.data, 0, 2)) {
        triggerAc();
    }
    if (xorBits(control_state_, frame.data, 0, 3)) {
        triggerDual();
    }
    if (xorBits(control_state_, frame.data, 0, 4)) {
        triggerMode();
    }
    if (xorBits(control_state_, frame.data, 0, 6)) {
        triggerFrontDefrost();
    }
    if (xorBits(control_state_, frame.data, 0, 7)) {
        triggerRecirculate();
    }
    if (xorBits(control_state_, frame.data, 1, 0)) {
        triggerFanSpeedUp();
    }
    if (xorBits(control_state_, frame.data, 1, 1)) {
        triggerFanSpeedDown();
    }
    if (xorBits(control_state_, frame.data, 1, 2)) {
        triggerDriverTempUp();
    }
    if (xorBits(control_state_, frame.data, 1, 3)) {
        triggerDriverTempDown();
    }
    if (xorBits(control_state_, frame.data, 1, 4)) {
        triggerPassengerTempUp();
    }
    if (xorBits(control_state_, frame.data, 1, 5)) {
        triggerPassengerTempDown();
    }
    if (xorBits(control_state_, frame.data, 4, 0)) {
        triggerRearDefrost();
    }

    // update the stored control state
    memcpy(control_state_, frame.data, 8);
}

void Climate::setActive(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 0, value);
}

void Climate::setAuto(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 1, value);
}

void Climate::setAc(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 2, value);
}

void Climate::setDual(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 3, value);
}

void Climate::setFace(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 4, value);
}

void Climate::setFeet(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 5, value);
}

void Climate::setRecirculate(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 7, value);
}

void Climate::setFrontDefrost(bool value) {
    state_changed_ |= setBit(state_frame_.data, 0, 6, value);
}

void Climate::setRearDefrost(bool value) {
    state_changed_ |= setBit(state_frame_.data, 4, 0, value);
}

void Climate::setFanSpeed(uint8_t value) {
    if (state_frame_.data[1] != value) {
        state_frame_.data[1] = value;
        state_changed_ = true;
    }
}

void Climate::setDriverTemp(uint8_t value) {
    if (state_frame_.data[2] != value) {
        state_frame_.data[2] = value;
        state_changed_ = true;
    }
}

void Climate::setPassengerTemp(uint8_t value) {
    if (state_frame_.data[3] != value) {
        state_frame_.data[3] = value;
        state_changed_ = true;
    }
}

void Climate::setOutsideTemp(uint8_t value) {
    if (state_frame_.data[7] != value) {
        state_frame_.data[7] = value;
        state_changed_ = true;
    }
}

void Climate::setMode(uint8_t mode) {
    switch (mode) {
        case MODE_FACE:
        case MODE_AUTO_FACE:
            setFace(true);
            setFeet(false);
            setFrontDefrost(false);
            break;
        case MODE_FACE_FEET:
        case MODE_AUTO_FACE_FEET:
            setFace(true);
            setFeet(true);
            setFrontDefrost(false);
            break;
        case MODE_FEET:
        case MODE_AUTO_FEET:
            setFace(false);
            setFeet(true);
            setFrontDefrost(false);
            break;
        case MODE_FEET_WINDSHIELD:
            setFace(false);
            setFeet(true);
            setFrontDefrost(true);
            break;
        case MODE_WINDSHIELD:
            setFace(false);
            setFeet(false);
            setFrontDefrost(true);
            break;
        default:
            setFace(false);
            setFeet(false);
            setFrontDefrost(false);
            break;
    }
}

void Climate::triggerOff() {
    toggleBit(control_frame_540_.data, 6, 7);
    control_changed_ = true;
}

void Climate::triggerAuto() {
    toggleBit(control_frame_540_.data, 6, 5);
    control_changed_ = true;
}

void Climate::triggerAc() {
    toggleBit(control_frame_540_.data, 5, 3);
    control_changed_ = true;
}

void Climate::triggerDual() {
    toggleBit(control_frame_540_.data, 6, 3);
    control_changed_ = true;
}

void Climate::triggerRecirculate() {
    toggleBit(control_frame_541_.data, 1, 6);
    control_changed_ = true;
}

void Climate::triggerMode() {
    toggleBit(control_frame_540_.data, 6, 0);
    control_changed_ = true;
}

void Climate::triggerFrontDefrost() {
    toggleBit(control_frame_540_.data, 6, 1);
    control_changed_ = true;
}

void Climate::triggerRearDefrost() {
    rear_defrost_.trigger();
    control_changed_ = true;
}

void Climate::triggerFanSpeedUp() {
    toggleBit(control_frame_541_.data, 0, 5);
    control_changed_ = true;
}

void Climate::triggerFanSpeedDown() {
    toggleBit(control_frame_541_.data, 0, 4);
    control_changed_ = true;
}

void Climate::triggerDriverTempUp() {
    if (state_ == STATE_OFF) {
        return;
    }
    toggleBit(control_frame_540_.data, 5, 5);
    control_frame_540_.data[3]++;
    control_changed_ = true;
}

void Climate::triggerDriverTempDown() {
    if (state_ == STATE_OFF) {
        return;
    }
    toggleBit(control_frame_540_.data, 5, 5);
    control_frame_540_.data[3]--;
    control_changed_ = true;
}

void Climate::triggerPassengerTempUp() {
    if (state_ == STATE_OFF) {
        return;
    }
    toggleBit(control_frame_540_.data, 5, 5);
    control_frame_540_.data[4]++;
    control_changed_ = true;
}

void Climate::triggerPassengerTempDown() {
    if (state_ == STATE_OFF) {
        return;
    }
    toggleBit(control_frame_540_.data, 5, 5);
    control_frame_540_.data[4]--;
    control_changed_ = true;
}
