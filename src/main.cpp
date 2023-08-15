// SenESP Engine Sensors

#include <Arduino.h>

#include "sensesp/sensors/analog_input.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp_app_builder.h"
#include "sensesp/transforms/linear.h"
#include "sensesp/transforms/moving_average.h"
#include "sensesp/transforms/analogvoltage.h"
#include "sensesp/transforms/curveinterpolator.h"
#include "sensesp/transforms/voltagedivider.h"
#include "sensesp/transforms/frequency.h"
#include "sensesp/sensors/digital_input.h"


using namespace sensesp;


class FuelInterpreter : public CurveInterpolator {
 public:
  FuelInterpreter(String config_path = "")
      : CurveInterpolator(NULL, config_path) {
    // Populate a lookup table to translate RPM to M3/s
    clear_samples();
    // addSample(CurveInterpolator::Sample(RPM, M3/s));
    add_sample(CurveInterpolator::Sample(500, 0.0000006944));   //  2,5 l/h
    add_sample(CurveInterpolator::Sample(1000, 0.0000008333));  //  3,0 l/h
    add_sample(CurveInterpolator::Sample(1500, 0.0000013889));  //  5,0 l/h
    add_sample(CurveInterpolator::Sample(1800, 0.0000020833));  //  7,5 l/h
    add_sample(CurveInterpolator::Sample(2000, 0.0000025000));  //  9,0 l/h
    add_sample(CurveInterpolator::Sample(2200, 0.0000030556));  // 11,0 l/h
    add_sample(CurveInterpolator::Sample(2400, 0.0000036111));  // 13,0 l/h
    add_sample(CurveInterpolator::Sample(2600, 0.0000041667));  // 15,0 l/h
    add_sample(CurveInterpolator::Sample(2800, 0.0000051389));  // 18,5 l/h
    add_sample(CurveInterpolator::Sample(3000, 0.0000061111));  // 22,0 l/h
    add_sample(CurveInterpolator::Sample(3200, 0.0000073611));  // 26,5 l/h
    add_sample(CurveInterpolator::Sample(3400, 0.0000088889));  // 32,0 l/h
    add_sample(CurveInterpolator::Sample(3600, 0.0000105556));  // 38,0 l/h
    add_sample(CurveInterpolator::Sample(3800, 0.0000122222));  // 44,0 l/h
  }
};

reactesp::ReactESP app;



// The setup function performs one-time application initialization.
void setup() {
#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("SensESP")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi("My WiFi SSID", "my_wifi_password")
                    //->set_sk_server("xx.xx.xx.xx", 80)
                    ->enable_uptime_sensor()
                    ->get_app();



 //RPM Application/////

  const char* config_path_port_calibrate = "/Port Engine RPM/calibrate";
  const char* config_path_port_skpath = "/Port Engine RPM/sk_path";
  const float multiplier_port = 0.06803;

  const char* config_path_starboard_calibrate = "/Starboard Engine RPM/calibrate";
  const char* config_path_starboard_skpath = "/Starboard Engine RPM/sk_path";
  const float multiplier_starboard = 0.06803;

  auto* sensor1 = new DigitalInputCounter(16, INPUT_PULLUP, RISING, 500);
  auto* sensor2 = new DigitalInputCounter(17, INPUT_PULLUP, RISING, 500);

  sensor1->connect_to(new Frequency(multiplier_port, config_path_port_calibrate))  
  // connect the output of sensor to the input of Frequency()
         ->connect_to(new MovingAverage(2, 1.0,"/Port Engine RPM/movingAVG"))
         ->connect_to(new SKOutputFloat("propulsion.port.revolutions", config_path_port_skpath));  
          // connect the output of Frequency() to a Signal K Output as a number

  sensor1->connect_to(new Frequency(4.08,"/PortFuelMultiplier"))
  // times by 60 to go from Hz to RPM
          ->connect_to(new MovingAverage(4, 1.0,"/Port Engine Fuel/movingAVG"))
          ->connect_to(new FuelInterpreter("/AQAD41 Engine Fuel/curve"))
          ->connect_to(new SKOutputFloat("propulsion.port.fuel.rate", "/Port Engine Fuel/sk_path"));                                       


  sensor2->connect_to(new Frequency(multiplier_starboard, config_path_starboard_calibrate))  
  // connect the output of sensor to the input of Frequency()
         ->connect_to(new MovingAverage(2, 1.0,"/Starboard Engine RPM/movingAVG"))
         ->connect_to(new SKOutputFloat("propulsion.starboard.revolutions", config_path_starboard_skpath));  
          // connect the output of Frequency() to a Signal K Output as a number

  sensor2->connect_to(new Frequency(4.08,"/StarboardFuelMultiplier"))
  // times by 60 to go from Hz to RPM
          ->connect_to(new MovingAverage(4, 1.0,"/Starboard Engine Fuel/movingAVG"))
          ->connect_to(new FuelInterpreter("/AQAD41 Engine Fuel/curve"))
          ->connect_to(new SKOutputFloat("propulsion.starboard.fuel.rate", "/Starboard Engine Fuel/sk_path"));  

  // Start networking, SK server connections and other SensESP internals
  sensesp_app->start();
}

void loop() { app.tick(); }
