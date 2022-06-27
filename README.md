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

1. Create a new scene corresponding to blue
2. Give it a name (and icon/area if you'd like)
3. Add entities/devices to the scene and adjust the colors to blue
4. Save the scene
5. Duplicate the scene and change the name to correspond with orange
6. Adjust entities/devices colors respectively.
7. Save the scene

[**More on scene creation here**](https://www.home-assistant.io/integrations/scene/)

## HA Webhooks Configuration:

### The plugin currently has _-blue_ and _-orange_ hard-coded into it, so please follow carefully.

1. Create a new automation
2. Give it a name (**I suggest not adding a color in the title until after the Webhook ID generation**) and set the trigger type to Webhook:

![image](https://user-images.githubusercontent.com/23534272/175829533-10634472-95e6-48e1-956b-5103fc7ed7c4.png)

![image](https://user-images.githubusercontent.com/23534272/175829554-36b192cc-59dd-4035-bc3d-d439162c5e32.png)


3. Append `-blue` to the generated Webhook ID:

![image](https://user-images.githubusercontent.com/23534272/175829548-490c0646-5474-4de6-a93d-70a572ebc230.png)


4. Adjust conditions as needed (Time of day, geolocation, etc, etc)
5. Change action type to Activate Scene, then select the blue scene you just created.

![image](https://user-images.githubusercontent.com/23534272/175829140-a1e1adfe-5acd-4d0f-b1c1-ce12d9406492.png)

6. Save the automation
7. Duplicate the automation

![image](https://user-images.githubusercontent.com/23534272/175829685-913ab471-0314-4603-b6a8-16e841ecc499.png)

8. Change the name to correspond with Orange

![image](https://user-images.githubusercontent.com/23534272/175829741-589a2225-a958-48a8-8420-3907efd516b0.png)


9. Replace '-blue' with '-orange' in the Webhook ID:

![image](https://user-images.githubusercontent.com/23534272/175829698-6acd767f-0886-49d0-8d85-0aad9f1aa7ec.png)

    The webhook was originally rocket-league-webhook-light-HsiQEzoCdZdgvv0sqxkdYIc2-**blue**, we've changed it to rocket-
    ue-webhook-light-    HsiQEzoCdZdgvv0sqxkdYIc2-**orange**

10. Change the scene to be activated by the automation.

You should now have 2 automations with identical Webhook ID's with the only difference being '-blue' and '-orange'

[**More on automation here**](https://www.home-assistant.io/docs/automation/)

## Plugin Configuration:

1. After plugin is installed, launch Rocket League and press F2
2. Under the plugin tab, select LightColorChanger:

![image](https://user-images.githubusercontent.com/23534272/175829395-e0ec1f90-fa12-4208-ac56-46cc0eedc878.png)

3. From Home Assistant, copy the Webhook ID from either automation and paste it into the setting's text box, then remove the appended "-color":
  
  a.
  
  ![image](https://user-images.githubusercontent.com/23534272/175829789-2e6e5c68-185e-4730-bab4-52f24f494593.png)

  b.
  
  ![image](https://user-images.githubusercontent.com/23534272/175829821-d6349cdd-cf58-4f6d-84f4-16bb9292bc71.png)

  c.
  
  ![image](https://user-images.githubusercontent.com/23534272/175829840-21117607-13ab-4364-9101-789d48cf19bc.png)


That should do it!

### Notes

If you manually reload the plugin (through the F6 BakkesMod Console) while in the game, the URL will have to be reentered.

## To Do

* Find a better Function to hook to for running the automation. Currently, the function is called when you switch teams, and when a goal is scored.
* Finalize RGB support to push to HA bulbs (This may vary on manufacturer, so may a duanting task)
* Probably code clean up and Nullchecks
