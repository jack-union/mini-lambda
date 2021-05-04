/*
   serial lambda sensor functions

*/
void fastLambdaUpdate() {
  altSerial.write('F');
  altSerial.write(EOL_CR);
  altSerial.flush();
}

bool lambdaReadInput() {
  char c;
  bool endFound = false;

  // Anything in input?
  while (altSerial.available()) {
    c = altSerial.read();
    lambdaInputBuffer[lambdaInputNextPos] = c;
    lambdaInputNextPos++;
        // Buffer overflow?
    if (lambdaInputNextPos == 20) {
      lambdaInputNextPos = 0;
    }
    
    // Linefeed?
    if ( c == EOL_LF ) {
      endFound = true;
      break;  
    } 
  }
  return endFound;
}

void lambdaProcessInput() {
  char *c_lambda;
  char *c_status;
  char *c_error;
  char *c_checksum;
  uint8_t reading;
  uint8_t status;
  uint8_t error;
  uint16_t checksum;  
    
  if (lambdaInputNextPos >= 2 ) { //at least CR/LF
    // terminate string with 0
    lambdaInputBuffer[lambdaInputNextPos] = 0;

    // split input on ;
    c_lambda   = strtok(lambdaInputBuffer, ";");
    c_status   = strtok(NULL, ";");
    c_error    = strtok(NULL, ";");
    c_checksum = strtok(NULL, ";");

    // convert to numeric
    reading  = atoi(c_lambda);
    status = atoi(c_status);
    error  = atoi(c_error);
    checksum = atoi(c_checksum);

    // data valid?
    if ((reading + status + error) == ~checksum )
    {
      // CJ125 status okay?
      if (error == 255) {
        lambda = reading;
        lambdaLastDataAt = millis();
      }
      lambdaStatus = status;
      lambdaError  = error;
    }
  }
  // reset buffer
  lambdaInputNextPos=0;
}

void lambdaSenseTimeout() {
  if ((millis() - lambdaLastDataAt) > UPDATE_INTERVAL*100) {
    lambdaError = 254;
  }    
}
