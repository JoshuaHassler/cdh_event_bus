#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <exception>
#include <stdexcept>

#include "octopos.h"
#include "subscriber.h"
#include "publisher.h"

BOOST_AUTO_TEST_CASE(getInstance) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
}

BOOST_AUTO_TEST_CASE(subscriberConstructor) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    pthread_t tmp;
    int x = 0;
    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, &x)) {
        exit(-1);
    }
    BOOST_REQUIRE_NO_THROW(subscriber<int>("test", MSGKEY));
}

BOOST_AUTO_TEST_CASE(publisherConstructor) {
    BOOST_REQUIRE_NO_THROW(octopOS::getInstance());
    pthread_t tmp;
    int x = 0;
    if (pthread_create(&tmp, NULL, octopOS::listen_for_child, &x)) {
        exit(-1);
    }
    BOOST_REQUIRE_NO_THROW(publisher<int>("test", MSGKEY));
}
