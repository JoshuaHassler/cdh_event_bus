// Copyright 2017 Space HAUC Command and Data Handling
/*!
 * @file
 */
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>

#include "../include/octopos.h"
#include "../include/tentacle.h"

#include "../include/subscriber.h"  // PLEASE DONT FORGET ME
#include "../include/publisher.h"

int main(int argc, char const *argv[]) {
    pid_t pid;

    try {
        octopOS::getInstance();
    } catch (std::system_error e) {
        std::cerr << e.what();
        throw;
    }
/*
    if (!(pid = fork())) {
        execl("../../testModule/build/bin/testModule",
            std::to_string(MSGKEY).c_str(), (char*)0);
    }
*/
    int x = 0;
    pthread_t tmp, subThread;

    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, &x)) {
        exit(-1);
    }

    if (pthread_create(&subThread, NULL,
        subscriber_manager::wait_for_data, &x)) {
        exit(-1);
    }

/*
    // test.write(1, "This is a test string");
    // sleep(1);

    // tentacle test(MSGKEY);
    subscriber<int> sub("test", MSGKEY);
    // std::cout << sub.listen_to_topic(4) << std::endl;
    // std::cout << test.read(4).second << std::endl;

*/

    subscriber<int> sub("test", MSGKEY);

    publisher<int> pub("test", MSGKEY);

    pub.publish(578);

    std::cout << "Data: " << sub.get_data() << std::endl;
    /*

    wait(NULL);

    // octopOS::getInstance().create_new_topic("test", sizeof(int));
    // octopOS::getInstance().create_new_topic("foo", sizeof(int));
    //
    // std::cout << octopOS::getInstance().subscribe_to_topic("test", 0) << std::endl;
    // std::cout << octopOS::getInstance().subscribe_to_topic("test", 1) << std::endl;
    // std::cout << octopOS::getInstance().subscribe_to_topic("foo", 1) << std::endl;
    // octopOS::getInstance().propagate_to_subscribers("test");
    // octopOS::getInstance().propagate_to_subscribers("foo");

*/

    return 0;
}
