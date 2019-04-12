[![Build Status](https://travis-ci.org/SPACE-HAUC/OctopOS.svg?branch=master)](https://travis-ci.org/SPACE-HAUC/OctopOS) [![codecov](https://codecov.io/gh/SPACE-HAUC/OctopOS/branch/master/graph/badge.svg)](https://codecov.io/gh/SPACE-HAUC/OctopOS) [![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg?style=flat)](https://github.com/SPACE-HAUC/OctopOS/blob/master/LICENSE.txt) [![release](http://github-release-version.herokuapp.com/github/SPACE-HAUC/OctopOS/release.svg?style=flat)](https://github.com/SPACE-HAUC/OctopOS/releases)

# OctopOS

A simple data bus for subsystem communication.

## Introduction
OctopOS is a data bus designed to make subsystem inter communications as easy as possible. It exposes a publisher subscriber model, however it's tentacles can be extended as needed to include other communication methods.

If you are working on a subsystem for SPACE HAUC then most of the information you will need can be found in the __Basic Usage__ section of this readme.

## Contents
* __Basic Usage__
    * Includes
    * Publisher
    * Subscriber
* __Advanced Topics__
    * Calling Modules
    * Control Flow
    * Shared Memory Segment
    * Communication Protocal
* __Further Documentation__

## Basic Usage
The basic usage of octopOS is ment to make inter system communication as easy as possible! This section is designed to show you what you can do.

To compile and install run `mkdir build && cd build && cmake .. && make && sudo make install`

### Includes and Linking
OctopOS provides a few libraries. The main ones you will be using are the `publiher` and `subscriber` libraries. All includes follow the format:
```cpp
#include<OctopOS/{lib}.h>
```
i.e
```cpp
#include<OctopOS/subscriber.h>
```

### Publisher
If you would like to publish data from your subsystem for others to read this is the section for you. To create a publisher you only need to construct an instance of the class:
```cpp
publisher<T> pub("topic_name", argv[0]);
```
Since `publisher` is a templated class `T` should be they type of data you are trying to publish. This include custome classes and structs.

The first parameter is also important as that is the name of the topic you wish to publis to. you can have multiple publishers to a single topic if you want. The name can not contain spaces.

If you would like to learn more about the ```argv[0]``` part refer to the __Calling Modules__ section of this readme. A sample usage would be:
```cpp
publisher<int> pub("test", argv[0]);
```

Now that we have a publisher to publish data all we have to do is:
```cpp
int data = 5;
pub.publish(data);
```
That is all there is to it! Don't forget to include `publisher.h`
### Subscriber
Ihis section gives instructions on how to read data from a topic using a subscriber. Due to there implementation subscribers require a bit more setup than publishers.

First you will want to create a thread that listens for events from octopOS. __YOU ONLY NEED ONE OF THESE THREADS FOR ALL SUBSCRIBERS IN THE PROCESS!__ This is done by:
```cpp
pthread_t wait_for_subscrber_event;

if (pthread_create(&wait_for_subscrber_event, NULL, subscriber_manager::wait_for_data, NULL)) {
    exit(-1);  // or handle the error as you see fit (this is almost always terminal)
}
```

Do not wait for this thread to join as it never will. Once this is complete we can set up our subscribers just like out publishers. First create an instance of the class with:
```cpp
subscriber<T> sub("topic_name", argv[0]);
```

Again this is very similar to `publiser` where `T` is the type of data, and the first paramater is the topic name you would like to get data from. If we wanted to subscribe to our topic from the publisher section it would look like:
```cpp
subscriber<int> sub("test", argv[0]);
```

Once we have created a subscriber to get data we simple do:
```cpp
int x = sub.get_data();
```

A note is that this call __WILL BLOCK__ your thread if there is no data to be had. It will unblock once data is available, but is advisable to read data from a secondaty thread so your main thread can continue execution.

Remember to include `subscriber.h`

## Advanced Topics
This section is for those who want to understand how octopOS works under the hood. If you just want to use octopOS see the above __Basic Usage__ section.

### Calling Modules
When a module is created registered with octopOS, it forks and execs the independant binary of that module. It passes the message key that it expects the module to communicate on as `argv[0]`, that is why you pass `argv[0]` to the constructor of classes inherited from tentical as it constructs itself on top of this message bus.

All modules, and the number of modules need to be known befor runtime as it stands right now. These are defined in `utility.h` and compiled into the main executable. In the future we hope to eliminate this need.

### Control Flow
This section aims to talk through the basic flow of the lifetime of a octopOS topic.
### Shared Memory Segment
### Communication Protocal

## Full Documentation
Full documentation is available [here](https://space-hauc.github.io/OctopOS/)
