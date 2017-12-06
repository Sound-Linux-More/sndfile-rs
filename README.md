# SoundFile ReSpectr.

ReSpectr wav file.

## Usage

 sndfile-rs [options] input.wav output.wav

## Options:

-m str	mode (string, optional, default = copy):  
    copy - copy wav file,  
    drs - diff respectr wav file (main function),  
    crs - complite respectr wav filter,  
    rs - respectr wav filter,  
    pow2 - power 2 wav file,  
    sqrt - sqrt wav file,  
    diff - differencial wav file,  
    int - integral wav file.  

-r	reverse filter (bool, optional, default = false)

-h	help

## Exampe:

 sox -S -v 0.5 sample.mp3 sample.mp3.wav  
 sndfile-rs -m drs sample.mp3.wav sample.mp3crs.wav  
 sox -S --norm sample.mp3crs.wav sample.mp3crsnorm.wav  
 faac -q 256 sample.mp3crsnorm.wav  
 MP4Box -add sample.mp3crsnorm.aac -copy sample.m4a  
 bash copymetascr.sh sample.mp3 sample.m4a # ! using ffmpeg  
 rm *.wav *.aac  

## Sample:

Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3

sox -S Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3 Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3.wav

Input File     : 'Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3'  
Channels       : 2  
Sample Rate    : 44100  
Precision      : 16-bit  
Duration       : 00:02:42.10 = 7148478 samples = 12157.3 CDDA sectors  
File Size      : 2.59M  
Bit Rate       : 128k  
Sample Encoding: MPEG audio (layer I, II or III)  
Comments       :   
Title=Мёртвый воин (TorchProj&Черника)  
Artist=Аксеаносилас  
Album=Ряженка  
Tracknumber=01/03  
Year=2008  
Genre=(20)  
Discnumber=1  

sndfile-rs -m drs Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3.wav Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav  
    Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3.wav -> Axeanosilas-2008-01-Myortviy_voin-TorchProjandChernika.mp3drs.wav ok [0.013643]

## COPYRIGHT

Copyright 2017 zezdochiot.  
 All rights reserved.