// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#include "../include/tentacle.h"

tentacle::tentacle(key_t msg_key) {
    if ( (message_que = msgget(msg_key, 0600)) < 0 ) {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Unable to connect tentacle to msg bus");
    }
    if (shared_data == NULL) {
        if (!(shared_data = connect_shm())) {
            throw std::system_error(
                errno,
                std::generic_category(),
                "Unable to connect tentacle to shm_object");
        }
    }
}

std::pair<long, std::string> tentacle::read(long type, bool block, bool under) {  // NOLINT Must use long
    message_buffer my_buffer;
    std::pair<long, std::string> return_value(0, "");                             // NOLINT Must use long
    int rc;

    type *= (under)? -1 : 1;  // sets to negative if we intend on reading under

    if (!block) {
        rc = msgrcv(message_que, &my_buffer,
            sizeof(my_buffer.text), type, IPC_NOWAIT);  // runs if not waiting
    } else {
        rc = msgrcv(message_que, &my_buffer,
            sizeof(my_buffer.text), type, 0);  // blocking call
    }

    if (rc >= 0) {
        return_value.first = my_buffer.type;
        return_value.second = my_buffer.text;
    }
    return return_value;
}

bool tentacle::write(long type, std::string data) {                               // NOLINT Must use long
    message_buffer my_buffer;
    bool return_value = false;

    my_buffer.type = type;
    strncpy(my_buffer.text, data.c_str(), data.size());
    my_buffer.text[data.size()] = '\0';

    int rc = msgsnd(message_que, &my_buffer,
        sizeof(my_buffer.text), IPC_NOWAIT);

    if (rc >= 0) {
        return_value = true;
    }

    return return_value;
}

bool tentacle::write(std::pair<long, std::string> pair) {                         // NOLINT Must use long
    return write(pair.first, pair.second);
}

long tentacle::getTempId(role_t role) {                                           // NOLINT Must use long
    long return_value = 0;                                                        // NOLINT Must use long
    static ushort rand_seed[3] = { 0 };

    switch (role) {
      default:
        break;
      case SUBSCRIBER:
        return_value |= SUB_BIT;
        break;
    }
    return_value |= TEMP_BIT;

    id_lock.lock();
    if (!rand_seed[0])
        initRand(rand_seed);

    return_value |= nrand48(rand_seed) & 0x1fffffff;
    id_lock.unlock();

    return return_value;
}

void tentacle::initRand(ushort *rand_seed) {
    struct timeval rand_time;
    gettimeofday(&rand_time, (struct timezone*)NULL);

    rand_seed[0] = (ushort) rand_time.tv_sec;
    rand_seed[1] = (ushort) (rand_time.tv_usec >> 16);
    rand_seed[2] = (getpid());
}

std::mutex tentacle::id_lock;
intptr_t* tentacle::shared_data = NULL;
int tentacle::message_que;
