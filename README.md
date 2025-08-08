# Rocket League Assistant
BakkesMod Plugin to integrate Rocket League events with Home Assistant

If you *really* want to buy me a coffee: [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/gtt1229)


## Discord: [Rocket League Assistant](https://discord.gg/8UDX9eVUFb)

![RLHAbanner](https://user-images.githubusercontent.com/23534272/175837042-8db1aea4-214a-4e69-92ab-2c4c705ffeda.png)

## ⭐Special thanks to those in the Discord and notably [Branky](https://github.com/ItsBranK) and [JerryTheBee](https://github.com/ubelhj)⭐
And some other person named Josh

# Home Assistant Configuration
The plugin utilizes Home Assistant's built in Webhook automation trigger with JSON-based conditions.

## Home Assistant Automations Configuration:

- [Option 1](#option-1---generate-the-base-automation-using-the-in-game-plugin-settings-window) - Use a Long-Lived Access Token to automatically create a base automation using Home Assistant's API
- [Option 2](#option-2---create-a-new-automation-using-the-rocketleague-bakkesbaseyaml-file) - Manually copy and paste the automation YAML to create the base automation
- [JSON Values](#json-values) - Keys and values for the JSON requests



### Option 1 - Generate the base automation using the in-game plugin settings window

1. **Generate a _Long-Lived Access Token_ in Home Assistant**:

   a. Click your username in the bottom left of the Home Assistant web interface
   
   b. Scroll to the bottom under "Long-Lived Access Tokens" and click "Create Token"
   
   c. Give the token a name and press "Ok"
   
   d. Copy the token string. ***It is best to click into the text box and press CTRL-A to select all, and then CTRL-C to copy the selected text***
	
	![HATokenStep1](https://user-images.githubusercontent.com/23534272/234130854-5aafac64-c6b8-47bc-ab5d-90625b864032.png)
    
	![HATokenStep2](https://user-images.githubusercontent.com/23534272/234130913-ce4de667-c4f3-452b-8b1e-2f70fc499f34.png)
	
   You will now have a token similar to this:
   `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI5NTgxMmNlMWIyYmI0OTRhYTIzN2U0NjNiOTIwMmU5MSIsImlhdCI6MTY4MjM2NzE2OSwiZXhwIjoxOTk3NzI3MTY5fQ.9gdUfsDoYXyTPtqi4vIZ0vuRPFZ-fUrSum_4BxEGzcw`
   ***You may want to temporarily paste the code into Notepad or the URL bar***

2. **Use the Template Generator in the plugin's setting window**:


   a. Press F2 in-game `Plugins` tab -> `RocketLeagueAssistant`
   
   b. Expand `Automation Template Generator`
   
   c. Populate the *Home Assistant's Address* field with your Home Assistant instance's URL or IP and populate the *Token* field with the token generated in step 1
   
   d. Click `Generate Automation Base Template`. Your *Home Assistant Web Hook Global URL* field will be automatically populated with the appropiate URL for Home Assistant. 
    
   **The token will be automatically erased from the plugin and config file on game close for security reasons. It is suggested to also delete the token from Home Assistant after automation generation is complete and confirmed.**

   The plugin configuration is done.
    
   ![RLAGenerateAutomation](https://user-images.githubusercontent.com/23534272/234132236-41fd50aa-6467-49c1-b320-dde76c58b682.png)


 You should now have an automation called ***RocketLeague - BakkesGenerated*** in Home Assistant

3. **Populate each condition with the action corresponding with its condition**:

   ![HAEditConditions](https://user-images.githubusercontent.com/23534272/234131005-bc842736-7ef7-4704-ac5e-f133c3adb462.png)


4. **The color values from the JSON request can be used as the colors for your automations**:
	
	a. Select **Call a service** as your action then edit in YAML.
	
This will set the color of your lights to the values of your team's primary color. Populate "LIGHTNAMEHERE" with the corresponding entity ID
```YAML
service: light.turn_on
data:
  rgb_color:
    - "{{ trigger.json.TeamData.PlayersTeam.color.r |int }}"
    - "{{ trigger.json.TeamData.PlayersTeam.color.g |int }}"
    - "{{ trigger.json.TeamData.PlayersTeam.color.b |int }}"
target:
  entity_id: light.LIGHTNAMEHERE
enabled: true
```

This will set the color of your lights to brighter values of your team's primary color (Useful for Goals/Demos). Populate "LIGHTNAMEHERE" with the corresponding entity ID:
```YAML
service: light.turn_on
data:
  rgb_color: >
     {% set r = trigger.json.TeamData.PlayersTeam.color.r | int %}
     {% set g = trigger.json.TeamData.PlayersTeam.color.g | int %}
     {% set b = trigger.json.TeamData.PlayersTeam.color.b | int %}
     {% macro adjust(color) %} {{ color + (255 - color) / 2 }} {% endmacro %}
     {{ [ adjust(r), adjust(g), adjust(b) ] | join(',') }}
  brightness_pct: 100
target:
  entity_id: light.LIGHTNAMEHERE
enabled: true
```

[**More on automations here**](https://www.home-assistant.io/docs/automation/)	

### Option 2 - Create a new automation using the [**RocketLeague-BakkesBase.yaml**](RocketLeague-BakkesBase.yaml) file.

1. Create a new automation
2. Select **Edit in YAML**
3. Paste the contents of [RocketLeague-BakkesBase.yaml](RocketLeague-BakkesBase.yaml)
4. Manually create a Webhook-ID (A Webhook-ID should be treated as a password and should be randomized like one)
5. Edit the actions respectively as shown in [Option 1](#option-1---generate-the-base-automation-using-the-in-game-plugin-settings-window) step 4

### JSON Values

```
x.data /Event data (demo, goal, etc)
x.TeamData.PlayersTeam.color.r /Player's primary color red
x.TeamData.PlayersTeam.color.g /Player's primary color green
x.TeamData.PlayersTeam.color.b /Player's primary color blue
x.TeamData.PlayersTeam.score /Player's team score

x.TeamData.OtherTeam.color.r /OtherTeam's primary color red
x.TeamData.OtherTeam.color.g /OtherTeam's primary color green
x.TeamData.OtherTeam.color.b /OtherTeam's primary color blue
x.TeamData.OtherTeam.score /OtherTeam's score
```

## Home Assistant Scenes Configuration to be Used in Automation(Not Required):

1. Create a new scene corresponding to the scenario (Home Team, Away Team, Demos, etc)
2. Give it a name (and icon/area if you'd like)
3. Add entities/devices to the scene and adjust the colors accordingly
4. Save the scene
5. Create a new, or duplicate the scene.
6. Add entities/devices to the scene and adjust the colors accordingly.
7. Save the scene
8. Repeat

[**More on scene creation here**](https://www.home-assistant.io/integrations/scene/)

[![Open your Home Assistant instance and show the blueprint import dialog with a specific blueprint pre-filled.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgist.github.com%2FGtt1229%2Ffd0ee332ebb3f7eeddaf9f14e0ef6d36)

### Notes

If you manually reload the plugin (through the F6 BakkesMod Console) while in the game, the URL will have to be reentered.

## To Do

* More JSON information
* Probably code clean up and Nullchecks
