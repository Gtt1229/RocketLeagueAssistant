# Rocket League Home Assistant Light Color Changer
BakkesMod Plugin to Change Home Assistant Connected Lights

**It would be nice to make this more module to control more things than just lights**

![RLHAtransparent](https://user-images.githubusercontent.com/23534272/175837042-8db1aea4-214a-4e69-92ab-2c4c705ffeda.png)

## ⭐Special thanks to those in the Discord and notably [Branky](https://github.com/ItsBranK) and [JerryTheBee](https://github.com/ubelhj)⭐
And some other person named Josh

# Home Assistant Configuration
The plugin utilizes Home Assistant's built in Webhook automation trigger. Currently, the plugin is only designed to be used with HTTP. _The skeleton for HTTPS and its token are already in place._

There are 2 automations to create: One for **Blue**, one for **Orange**. _Custom RGB Values may be added in the future, the skeleton is already in the code_

## HA Scenes Configuration:

1. Create a new scene corresponding to the scenario (Home Team, Away Team, Demos, etc)
2. Give it a name (and icon/area if you'd like)
3. Add entities/devices to the scene and adjust the colors accordingly
4. Save the scene
5. Create a new, or duplicate the scene.
6. Adjust entities/devices colors respectively.
7. Save the scene

[**More on scene creation here**](https://www.home-assistant.io/integrations/scene/)

## HA Webhooks Configuration:

### The plugin currently has _-blue_ and _-orange_ hard-coded into it, so please follow carefully.

1. Create a new automation
2. Give it a name (**I suggest not adding a name in the title until after the Webhook ID generation**) and set the trigger type to Webhook:

![image](https://user-images.githubusercontent.com/23534272/175829533-10634472-95e6-48e1-956b-5103fc7ed7c4.png)

![image](https://user-images.githubusercontent.com/23534272/175829554-36b192cc-59dd-4035-bc3d-d439162c5e32.png)

3. Adjust conditions as needed (Time of day, geolocation, etc, etc)
4. Change action type to Activate Scene, then select the corresponding scene you've created.

![image](https://user-images.githubusercontent.com/23534272/175829140-a1e1adfe-5acd-4d0f-b1c1-ce12d9406492.png)

5. Save the automation.
**I like to duplicate the automation and simply append the corresponding function to the Webhook ID, like so:**

![image](https://user-images.githubusercontent.com/23534272/176255055-91793bad-895b-497c-8c97-74185d8a37e3.png)
![image](https://user-images.githubusercontent.com/23534272/176255115-40801a9a-8ea0-462e-b06d-b9754704ba31.png)

6. Change the scene to be activated by the automation.
7. Repeat for each color change you'd like to have (Demos, Freeplay, Main Menu, etc)

[**More on automation here**](https://www.home-assistant.io/docs/automation/)

## Plugin Configuration:

1. After plugin is installed, launch Rocket League and press F2
2. Under the plugin tab, select LightColorChanger:

![image](https://user-images.githubusercontent.com/23534272/176254969-cc357749-22e5-4fa2-849a-419f3f824d40.png)

3. From Home Assistant, copy the Webhook ID from the corresponding automation and paste it into the setting's correct setting's text box.
    
    a.
  
    ![image](https://user-images.githubusercontent.com/23534272/175829789-2e6e5c68-185e-4730-bab4-52f24f494593.png)
    
    b.

    ![image](https://user-images.githubusercontent.com/23534272/176254104-5f802d3a-0b11-49a7-9853-7d25d9109186.png)

That should do it!

### Notes

If you manually reload the plugin (through the F6 BakkesMod Console) while in the game, the URL will have to be reentered.

## To Do

* Find a better Function to hook to for running the automation. Currently, the function is called when you switch teams, and when a goal is scored.
* Finalize RGB support to push to HA bulbs (This may vary on manufacturer, so may a duanting task)
* Probably code clean up and Nullchecks
