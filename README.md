# Can-bus-id-generator

This software was developed from E-agleTRT in order to generate CAN-BUS IDs. In order to find optimezed IDs for Fenice (season 2019-2020) architecture the software is based on masks, filters and messages priority. This software generates automatically other code usable in different devices like STM32F4, STM32F7, Raspberry and others. Due to the the fact the architecture is composed by very different devices, a common file is needed in order to make all of them communicating with each others.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

This software was implemented with `QTCreator` in `C++` for feature graphic implementations (writing the `.json` files could be a mass the first times).

### Installing

The following steps give you the possibility to run and modify the code in order to implement for your requirements.

Get QTCreator software from:

```
https://www.qt.io/
```

and follow the installation steps from the website guide:

```
https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5
```

Now we are ready for coding on QTCreator.

If you have not clone one of the other repositories of `eagletrt` user than clone one of them with the following comand. If you did it skip this step:

```
git clone https://github.com/eagletrt/the-repo.git
```

Then due to the fact this repo uses sub-modules you have to perform:

```
git submodule init
```

and

```
git submodule update
```

Now you have the cloned repo of your interest with inside a linked folder to the `can-bus-id-generator`. Every time you need to refresh your linked `can-bus-id-generator` just type:

```
git submodule update
```

and your file will be update to newest version of can-bus-id-generator.

## Running the software

To run the software different steps are required. First of all you have to write the `.json` files with your own messages in this way:

```
 {
    "array":[{
        "name": "ENC_L_FROM_ACU",
        "priority": 20,
        "device": "ACU",
        "id": 10,
        "send_to": ["SW", "CONTR"],
        "to_mod": 1,
        "byte0": "speed/256",
        "byte1": "speed%256",
        "byte2": "sign",
        "byte3": "prescaler",
        "byte4": "meters/256",
        "byte5": "meters%256",
        "byte6": "error",
        "byte7": ""
    },{
        "name": "ENC_R_FROM_ACU",
        "priority": 20,
        "device": "ACU",
        "id": 10,
        "send_to": ["SW", "CONTR"],
        "to_mod": 0,
        "byte0": "speed/256",
        "byte1": "speed%256",
        "byte2": "sign",
        "byte3": "prescaler",
        "byte4": "meters/256",
        "byte5": "meters%256",
        "byte6": "error",
        "byte7": ""
    }]
    }
```

Where:

- `name` is the name of your message;
- `priority` is the desired priority you want for that message;
- `device` is the sender;
- `id` is a random one. It will be generated from then from software;
- `send_to` defines to which devices the message will be sent;
- `to_mod` indicates if that messages will be modified `1` or not `0` just if you want to maintenied some IDs as you set in the `.json` files;
- `byte0-7` specificies the payload of your messages.

The software gives you the possibility to write multiple `.json` files in order to separate each type of message (ex: the start-up messages from the sensors messages). Just insert into the code the file you want to open in the files handler section with:

```
files.push_back("../import/sensors.json");
```

Than run the code with the `green play button` in QTCreator, a terminal will display the results and header file is generated in your project folder:

```
E-agleTRTCanMsg.h
```
```
#ifndef _EAGLE_TRT_CAN_MSG_H
#define _EAGLE_TRT_CAN_MSG_H

#define ENC_L_FROM_ACU 20 	 //ACU sending to: SW CONTR --> B0: speed/256 | B1: speed%256 | B2: sign | B3: prescaler | B4: meters/256 | B5: meters%256 | B6: error | B7: 
#define ENC_R_FROM_ACU 10 	 //ACU sending to: SW CONTR --> B0: speed/256 | B1: speed%256 | B2: sign | B3: prescaler | B4: meters/256 | B5: meters%256 | B6: error | B7: 

#endif
```

This file contains all the generated IDs based on masks you put at the initial part of the code. This code is based on binary operation in order to set the proper IDs so you have to think to your own masks best solution. In our case it is:

```
mask.tel = 0b00000000000;       //0
mask.acu = 0b00000000001;       //1
mask.bms_hv = 0b00000000010;    //2
mask.contr = 0b00000000100;     //4
mask.atc = 0b00000001000;       //8
mask.sw = 0b00000010000;        //16
mask.bms_lv = 0b00000100000;    //32
```
CAN-BUS protocol is not explained here but these masks are the results of a study on it in order to find the best approach for our arch. This method avoid the possibility of collision of messages with a more robust and faster approach.

## Built With

- [QTCreator](https://www.qt.io/) - QTCreator
- [Nlohmann](https://github.com/nlohmann/json) - JSON for Modern C++

## Versioning

This is the first version for the `can-bus-id-generator`. Probably a new version will be deployed later with graphics contnet in order to make `.json` file writing and the header reading easier.

## E-agleTRT contacts
For issues and questions please contact us fsae.unitn@gmail.com, for tech matteo.spadetto-1@studenti.unitn.it and for GitHub problems matteo.bonora@studenti.unitn.it

- **E-agle Trento Racing Team GitHub** - _eagletrt user_ - [eagletrt](https://github.com/eagletrt)

- See also our [website](https://eagletrt.it/).
