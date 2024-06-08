# JDY08 Ring Buffer
This library is used for interfacing with the JDY08 BLE Module and can estimate the distance based off the RSSI signal


![photo](JDY08_module.png)

Requirements :
 - Any STM32 MCU
 - JDY08 Bluetooth module (it works with other modules as well)
 - LightBlue mobile app (available on both Android and iOS)

How to estimate the distance based off the RSSI measurement :

First, open the LightBlue app, you should see the module available(under a similar name perhaps, in my case it's labeled as "JDY") :


![lbbild](https://github.com/theom4/JDY08_Ring_Buffer/assets/154817034/a9c05a33-732a-4de5-a4f8-a6377fe9fa0e)

  Place your phone 1 meter away from the module and note down the shown RSSI(it can be seen just below the signal strength indicator).
Now change the macro "RSSI_D0" in the JDY08.h file
to this value.The next step is to place the phone to another known distance(in my case 2 meters away) and note down again the new RSSI, 
which should be used when calling the function 
"rssi_get_n_factor([RSSI_VALUE_AT_2_METERS],[DISTANCE_CHOSEN])".Now the distance can be estimated based off the returned "n" factor with the function "rssi_get_distance()".

Sending and receiving the data via BLE with the LightBlue app :

  On the JDY08 module side, the data can be sent or received by simply writing or reading via UART with the MCU.Once the app is connected to the module, 
go to the "RX-TX" section for commnication.

And that's pretty much it! 
