# Sample sketch for adtb7m

## 概要

オプティカルトラックボールモジュール ADTB7M の実装サンプル on Arduino Pro Micro。

## 接続

__!!注意!! レベル変換して接続してください__  
[レベル変換基板](https://github.com/sekigon-gonnoc/LevelConverterForTrackballModule) を利用すると楽です。

|ADTB7M|ProMicro|
|--|--|
|SCLK|D16|
|SDIO|D10|
|MOTSWK|N.C.|
|GND|GND|
|VDD|VCC|

## 資料

“ADTB7M” オプティカルトラックボールモジュール  
https://bit-trade-one.co.jp/selfmadekb/adtb7m/

PAW3204OAセンサースペックシート  
https://bit-trade-one.co.jp/forUser/paw3204spec.pdf
