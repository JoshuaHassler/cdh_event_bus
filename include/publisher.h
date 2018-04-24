// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#ifndef INCLUDE_PUBLISHER_H_
#define INCLUDE_PUBLISHER_H_

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>

#include "tentacle.h"
/*!
 * This class is used to publish data to a topic specified on construction. The
 * template parameter is the data type of the topic. Inherites from tentacle
 * for IPC communication.
 */
template <typename T>
class publisher : public tentacle {
 public:
    /*!
     * creates a publisher object and registers it with octopOS.
     * @param _name The name of the topic to publish to.
     * @param message_key The key for the message bus to create a tentacle on.
     */
    publisher(std::string _name, key_t message_key):
        tentacle(message_key), name(_name) {
        id = getTempId(tentacle::role_t::PUBLISHER);

        write(CREATE_PUB, std::to_string(id) + " " +
            std::to_string(sizeof(T)) + " " + name);

        std::pair<long, std::string> response;                                    // NOLINT

        response = read(id);

        std::istringstream iss(response.second);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
            std::istream_iterator<std::string>{}};
        if ((semid = semget(std::stoi(tokens[1]), 2, 0600)) < 0) {
            throw std::system_error(
                errno,
                std::generic_category(),
                "Unable to get r/w sem");
        }

        rw = reinterpret_cast<shm_object*>(shared_data + std::stoi(tokens[0]));

        data_ptr = reinterpret_cast<T*>(rw + 1);
        id = std::stoi(tokens[2]);
    }

    /*!
     * Updates data in shared memory segment, then tells octopOS to notify
     * subscribers of the newly published data.
     * @param data The data to be published
     * @return true if data published successfully otherwise false.
     */
    bool publish(T data) {
        bool return_value = false;
        if (p(semid, 1) >= 0) {
            rw->rw_array[1] += 1;
            if (rw->rw_array[1] == 1) {
                p(semid, 3);  // TODO(JoshuaHassler) check for success
            }

            v(semid, 1);
        }

        if (p(semid, 2) >= 0) {
            *data_ptr = data;
            return_value = true;
            write(PUBLISH_CODE, name);
            v(semid, 2);
        }

        if (p(semid, 1) >= 0) {
            rw->rw_array[1] -= 1;
            if (rw->rw_array[1] == 0)
                v(semid, 3);  // TODO(JoshuaHassler) check for success
            v(semid, 1);
        }
        return return_value;
    }

 private:
    /*! unique identifier of publisher */
    long id;                                                                      // NOLINT
    /*! name of topic to publish to */
    std::string name;
    /*! id of semaphore controlling shared data lock */
    int semid;
    /*! pointer to reader writer pointers for data access controll in shared
       memory. */
    shm_object* rw;
    /*! pointer to shared data struct */
    T* data_ptr;
};

#endif  // INCLUDE_PUBLISHER_H_
