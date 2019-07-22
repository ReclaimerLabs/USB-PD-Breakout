# Introduction

This board features an FUSB302B as a Type-C Port Management (TCPM) chip. You can purchase one on [Tindie](https://www.tindie.com/products/ReclaimerLabs/usb-type-c-power-delivery-phy-breakout-board/). 

# Features

The FUSB302B is a fully-capable USB Power Delivery device. The power header is rated to 100 W (20 V at 5 A) if both Vbus pins are used along with two ground pins. The FUSB302B's max I2C clock frequency is 1 MHz. 

# Example Usage

Example code for use with Arduino can be found [here](https://github.com/graycatlabs/usb-c-arduino/tree/master/usb-c-demo). The [USB-C Explorer](https://github.com/ReclaimerLabs/USB-C-Explorer/tree/master/firmware/USB-C%20Explorer) also uses the same chip. 

Here are the connections you will need. 

* Breakout Pin -> Arduino Pin
* GND -> GND
* Vbus -> Vin (power to the Arduino, regulated down to +5V)
* Vpu -> IOREF (power for pullups for I2C and interrupt)
* VDD -> 3.3V (power for the FUSB302B)
* SDA -> SDA
* SCL -> SCL
* INT -> D12
* GND -> GND (for good measure)

# Questions, Comments, and Contributions

Pull requests are welcome. If you have questions or comments, you can email me directly at jason@reclaimerlabs.com. 
