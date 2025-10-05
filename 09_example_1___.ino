// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define N 30 // Number of samples for median filter. You can change this value (e.g., 3, 10, 30)
float samples[N]; // Array to store recent N samples
int sample_index = 0; // Index for the circular buffer

// global variables
unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_median;
  
  // wait until next sampling time.
  if (millis() < last_sampling_time + INTERVAL)
    return;
  
  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  // Implement the range filter
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX) || (dist_raw < _DIST_MIN)) {
      samples[sample_index] = dist_raw;
  } else {    // In desired Range
      samples[sample_index] = dist_raw;
  }
  
  // Update the sample index for circular buffer
  sample_index = (sample_index + 1) % N;
  
  // Get the median of the samples
  dist_median = getMedian(samples, N);

  // output the distance to the serial port
  Serial.print("Min:");
  Serial.print(_DIST_MIN);
  Serial.print(",raw:");
  Serial.print(dist_raw);
  Serial.print(",ema:");
  Serial.print(0,0);
  Serial.print(",median:");
  Serial.print(dist_median);
  Serial.print(",Max:");
  Serial.print(_DIST_MAX);
  Serial.println("");

  // Control LED based on raw distance
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX)) {
    digitalWrite(PIN_LED, 1); // LED OFF
  } else {
    digitalWrite(PIN_LED, 0); // LED ON
  }
  
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// Function to calculate the median of a float array
float getMedian(float arr[], int size) {
  // Simple bubble sort to sort the array
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j+1]) {
        float temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }

  // If size is odd, return the middle element
  if (size % 2 != 0) {
    return arr[size / 2];
  } else { // If size is even, return the average of the two middle elements
    return (arr[size / 2 - 1] + arr[size / 2]) / 2.0;
  }
}

// get a distance reading from USS.
// return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
