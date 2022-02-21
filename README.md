# Foodguard
Product Development Project 2017-2018 for Orthex at Aalto University.
In this project the aim was to provide additional value to Orhtex by integrating a "smart product" that would assist with their core businesses. After significant market research and ideation, our focus was narrowed to their food storage. 
In food storage, we identified food waste being a problem. This was due to people forgetting what was in the storage containers, and when that food was put into them. To address this problem, we created a attachable e-paper display that could be easily controlled by an app.

![Image of foodguard app and IOT device](https://user-images.githubusercontent.com/35771181/154860423-049b961f-e98c-41a8-a460-398b4503d094.JPG)

The team comprised of 3 mechanical engineering majors, 1 electrical engineering major, 2 design majors and 3 marketing majors (remotely).  

## The product
The premise of the app was to help track what food you have at home, and remind the user when something was going to expire. This was done by having the user add their food items by hand, or take a picture of the item they wanted to add. An item would be created in a list, an expected expiration date would be suggested which could be modified if needed. At this stage, the user could connect a "FoodGuard" tag to the item, which would display the name of the food and the expiration date. The tag would turn Red, when the food item was approaching the expiration date.

## Product architecture
The app would create data onto a firebase server, from which the IOT device would fetch the data and update the acknowledgment that data had been recieved.

For streamlining the item addition process, we added image recognition functionality with the aid of Google’s Cloud vision API. The cloud vision platform sends the image to google’s servers where it returns keywords and the read text. The app then reads that text and searches the local database for similar food to estimate the expiration. 
![image](https://user-images.githubusercontent.com/35771181/154919056-ae3068a3-158a-48a3-a9b1-10b07885fe44.png)

### The App
The choice of App platform was down to ease of accessibility: thus Android 5.0 ( (api 21/LOLLIPOP) or nerwer was set as the target demographic (accounted for 70% Finnish smart phone market). 
![FoodGuard app](https://user-images.githubusercontent.com/35771181/154912426-e8e4ffba-84a0-42c6-a84b-19f030521b9f.png)

The User Would log in to the app once, and then their id would be stored. Along with this, A new user food item list would be generateed in the cloud. The cloud database was hosted with  google’s Firebase Database servers. The choice of Firebase was primarily based on ease of integration, and the possibility to easily integrate advertising and other features in the future.

### The IOT device
![IOT device](https://user-images.githubusercontent.com/35771181/154917472-e25c2059-e802-4d9e-97f8-4060d17ca41c.png)

The IOT device was made by attaching a E-paper displat to an ESP8266. A LiPo battery was used to power the item.  

The e-paper would provide a display that would display the information even in the case of the battery running out, and also consumed significantly less than other alternatives. The ESP8266 was chosen for its wifi connectivity and the breadth of its supported libraries. This allowed us to iterate with arduino quickly. An ESP32 was our original choice, but due to our lack of experience and the immaturity of the platform, we felt compelled to switch away from it.

### Issues

1) Security is an issue that was not addressed during development. If the product was developed foreward, a different securrity structure would be advised.
2) The database does not scale gracefully, and switching to any structured database would provide a performance improvement. 
3) WIFI was used for communication with the device, which is power inefficient. BLE was a choice early in development, but due to our lack of expeience with it, it was ignored.

My key contribuion was the app that we created and project management. 

