# Foliar-Fertigation-Exp
Our project applies two foliar fertigation methods, an automated environmental and physiological driven method vs. a constant rate fertigation system
# Introduction:

Foliar fertilization is a direct nutrient delivery method for elements with low mobility and availability in the soil-plant pathway. Minerals are absorbed through the leaves, ensuring efficient transport to their target sites. Since nutrient uptake occurs via stomata, wider stomatal openings enhance fertilization efficiency. In our project, we developed a system to foliar application methods. One system applies fertilization at conventional times, while the other uses environmental and physiological data to optimize timing, ensuring fertilization occurs precisely when stomata are open.
![image](https://github.com/user-attachments/assets/9ea02b4d-c344-4080-bd67-ccb760304a65)![image](https://github.com/user-attachments/assets/cbad90ad-c8bb-4fc6-9803-bb326e3d66c6)



# Hardware and assembly
To measure stomatal conductance, we used the formula gs = E / VPD. Transpiration (E) was measured using NAU7802 load cells, which recorded plant weight changes, while VPD values were obtained from a meteorological station. Sensor data was processed by a background software that continuously calculated the current stomatal conductance.
<img width="653" alt="image" src="https://github.com/user-attachments/assets/d679c28c-0372-4901-afde-8e4eceabcd26" />

The desired stomatal opening range was set arbitrarily.
When the software detected that the threshold was met, it triggered fertilization for the "smart" treatment. Meanwhile, the conventional treatment received fertilization at fixed preset times.

Fertilization timing also varied between treatments. The conventional treatment received a fixed 3-minute application, while the smart treatment used a leaf moisture sensor (potentiometer) to monitor saturation. Fertilization stopped as soon as the leaf reached full absorption capacity, preventing waste and ensuring optimal uptake.

Irrigation was applied identically to both plants, with water volume determined based on transpiration measurements. The amount of water supplied was calculated according to the transpiration rate of a single reference plant, under the assumption that both plants experienced similar environmental conditions and transpiration patterns. This approach ensured that irrigation matched actual water loss, maintaining consistent hydration levels without excess or deficit.

sketch
 ![image](https://github.com/user-attachments/assets/d2bb3d9b-94bb-4aba-9f59-729c4e2495f4)
 ![image](https://github.com/user-attachments/assets/5840ab3e-4319-4221-80b2-c94b0ebd7780)

 
 We used two esp32 controllers: one connected to the load cells, transmitting data to ThingSpeak, and another controlling the fertilization pump and valves based on commands from the background software. Additionally, the second controller was linked to the leaf moisture sensor, receiving real-time readings to stop fertilization once saturation was reached.


# Experiment
![image](https://github.com/user-attachments/assets/5216f937-63fb-45c5-800a-081717464ffe)
![image](https://github.com/user-attachments/assets/9c22373a-a0de-436b-b9f5-81d950a752f6)
![image](https://github.com/user-attachments/assets/9c6aa51e-e57c-4f8d-a51e-4ee824519b0c)




