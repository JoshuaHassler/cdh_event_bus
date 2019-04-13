// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#ifndef INCLUDE_TENTACLE_H_
#define INCLUDE_TENTACLE_H_

#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <cstdlib>
#include <string>
#include <utility>
#include <mutex>                                                                  // NOLINT

#include "utility.h"                                                              // NOLINT

/*!
 * class provides a wrapper for system V IPC message buss. This allows for easy
 * data communication between child modules and octopOS.
 */
class tentacle {
    /*! allows octopOS to see its private. This is needed in a few use cases */
    friend class octopOS;

 private:
    /*! guards access to rand48 for temp id generation. */
    static std::mutex id_lock;

    /*! gives a static initialization of rand48 for temp id generation */
    static void initRand(ushort* rand_seed);

 protected:
    /*! id of system V message queue */
    static int message_que;

    /*! a pointer to the shared memory segment */
    static intptr_t* shared_data;

    /*! enum for role discrimination. used to generate temp/perm ids */
    enum role_t {SUBSCRIBER, PUBLISHER };  // PROTECT ME SQUIRE

    /*!
     * generated a tem id for initial communication with octopOS.
     * @param role The role that the id should be generated for.
     * @return An unique temp id.
     */
    static long getTempId(role_t role);                                           // NOLINT

 public:
    /*!
     * constructs a tentacle. Attaches shared memory segment to process if it
     * is not already attached.
     * @param msg_key message bus to attach tentacle to.
     */
    explicit tentacle(key_t msg_key);

    /*!
     * reads a message from the message bus.
     * @param type The type(id) of message to read from the bus.
     * @param block Whether or not the read call should block thread untill
     * success, or just return. Set to false to do a non-blocking read.
     * @param under Set to true if you wish to read the range of ids under that
     * type specified. This should only be used by octopOS.
     * @return A pair where first is the type of the message read, and second is
     * the contents of the message as a std::string.
     */
    static std::pair<long, std::string> read(long type, bool block = true,        // NOLINT
                                      bool under = false);

    /*!
     * writes data to the message bus.
     * @param type The type to publish the message under
     * @param data The message to be sent.
     * @return true if write was successfull, otherwise false.
     */
    bool write(long type, std::string data);                                      // NOLINT

    /*!
     * a wrapper for normal write. allows to pass a pair instead.
     * @param pair The data to write. first becomes type and second beecomes
     * data
     * @return true if write was successfull, otherwise false.
     */
    bool write(std::pair<long, std::string> pair);                                // NOLINT
};

// Size-invariant string data type for publishing
class OctoString {
 private:
    char buf[MSGLEN - 1];

 public:
    OctoString() {}
    explicit OctoString(const std::string &value) {
        if (value.size() > (MSGLEN - 2)) {
            throw std::range_error("Given string is too large to publish.");
        }
        strcpy(buf, value.c_str());  // NOLINT - complains about strcpy?
    }
    OctoString& operator=(const std::string &other) {
	    strcpy(buf, other.c_str());  // NOLINT - complains about strcpy?
        return *this;
    }
    OctoString& operator=(const OctoString &other) {
        strcpy(buf, other.buf);  // NOLINT - complains about strcpy?
        return *this;
    }
    std::string get() {
        return std::string(buf);
    }

    operator std::string() {
        return std::string(buf);
    }
};

#endif  // INCLUDE_TENTACLE_H_
