# BattleSub
Will eventually become a two player 2D submarine game with some fluid dynamics.
BattleSub is using Magnum Graphics, Box2D, EnTT, and ImGUI:

[![Build Status](https://travis-ci.com/bfeldpw/battlesub.svg?branch=master)](https://travis-ci.com/bfeldpw/battlesub)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/5aa2f9b18121497cbe9ec07c610a08bd)](https://www.codacy.com/gh/bfeldpw/battlesub/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bfeldpw/battlesub&amp;utm_campaign=Badge_Grade)
[![Last commit](https://img.shields.io/github/last-commit/bfeldpw/battlesub)

At the moment, there is just code cleanup. A proper readme will be written soon.
To build and install after cloning run
```bash
cd scripts
./build_dependencies && ./build
```
from BattleSub directory.

If successful, run BattleSub using `./run` or `/<path>/<to>/scripts/run` if in another directory.

## Screenshots

Final buffer composition with debug GUI:
![alt text](screenshots/Screenshot_20210219_172850.png?raw=true)
Velocity field:
![alt text](screenshots/Screenshot_20210219_172911.png?raw=true)
Pressure field:
![alt text](screenshots/Screenshot_20210219_173001.png?raw=true)
Heightmap (Background):
![alt text](screenshots/Screenshot_20210219_173152.png?raw=true)
Heightmap distorted by velocity field:
![alt text](screenshots/Screenshot_20210219_173212.png?raw=true)
Velocity vectors (sub-sampled):
![alt text](screenshots/Screenshot_20210219_173237.png?raw=true)
Velocity probes:
![alt text](screenshots/Screenshot_20210219_173255.png?raw=true)

