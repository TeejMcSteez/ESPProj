## ESP Door Sensor Project ##
This uses two HiLetGo ESP-32 micro-controllers and an HC-SR04 ultrasonic sensor to create a web UI that logs room entry and exit information
Made mainly to get better with network protocols and have fun with pointers
It uses ESP-Now to accept data from sensor so the specific devices are paired. This makes it so the receiver can be anywhere in the house away from the sensor.
it has a long range see -> https://www.reddit.com/r/esp32/comments/o1xpzn/espnow_real_world_range_test_standard_and_long/
---
### Parts ####
- http://www.hiletgo.com/ProductDetail/1906566.html 
- https://www.amazon.com/HC-SR04-Ultrasonic-Distance-Measuring-MEGA2560/dp/B088BT8CDW
- https://www.digikey.com/en/products/detail/sunled/XLMDK12D/6615735?utm_adgroup=&utm_source=google&utm_medium=cpc&utm_campaign=PMax%20Shopping_Product_Medium%20ROAS%20Categories&utm_term=&utm_content=&utm_id=go_cmp-20223376311_adg-_ad-__dev-c_ext-_prd-6615735_sig-CjwKCAjwpbi4BhByEiwAMC8JnVPHhJuY_SN-pRvSCNyIcE5f4ogwmm62Oj62RAakFOtno5SIwvO7wBoCkJQQAvD_BwE&gad_source=1&gclid=CjwKCAjwpbi4BhByEiwAMC8JnVPHhJuY_SN-pRvSCNyIcE5f4ogwmm62Oj62RAakFOtno5SIwvO7wBoCkJQQAvD_BwE
- https://www.elexp.com/products/08lvb3330dblue-super-bright-led-diffuse?variant=44459161485540&currency=USD&utm_source=google&utm_medium=organic&utm_campaign=Google%20Shopping&utm_content=Super%20Bright%20LED%20-%20Blue%20-%205mm%20-%20Diffused%20Lens&gad_source=1&gclid=CjwKCAjwpbi4BhByEiwAMC8JncXPOFQixLswLoYdqhLPlTJ6K8gyogopYL8b94I57OwVibRkV8Wc5xoCmPoQAvD_BwE
- https://www.amazon.com/TUOFENG-Electronic-Prototyping-Circuits-Breadboarding/dp/B07TX6BX47?source=ps-sl-shoppingads-lpcontext&ref_=fplfs&smid=A1P2Y1BEUUWGO2&th=1
### Libraries ### 
1. WiFi -> https://docs.arduino.cc/libraries/wifi/ 
2. esp_now -> https://github.com/espressif/arduino-esp32
3. ESPAsyncWebServer -> https://github.com/me-no-dev/ESPAsyncWebServer 
4. ESPmDNS -> https://github.com/espressif/arduino-esp32/blob/master/libraries/ESPmDNS/src/ESPmDNS.h (Not Required but used for easy to remember URL rather than connecting at the port and ip specifically [QOL enhancement])
---
**NOTE** 
---
This project is VERY broken as I am not good with the low level stuff yet and is more to learn about how it works by breaking stuff