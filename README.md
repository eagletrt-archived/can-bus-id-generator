# Can-bus-id-generator

This software was developed from E-agleTRT for CAN-BUS IDs generation. In order to find optimezed IDs for Fenice (season 2019-2020 electric car) architecture this software is based on masks, filters and messages priority. This software generates automatically an header file inportable in different devices like STM32F4, STM32F7, Raspberry and others. Due to the the fact the architecture is composed by very different devices, a common file is needed in order to make all of them communicating with each others.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

This software was implemented with `QtCreator` in `C++` for future graphic implementations (mainly to avoid the problem of writing all the `.json` files for the first times).

### Installing

The following steps give you the possibility to run and modify the code in order to implement it for your requirements.

Get QtCreator software from:

```
https://www.qt.io/
```

and follow the installation steps from the website guide:

```
https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5
```

Now we are ready for coding on QtCreator.

If you have not cloned yet one of the other repositories of `eagletrt` than clone one of them with the following comand (if you already did it skip this step):

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

Now you have the cloned repo of your interest with a linked folder inside to `can-bus-id-generator` repo. Every time you need to refresh your linked `can-bus-id-generator` just type:

```
git submodule update
```

and your files will be update to the newest version of `can-bus-id-generator`.

## Compile and Run for VScode

After the previous steps, if you want to compile and run this project on `VScode` just import it and than in `deploy.sh` file you have to change the `QMAKE` path to your `QtCreator` qmake file.

```
QMAKE="/opt/Qt/5.12.0/gcc_64/bin/qmake"
```

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
- `id` is a random one. It will be generated from the `can-bus-id-generator`;
- `send_to` defines to which devices the message will be sent;
- `to_mod` indicates if those messages will be modified `1` or not `0` just if you want to maintained some IDs as you set in the `.json` files;
- `byte0-7` specificies the payload of your message.

The software gives you the possibility to write multiple `.json` files in order to separate each class of messages (ex: the start-up messages from the sensors messages). Just insert into the code the file you want to open in the files handler section with:

```
files.push_back("../import/sensors.json");
```

Than run the code with the `green play button` in QtCreator, a terminal will display the results and an header file is generated in your project folder:

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

This file contains all the generated IDs based on the masks you put at the initial part of the code.

## How to implement the CAN-BUS masks and filters

Masks and filters in CAN-BUS protocol are very important for a large amount of messages. They are used in order to avoid collisions and make the controllers less busy. With the hardware masks the messages not sent will arrive only to the controllers that need those datas.

Masks and filters work only on the IDs of the message in this way:

- Masks:
  - If the bit is 0 the bit of the ID at the same position is passing through with any control;
  - If the bit is 1 the bit of the ID at the same position is controlled with the filter;
- Filters:
  - If all the bits to control (setted by the mask) are equal to the corresponding bits of the filter than the message pass through;
  - If just one of the bits to control (setted by the mask) is different from its corresponding bit of the filter than the message is canceled.

So the general idea is to split the 11 bits of the IDs in two parts:

- The MSBs representing the priority of the message;
- The LSBs containing the informations of where the message will go. This part is made by the `OR` of the masks of the device to reach.

This separation generates a trade-off between how many messages is possible to have for each masks combination (MSB) and how many devices are receiving (LSB). In our case we separeted in 5 MSB and 6 LSB so we can send to 6 different devices and having the 32 messages for each different combination of the 6 LSB.

As it is possible to see, this code is based on binary operations, so in order to generate the proper IDs you have to think to your own masks and filters best solution. In our case it is:

```
mask.tel = 0b00000000000;       //0 TEL is catching everything
mask.acu = 0b00000000001;       //1
mask.bms_hv = 0b00000000010;    //2
mask.contr = 0b00000000100;     //4
mask.atc = 0b00000001000;       //8
mask.sw = 0b00000010000;        //16
mask.bms_lv = 0b00000100000;    //32
```

and

```
filter.tel = 0b00000111111;     //64
filter.acu = 0b00000111111;     //64
filter.bms_hv = 0b00000111111;  //64
filter.contr = 0b00000111111;   //64
filter.atc = 0b00000111111;     //64
filter.sw = 0b00000111111;      //64
filter.bms_lv = 0b00000111111;  //64
```

So all LSBs is checked by masks and filters in order to optimize the CAN-BUS architecture and MSBs part defines how much priority the message will have. At the end, just to be sure, if messages with the same ID exist, they are rearranged.
This method avoid the possibility of collision of the messages with a more robust and faster approach.

## Built With

- [QtCreator](https://www.qt.io/) - QtCreator
- [Nlohmann](https://github.com/nlohmann/json) - JSON for Modern C++

## Versioning

This is the first version of `can-bus-id-generator`. Probably a new version will be deployed later with graphics contnet in order to make `.json` file easier to write and the header easier to read.

## E-agleTRT contacts

For issues and questions please contact us at fsae.unitn@gmail.com, for tech questions matteo.spadetto-1@studenti.unitn.it and for GitHub problems matteo.bonora@studenti.unitn.it

- **E-agle Trento Racing Team GitHub** - [eagletrt](https://gith8ub.com/eagletrt)

- See also our [website](https://eagletrt.it/).
