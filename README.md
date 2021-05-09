![GitHub release (latest by date)](https://img.shields.io/github/v/release/Sound-Linux-More/sndfile-rs)
![GitHub Release Date](https://img.shields.io/github/release-date/Sound-Linux-More/sndfile-rs)
![GitHub repo size](https://img.shields.io/github/repo-size/Sound-Linux-More/sndfile-rs)
![GitHub all releases](https://img.shields.io/github/downloads/Sound-Linux-More/sndfile-rs/total)
![GitHub](https://img.shields.io/github/license/Sound-Linux-More/sndfile-rs)

# SoundFile ReSpectr.

ReSpectr wav file.

## Usage

```
sndfile-rs [options] input.wav output.wav
```

## Options:

```
-m str  mode (string, optional, default = copy):  
        copy - copy wav file,  
        drs - diff respectr wav file (main function),  
        crs - complite respectr wav filter,  
        rs - respectr wav filter,  
        pow2 - power 2 wav file,  
        sqrt - sqrt wav file,  
        kalman - kalman filter wav file,  
        diff - differencial wav file,  
        int - integral wav file.  

-r      reverse filter (bool, optional, default = false)

-h      help
```

## Exampe:

```
sox -S -v 0.5 sample.mp3 sample.mp3.wav  
sndfile-rs -m drs sample.mp3.wav sample.mp3crs.wav  
sox -S --norm sample.mp3crs.wav sample.mp3crsnorm.wav  
faac -q 256 sample.mp3crsnorm.wav  
MP4Box -add sample.mp3crsnorm.aac -copy sample.m4a  
bash copymetascr.sh sample.mp3 sample.m4a # ! using ffmpeg  
rm *.wav *.aac  
```

## Sample:

_Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3_

```
bash sox-spectrogram-1.sh Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3
```

![Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3-spectrogram.png](https://raw.githubusercontent.com/zvezdochiot/sndfile-rs/master/images/Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3-spectrogram.png)

```
sox -S Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3 Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3.wav
```

_Input File     : 'Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3'_  
_Channels       : 2_  
_Sample Rate    : 44100_  
_Precision      : 16-bit_  
_Duration       : 00:02:42.10 = 7148478 samples = 12157.3 CDDA sectors_  
_File Size      : 2.59M_  
_Bit Rate       : 128k_  
_Sample Encoding: MPEG audio (layer I, II or III)_  
_Comments       :_  
_Title=Мёртвый воин (TorchProj&Черника)_  
_Artist=Аксеаносилас_  
_Album=Ряженка_  
_Tracknumber=01/03_  
_Year=2008_  
_Genre=(20)_  
_Discnumber=1_  

```
sndfile-rs -m drs Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3.wav Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav  
```
_Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3.wav -> Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav ok [0.013643]_

```
bash sox-spectrogram-1.sh Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav
```

![Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav-spectrogram.png](https://raw.githubusercontent.com/zvezdochiot/sndfile-rs/master/images/Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav-spectrogram.png)

## COPYRIGHT

Public Domain Mark 1.0  
 No Copyright
