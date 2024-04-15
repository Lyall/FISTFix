# FIST: Forged In Shadow Torch Fix
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)<br />
 <iframe src="https://yikuansun.github.io/patreonbutton/widget.html?username=wintermance" height="68" width="298" scrolling="no" data-theme="light" style="border: 0; border-radius: 10px; overflow: hidden"></iframe><br/>
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/FISTFix/total.svg)](https://github.com/Lyall/FISTFix/releases)

This is a fix that adds custom resolutions, ultrawide support and more to FIST: Forged In Shadow Torch.<br />

## Features
- Custom resolution support that integrates with the game's menus.
- Ultrawide/narrow aspect ratio support.
- Correct FOV at any aspect ratio.
- Option to increase gameplay FOV.
- Ability to override DLSS "Auto" quality setting.

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

| |
|:--:|
| Gameplay |

| |
|:--:|
| Cutscene |

## Credits
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.
