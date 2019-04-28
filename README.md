# Connected Healthcare  

# Low Power Node  
Group report: https://drive.google.com/drive/u/1/folders/19hdArP41vl--8bYrzDXRaQdipgCvqJyU  

Individual report: https://docs.google.com/document/d/1R7fJJIHysDMEKZR2WjfFpgR2WOkpU7uXLiXtukH0Cd0/edit  

Command table: https://docs.google.com/spreadsheets/u/1/d/1uHh-QOkRwY1GcTBOUW6LPIHZMnQlPVpkgPMqkcA2ELI/edit?usp=drive_web&ouid=115121427441734205213

# April 13th, 2019 - Project Update 1  

1) Interfaced accelerometer and tested fall detection functionality  
2) Implemented simple publish feature  


# April 20th, 2019 - Project Update 2  

1) Configured accelerometer to detect taps  
2) Implemented Low Power Node that publishes and subscribes using mesh level model  

Working:  
1) If fall or tap is detected a buzzer starts to sound and status is published to the friend node  
2) Friend node responds with a signal to turn off the buzzer  

# April 27th, 2019 - Final Project
1) Pressing PB0 configures fall detection mode  
2) Pressing PB1 configures tap detection mode  
3) Mesh communication between friend and low power node successful using 2 mesh models - ON/OFF, Level model  
4) Alarm notification is emulated by turning ON/OFF the onboard LED. A Buzzer ON/OFF message is displayed on the LCD  


