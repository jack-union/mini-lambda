/*
 * Stepper motor functions
 * 
 */
 
void reset_stepper() {
  stepper.zero(); //Initialize stepper at 0 location
  if (SWEEP_AT_START) {
    stepper.setPosition(STEPS - 1);
    stepper.updateBlocking();
    delay (500);
    stepper.setPosition(0);
    stepper.updateBlocking();
  }
}

void set_stepper_pos() {
  uint16_t pos;

  // connection valid?
  if (lambdaError == 255) {
    pos = (((uint32_t)lambda - 70) * GAUGE_RANGE / 60) + GAUGE_OFFSET;
    stepper.setPosition(pos);
  } else { // display error status
    if (lambdaError == 127) { // no sensor
      stepper.setPosition(GAUGE_NC);
    } else { // all other errors
      stepper.setPosition(GAUGE_ND);
    }
    stepper.updateBlocking(); // go there now
  }
}
