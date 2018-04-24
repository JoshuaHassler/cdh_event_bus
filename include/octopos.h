// Copyright 2017 Space HAUC Command and Data Handling
// This file is part of octopOS which is released under AGPLv3.
// See file LICENSE.txt or go to <http://www.gnu.org/licenses/> for full
// license details.

/*!
 * @file
 */
#ifndef INCLUDE_OCTOPOS_H_
#define INCLUDE_OCTOPOS_H_

#include <signal.h>

#include <unordered_map>
#include <mutex>                                                                  // NOLINT
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <iterator>
#include <memory>
#include <utility>

#include "utility.h"
#include "tentacle.h"

/*!
 * octopOS is the main control class. It is singleton so there can only ever be
 * one instance. octopOS implements a light weight data bus on top of the IPC
 * primatives. On destruction, or a deadly signal
 * octopOS will free these resources befor terminating the program
 */
class octopOS {
 public:
    /*!
     * getInstance returns a reference to the single static instance of octopOS.
     * @return reference to running octopOS instance
     */
    static octopOS& getInstance() {
        static octopOS instance;
        return instance;
    }

    /*! copy constructor deleted to prevent accidental copying of class */
    octopOS(octopOS const&) = delete;

    /*! assignment operator deleted to prevent accidental copying of class */
    void operator=(octopOS const &) = delete;

    /*!
     * The constructor creates the neccecary shared memory segments,
     * semaphors, and IPC message busses for the rest of the system to build on.
     */
    ~octopOS();

    /*!
     * create_new_topic adds a new topic to octopOS that can be published to or
     * subscreibed from.
     * @param name The name of the new topic (will be used as it's identifier)
     * @param size The amount of space need to store the data object associated
     * with the topic.
     * @return a pair where the first is the offset of the data object in the
     * shared memory segment, and the second is the key for the associated
     * semaphore
     */
    std::pair<unsigned, key_t> create_new_topic
        (std::string name, unsigned size);

    /*!
     * listen_for_child listens for requestes from a child on a specific
     * tentacle. It then routes the request and executtes the proper octopOS
     * code to complete the request. Function signature is designed to be used
     * with pthreads_create.
     * @param tentacle_index The index in the global tentacle list is should be
     * listening on.
     * @return NULL
     */
    static void* listen_for_child(void* tentacle_index);

    /*!
     * propagate_to_subscribers sends a message to every subscriber of a topic
     * alerting it that the data has for the topic has been updated.
     * @param name The topic to propagate t.
     * @return true if propagation was successfull otherwise false.
     */
    bool propagate_to_subscribers(std::string name);

    /*!
     * subscribe_to_topic adds a subscriber to a topic's subscriber list.
     * @param name The name of the topic to subscribe to.
     * @param tentacle The tentacle that the subscription request is comming
     * from. Need for later routing.
     * @param subscriber_id The unique identifier of the subscriber.
     * @param size The size of the topic data. Needed only if the topic does
     * not exist so it can be created.
     * @return a pair where the first is the offset of the data object in the
     * shared memory segment, and the second is the key for the associated
     * semaphore
     */
    std::pair<unsigned, key_t> subscribe_to_topic(std::string name,
        unsigned tentacle, octopOS_id_t subscriber_id, long size = -1);           // NOLINT

    /*!
     * sig_handler provides a signal handler to clean up persistant resources
     * in the case of total failure.
     * @param sig The number identifier of the signal caught
     */
    static void sig_handler(int sig);

 private:
    /*!
     * get_id generates a unique identifier based off the role (publiser or
     * subscriber) of the caller.
     * @param role The role of the caller
     * @return uniqie identifier
     */
    static long get_id(tentacle::role_t role);                                    // NOLINT

    /*!
     * cheacks in a reader for topic_data
     */
    void topic_reader_in();

    /*!
     * cheacks out a reader for topic_data
     */
    void topic_reader_out();

    /*!
     * cheacks in a writer for topic_data
     */
    void topic_writer_in();

    /*!
     * cheacks in a writer for topic_data
     */
    void topic_writer_out();

    /*! The id of the shared memory segment */
    static int shmid;
    /*! The ids of the IPC message buss */
    static int tentacle_ids[NUMMODULES];
    /*! The ids of the used semaphores */
    static std::vector<int> semids;
    /*! The global list of tentacles used for communication with children */
    static std::vector<std::shared_ptr<tentacle>> tentacles;
    /*! Pointers to locations inside the shared memory segment. Used for
     * allocation */
    static intptr_t *shared_ptr, *shared_end_ptr;

    /*!
     * topic_data is the main struct of octopOS. It is a map that associates a
     * topic name with the rest of its data.
     * Each tuple consists of:
     *      1. offest of topics'a data in shared memory
     *      2. key for semaphores that controll access to the shared data
     *      3. array of subscribers
     * The array of subscribers consists of a pair of:
     *      1. topic ID
     *      2. tentacle ID
     */
    static std::unordered_map<std::string,
               std::tuple<unsigned,
                      key_t,
                      std::vector<std::pair<octopOS_id_t, unsigned>>
                        >
                   > topic_data;

    /*! mutex locks used to controll access to topic_data structure */
    static std::mutex topic_data_rdlock, topic_data_rdtry,
        topic_data_wrlock, topic_data_lock;
    /*! counters to implemnt reader/writer access for topic_data */
    static unsigned topic_data_readers, topic_data_writers;

    /*!
     * private constructor to prevent creation of multiple instances of
     * octopOS
     */
    octopOS();
};

#endif  // INCLUDE_OCTOPOS_H_
