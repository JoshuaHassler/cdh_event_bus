// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <exception>
#include <stdexcept>
#include <utility>
#include <string>

#include "../include/octopos.h"
#include "../include/subscriber.h"
#include "../include/publisher.h"

BOOST_AUTO_TEST_CASE(getInstance) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
}

BOOST_AUTO_TEST_CASE(subscriberConstructor) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    pthread_t tmp;
    int *xptr = NULL;
    xptr = (int*)malloc(sizeof(int));  // NOLINT
    *xptr = 0;
    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, xptr)) {
        exit(-1);
    }
    BOOST_REQUIRE_NO_THROW(subscriber<int>("test", MSGKEY));
}

BOOST_AUTO_TEST_CASE(publisherConstructor) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    pthread_t tmp;
    int *xptr = NULL;
    xptr = (int*)malloc(sizeof(int));  // NOLINT
    *xptr = 0;
    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, xptr)) {
        exit(-1);
    }
    BOOST_REQUIRE_NO_THROW(publisher<int>("test", MSGKEY));
}

BOOST_AUTO_TEST_CASE(getPublishedData) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    pthread_t tmp, sub_listener;
    int answer = 5;
    int x = 0;
    int *xptr = NULL;
    xptr = (int*)malloc(sizeof(int));  // NOLINT
    *xptr = x;
    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, xptr)) {
        exit(-1);
    }
    if (pthread_create(&sub_listener, NULL, subscriber_manager::wait_for_data,
        &x)) {
        exit(-1);
    }

    publisher<int> pub("test", MSGKEY);
    subscriber<int> sub("test", MSGKEY);

    pub.publish(answer);

    BOOST_REQUIRE(answer == sub.get_data());

    pthread_cancel(tmp);
    pthread_cancel(sub_listener);
}

BOOST_AUTO_TEST_CASE(tentacleWritePair) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    tentacle tent(MSGKEY);
    BOOST_REQUIRE_NO_THROW(
        tent.write(std::pair<long, std::string>(100, "test")));  // NOLINT
}

BOOST_AUTO_TEST_CASE(multiPublisher) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    pthread_t tmp;
    int *xptr = NULL;
    xptr = (int*)malloc(sizeof(int));  // NOLINT
    *xptr = 0;
    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, xptr)) {
        exit(-1);
    }
    BOOST_REQUIRE_NO_THROW(publisher<int>("test", MSGKEY));
    BOOST_REQUIRE_NO_THROW(publisher<int>("test", MSGKEY));
}
