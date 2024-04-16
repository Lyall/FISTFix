# FIST: Forged In Shadow Torch Fix
[![Patreon-Button](https://github.com/Lyall/FISTFix/assets/695941/19c468ac-52af-4790-b4eb-5187c06af949)](https://www.patreon.com/Wintermance) [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)<br />
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/FISTFix/total.svg)](https://github.com/Lyall/FISTFix/releases)

This is a fix for FIST: Forged In Shadow Torch that adds custom resolutions, ultrawide support and more.<br />

## Features
- Custom resolution support that integrates with the game's menus.
- Ultrawide/narrow aspect ratio support.
- Correct FOV at any aspect ratio.
- Option to constrain HUD to 16:9.
- Option to increase gameplay FOV.
- Ability to override DLSS "Auto" quality setting.
- Intro skip.

## Installation
- Grab the latest release of FISTFix from [here.](https://github.com/Lyall/FISTFix/releases)
- Extract the contents of the release zip in to the the game's binary folder.<br />(e.g. "**steamapps\common\FISTGame**" for Steam).

### Steam Deck/Linux Additional Instructions
🚩**You do not need to do this if you are using Windows!**
- Open up the game properties in Steam and add `WINEDLLOVERRIDES="dsound=n,b" %command%` to the launch options.

## Configuration
- See **FISTFix.ini** to adjust settings for the fix.

## Known Issues
Please report any issues you see.
This list will contain bugs which may or may not be fixed.

- At the moment, this is only tested on the Epic Games Store version. If you run in to any issues with your specific version, please create an issue.
## Screenshots

| ![ezgif-2-5036ee6527](https://github.com/Lyall/FISTFix/assets/695941/18dc4094-56f6-4b13-84ac-2d044ecf112d) |
|:--:|
| Gameplay |

|![ezgif-5-870a5852f0](https://github.com/Lyall/FISTFix/assets/695941/7b3a6a12-cac0-4a37-ab63-81aab8739734) |
|:--:|
| Cutscene |

## Credits
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.
